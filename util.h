/****************************************************

 u t i l . h

 Generic utilities

 General use functions called from other parts of the code

*****************************************************/

#ifndef _UTIL_H   // Definition so that the file
#define _UTIL_H   // is only included once

// Configure a GPIO line as push-pull output, at the lowest speed,
// and starting with a low value
//     port: GPIO port
//     line: GPIO line to set as output
void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t linia);

// Configure a GPIO line as open drain output, at the lowest speed,
// and write a high value (open state)
//     port: GPIO port
//     line: GPIO line to set as output
void GPIO_ModeOpenDrain(GPIO_TypeDef *port, int32_t line);

// Configure a GPIO line as input
//     port: GPIO port
//     line: GPIO line to set as output
//     pullUpDown: 0 to disable pull-up / pull-down, 1 to enable pull-up
//                 resistor, 2 to enable pull-down resistor
void GPIO_ModeInput(GPIO_TypeDef *port, int32_t line, int32_t pullUpDown);

// Integer to string conversion in the given radix
//      num:   Number to convert
//      str:   Pointer to the string where the result should be located
//               the user should reserve the needed space fo the string.
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal
char *itoa(int32_t num, char *str, int32_t radix);

// Fixed point number to string conversion in the given radix
//      num:   Number to convert
//      str:   Pointer to the string where the result should be located
//               the user should reserve the needed space fo the string.
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal
//      fixed: Amount of digits to show after the dot
char *itoa_fix(int32_t num, char *str, int32_t radix, int32_t fixed);

// Return length of string, excluding NUL terminator
static inline int32_t strlen(const char *str) {
    int32_t len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// String to integer conversion in the given radix
// Expects an option '-' or '+' sign, then a series of 0-9A-Z.
//      str:  String to convert
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal
int32_t atoi(const char *str, int32_t radix);

#endif //_UTIL_H
