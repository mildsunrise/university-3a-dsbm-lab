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
