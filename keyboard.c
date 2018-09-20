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
