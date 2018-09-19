/**************************************************************

 i n t . c

 Interrupt test source file

***************************************************************/

#include "Base.h"     // Basic definitions
#include "int.h"      // This module header file
#include "lcd.h"      // LCD module header file
#include "accel.h"    // Accelerometer module header file

/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

static volatile int switchFlag;

// Configures the button (PA0) ISR
// PA0 is already configured as input in the halinit() call
// This function must:
//
//    * Activate the SYSCFG clock
//
//    * Assigna port A to EXTI0
//
//    * Enable the EXTI0 interrupt
//    * Acctivate EXTI0 on rising edge
//    * Clear the pending interrupt flag at EXTI0
//
//    * Enable EXTI0 at NVIC

void interruptTest(void) {
    // Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Set EXTI0 to pin PA0
    SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & (SYSCFG_EXTICR1_EXTI0)) | SYSCFG_EXTICR1_EXTI0_PA;

    // Enable EXTI0 on rising edges (IMR0 = RSTR0 = 1), clear pending request bit
    EXTI->IMR |= EXTI_IMR_MR0;
    EXTI->RTSR |= EXTI_RTSR_TR0;
    EXTI->PR = EXTI_PR_PR0;

    // Enable interrupt vector
    nvicEnableVector(EXTI0_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI0_IRQ_PRIORITY));

    // Setup display
    LCD_ClearDisplay();
    LCD_SendString("Y acceleration:");
    LCD_Config(TRUE, FALSE, FALSE);

    // Main loop
    while (TRUE) {
        // Clear flag
        switchFlag = FALSE;

        // Wait for it to become set
        while (!switchFlag);

        // Read acceleration and convert to string
        int32_t yacc = readAccel(LIS_R_OUT_Y, 1);
        char yaccStr [5];
        itoa(yacc, yaccStr, 10);

        // Write at display
        LCD_GotoXY(0, 1);
        LCD_SendString(yaccStr);
        LCD_SendString("   ");    // erase any left-over characters
    }
}


/********************* ISR FUNCTIONS ******************************
Those functions should never be called directly or indirectly from
the main program. They are automatically called by the NVIC
interrupt subsystem.
*******************************************************************/

// EXTI 0 ISR
// Associated to the user button
// Tasks to do:
//    * Erase the pending interrupt flag
//    * Change the green led status
//    * Activate the flag so that the accelerometer Y axis is read

CH_IRQ_HANDLER(EXTI0_IRQHandler) {
    CH_IRQ_PROLOGUE();
    // Start of the ISR code

    // Erase pending interrupt flag
    EXTI->PR = EXTI_PR_PR0;

    // Toggle green LED
    LEDS_PORT->ODR ^= GREEN_LED_BIT;

    // Activate flag
    switchFlag = TRUE;

    // End of the ISR code
    CH_IRQ_EPILOGUE();
}

