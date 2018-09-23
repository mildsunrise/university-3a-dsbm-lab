/**************************************************************

 k e y b o a r d . c

 4x4 matrix keyboard module

***************************************************************/

#include "Base.h"     // Basic definitions
#include "keyboard.h" // Header file for this module
#include "util.h"     // Generic utilities

// Initialize the keyboard GPIO pins and resources
void initKeyboard(void) {
    // Put rows in open drain mode
    GPIO_ModeOpenDrain(KEY_PORT, KEY_ROW1_PAD);
    GPIO_ModeOpenDrain(KEY_PORT, KEY_ROW2_PAD);
    GPIO_ModeOpenDrain(KEY_PORT, KEY_ROW3_PAD);
    GPIO_ModeOpenDrain(KEY_PORT, KEY_ROW4_PAD);

    // Put columns in pull-up input mode
    GPIO_ModeInput(KEY_PORT, KEY_COL1_PAD, 1);
    GPIO_ModeInput(KEY_PORT, KEY_COL2_PAD, 1);
    GPIO_ModeInput(KEY_PORT, KEY_COL3_PAD, 1);
    GPIO_ModeInput(KEY_PORT, KEY_COL4_PAD, 1);
}

volatile int32_t detectedKey = KEY_NOT_FOUND;

// Initialize the interrupts for key detection,
// must be called after initKeyboard()
void initConfigKeyboard(void) {
    // Put all the rows in 0 (drain) state
    KEY_PORT->BSRR.H.clear = 0b1111 << KEY_ROW1_PAD;

    // Enable SYSCFG clock and configure EXTI9..EXTI6 on the column pins
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & (~SYSCFG_EXTICR2_EXTI6)) | SYSCFG_EXTICR2_EXTI6_PD;
    SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & (~SYSCFG_EXTICR2_EXTI7)) | SYSCFG_EXTICR2_EXTI7_PD;
    SYSCFG->EXTICR[2] = (SYSCFG->EXTICR[2] & (~SYSCFG_EXTICR3_EXTI8)) | SYSCFG_EXTICR3_EXTI8_PD;
    SYSCFG->EXTICR[2] = (SYSCFG->EXTICR[2] & (~SYSCFG_EXTICR3_EXTI9)) | SYSCFG_EXTICR3_EXTI9_PD;

    // Configure falling trigger interrupts for EXTI9..EXTI6, clear pending bits
    EXTI->IMR |= EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8 | EXTI_IMR_MR9;
    EXTI->RTSR &= ~(EXTI_RTSR_TR6 | EXTI_RTSR_TR7 | EXTI_RTSR_TR8 | EXTI_RTSR_TR9);
    EXTI->FTSR |= EXTI_FTSR_TR6 | EXTI_FTSR_TR7 | EXTI_FTSR_TR8 | EXTI_FTSR_TR9;
    EXTI->PR = EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9;

    // Enable interrupt vector
    nvicEnableVector(EXTI9_5_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI5_9_IRQ_PRIORITY));
}

// EXTI9..5 RSI associated to key presses
CH_IRQ_HANDLER(EXTI9_5_IRQHandler) {
    CH_IRQ_PROLOGUE();

    // Disable interrupts and put all rows as open
    EXTI->IMR &= ~(EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8 | EXTI_IMR_MR9);
    KEY_PORT->BSRR.H.set = 0b1111 << KEY_ROW1_PAD;

    // Explore the keyboard and set detected key
    int32_t key = readKeyboard();
    if (key != KEY_NOT_FOUND) detectedKey = key;

    // Return rows to drain, erase pending interrupt and enable interrupts again
    KEY_PORT->BSRR.H.clear = 0b1111 << KEY_ROW1_PAD;
    EXTI->PR = EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9;
    EXTI->IMR |= EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8 | EXTI_IMR_MR9;

    CH_IRQ_EPILOGUE();
}

// Explore the keyboard looking for a single key, and return its keycode
// or KEY_NOT_FOUND if no pressed key was detected
int32_t readKeyboard(void) {
    int32_t row;
    for (row = 0; row < 4; row++) {
        // Enable output at row
        KEY_PORT->BSRR.H.clear = BIT(KEY_ROW1_PAD + row);

        // Wait till lines are charged
        DELAY_US(8);

        // Read input pins set to 0
        int32_t cols = ((~KEY_PORT->IDR) >> KEY_COL1_PAD) & 0b1111;

        // Return row output to open
        KEY_PORT->BSRR.H.set = BIT(KEY_ROW1_PAD + row);

        // If a key was detected, find which one and return its code
        if (cols != 0) {
            int32_t col = 0;
            while ((cols & BIT(col)) == 0) col++;
            return (row << 2) | col;
        }
    }

    // No key was found. Sad.
    return KEY_NOT_FOUND;
}

// Explore the full keyboard looking for any pressed keys, and return
// a 16-bit mask with bits corresponding to pressed key codes set to 1.
// Careful! Some key combinations can lead to false presses, use
// verifyPresses() to make sure no false presses are present.
int32_t readMultikey(void) {
    int32_t keys = 0;
    int32_t row;
    for (row = 0; row < 4; row++) {
        // Enable output at row
        KEY_PORT->BSRR.H.clear = BIT(KEY_ROW1_PAD + row);

        // Wait till lines are charged
        DELAY_US(8);

        // Read input pins set to 0, and set bits on keys
        int32_t cols = ((~KEY_PORT->IDR) >> KEY_COL1_PAD) & 0b1111;
        keys |= (cols << (row * 4));

        // Return row output to open
        KEY_PORT->BSRR.H.set = BIT(KEY_ROW1_PAD + row);
    }

    return keys;
}

// Given a pressed key mask, as returned by readMultikey(), verify
// that no incompatible key combination was found (return TRUE in that case).
int32_t verifyPresses(int32_t keys) {
    // This variable is a bit mask that accumulates the rows (bits 7..4)
    // and columns (bits 3..0) we've seen so far on pressed keys
    int32_t rowsAndCols = 0;

    // Start processing each pressed key
    int32_t key;
    for (key = 0; key < 16; key++) {
        if ((keys & BIT(key)) != 0) {
            int32_t row = key >> 2, col = key & 0b11;

            // If we've already seen both the row and the column
            // of the key, then this is an invalid combination
            if ((rowsAndCols & BIT(row + 4)) && (rowsAndCols & BIT(col)))
                return FALSE;

            // Set bits to 1, to mark row and column as seen
            rowsAndCols |= BIT(row + 4) | BIT(col);
        }
    }

    return TRUE;
}
