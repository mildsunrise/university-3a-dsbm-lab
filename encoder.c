/**************************************************************

 e n c o d e r . c

 Quadrature encoder module

***************************************************************/

#include "Base.h"     // Basic definitions
#include "encoder.h"  // Header file for this module
#include "util.h"     // Generic utilities

volatile int32_t encoderCount = 0;

// Initialize the encoder GPIO pins and interrupts
void initEncoder(void) {
    // Configure pins as inputs w/ pull-up
    GPIO_ModeInput(ENC_PORT, ENC_A_PAD, 1);
    GPIO_ModeInput(ENC_PORT, ENC_B_PAD, 1);

    // Enable SYSCFG clock and configure EXTI1 and EXTI3 on our port (A)
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & (~SYSCFG_EXTICR1_EXTI1)) | SYSCFG_EXTICR1_EXTI1_PA;
    SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & (~SYSCFG_EXTICR1_EXTI3)) | SYSCFG_EXTICR1_EXTI3_PA;

    // Enable rising and falling triggers on EXTI1 and EXTI3, clear pending bits
    EXTI->RTSR |= EXTI_RTSR_TR1 | EXTI_RTSR_TR3;
    EXTI->FTSR |= EXTI_FTSR_TR1 | EXTI_FTSR_TR3;
    EXTI->PR = EXTI_PR_PR1 | EXTI_PR_PR3;

    // Enable interrupt vectors
    nvicEnableVector(EXTI1_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI1_IRQ_PRIORITY));
    nvicEnableVector(EXTI3_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI3_IRQ_PRIORITY));

    // Enable interrupts for EXTI1, program starts in A state
    EXTI->IMR |= EXTI_IMR_MR1;
}

// ISR for PA1 (line A)
CH_IRQ_HANDLER(EXTI1_IRQHandler) {
    CH_IRQ_PROLOGUE();

    // Mask interrupts at our line, clear pending bit
    EXTI->IMR &= ~(EXTI_IMR_MR1);
    EXTI->PR = EXTI_PR_PR1;

    // If it's a falling edge, update encoder count
    if ((ENC_PORT->IDR & ENC_A_BIT) == 0)
        encoderCount += ((ENC_PORT->IDR & ENC_B_BIT) != 0) ? +1 : -1;

    // Enable interrupts at other line, clear pending bit
    EXTI->IMR |= EXTI_IMR_MR3;
    EXTI->PR = EXTI_PR_PR3;

    CH_IRQ_EPILOGUE();
}

// ISR for PA3 (line B)
CH_IRQ_HANDLER(EXTI3_IRQHandler) {
    CH_IRQ_PROLOGUE();

    // Mask interrupts at our line, clear pending bit
    EXTI->IMR &= ~(EXTI_IMR_MR3);
    EXTI->PR = EXTI_PR_PR3;

    // Enable interrupts at other line, clear pending bit
    EXTI->IMR |= EXTI_IMR_MR1;
    EXTI->PR = EXTI_PR_PR1;

    CH_IRQ_EPILOGUE();
}
