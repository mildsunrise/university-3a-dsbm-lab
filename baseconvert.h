/****************************************************

 b a s e c o n v e r t . h

 Custom program: Base converter

*****************************************************/

#ifndef _BASECONVERT_H   // Definition so that the file
#define _BASECONVERT_H   // is only included once

/** BASIC USER INPUT LAYER **/

#define INPUT_CONTROLLER_MULTIPLE (1 << 9)

// Translation / validation function
// A function that, given a pressed key and the current string,
// returns a translated character or -1 if the pressed key
// isn't valid (inputs nothing), -2 if the string is complete,
// or -3 if delete last character (backspace).
// If a valid character is returned with INPUT_CONTROLLER_MULTIPLE bit set,
// the key is considered incompletely entered.
typedef int32_t (*InputController)(char *string, int32_t len, int32_t key, int32_t var, void *arg);

// Prompt the user for a string
//    string: variable to store input string in
//    len: initial length of string (i.e. how many characters are already input)
//    controller: key to character translation / validation function
//    col, row: column and row where the first character of the string is input
// Returns new string length
// - When calling this function, any already-input characters should be already
//   visible on the screen and any free area should be cleared
// - When this function has returned, the cursor is disabled again but the
//   LCD stays positioned at the end of the string. WAIT TILL KEY IS RELEASED!
// - Before return, a NUL terminator is added at the end of the string (not
//   included in returned length), but make sure the translator never returns 0.
//   Thus make sure string has space for maxLength + 1 characters.
// - Function can break if encoder changes, managed through encoderBreak:
//   pointer is set to true if we broke from encoder, not changed otherwise,
//   pass NULL to ignore encoder
int32_t enterString(char *string, int32_t len, int32_t maxLength, InputController controller, void* arg, int32_t col, int32_t row, int32_t *encoderBreak);

// Implementation of a controller allowing user to enter digits
// of a given radix (base). arg must point to an int32_t specifying
// the input radix between 2 and 36. To enter characters above D, user
// presses D repeatedly.
int32_t numberInputController(char *string, int32_t len, int32_t key, int32_t var, void *arg);


/** PROGRAM **/

void baseConverter(void);


#endif //_BASECONVERT_H


