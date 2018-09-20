/**************************************************************

 k e y b o a r d . h

 4x4 matrix keyboard module

***************************************************************/

#ifndef _KEYBOARD_H   // Definition so that this file
#define _KEYBOARD_H   // is included only once

// Initialize the keyboard GPIO pins and resources
void initKeyboard(void);

// Initialize the interrupts for key detection,
// must be called after initKeyboard()
void initConfigKeyboard(void);

// Global variable where last detected key is saved
extern volatile int32_t detectedKey;

// Explore the keyboard looking for a single key, and return its keycode
// or KEY_NOT_FOUND if no pressed key was detected
int32_t readKeyboard(void);

// Explore the full keyboard looking for any pressed keys, and return
// a 16-bit mask with bits corresponding to pressed key codes set to 1.
// Careful! Some key combinations can lead to false presses, use
// verifyPresses() to make sure no false presses are present.
int32_t readMultikey(void);

// Given a pressed key mask, as returned by readMultikey(), verify
// that no incompatible key combination was found (return TRUE in that case).
int32_t verifyPresses(int32_t keys);

// CONSTANTS

#define KEY_NOT_FOUND 32

#define KEY_CHARS "123A" \
                  "456B" \
                  "789C" \
                  "*0#D"

#define KEY_CODE_0    13
#define KEY_CODE_1    0
#define KEY_CODE_2    1
#define KEY_CODE_3    2
#define KEY_CODE_4    4
#define KEY_CODE_5    5
#define KEY_CODE_6    6
#define KEY_CODE_7    8
#define KEY_CODE_8    9
#define KEY_CODE_9    10
#define KEY_CODE_STAR 12
#define KEY_CODE_HASH 14
#define KEY_CODE_A    3
#define KEY_CODE_B    7
#define KEY_CODE_C    11
#define KEY_CODE_D    15

#endif // _KEYBOARD_H

