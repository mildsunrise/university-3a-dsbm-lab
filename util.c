/****************************************************

 u t i l . c

 Generic utilities

 *****************************************************/

#include "Base.h"  // Basic definitions
#include "util.h"   // Generic utilities

#define SET_BITS(value, offset, nbits, bits) \
    value = ((value) & ~((BIT(nbits)-1) << (offset))) | ((bits) << (offset));

// Configure a GPIO line as push-pull output, at the lowest speed,
// and write a low value
//     port: GPIO port
//     line: GPIO line to set as output
void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t line) {
    line &= 0xF;

    // MODERy[1:0] -> 01 General purpose output mode
    SET_BITS(port->MODER, 2*line, 2, 0b01);

    // OTy -> 0 Push-pull output
    SET_BITS(port->OTYPER, line, 1, 0);

    // OSPEEDRy[1:0] -> 00 Speed 25MHz
    SET_BITS(port->OSPEEDR, 2*line, 2, 0b00);

    // PUPDRy[1:0] -> 00 No pull-up, no pull-down
    SET_BITS(port->PUPDR, 2*line, 2, 0b00);

    // ODRy -> 0 Low
    SET_BITS(port->ODR, line, 1, 0);
}

// Configure a GPIO line as open drain output, at the lowest speed,
// and write a high value (open state)
//     port: GPIO port
//     line: GPIO line to set as output
void GPIO_ModeOpenDrain(GPIO_TypeDef *port, int32_t line) {
    line &= 0xF;

    // MODERy[1:0] -> 01 General purpose output mode
    SET_BITS(port->MODER, 2*line, 2, 0b01);

    // OTy -> 1 Open drain output
    SET_BITS(port->OTYPER, line, 1, 1);

    // OSPEEDRy[1:0] -> 00 Speed 25MHz
    SET_BITS(port->OSPEEDR, 2*line, 2, 0b00);

    // PUPDRy[1:0] -> 00 No pull-up, no pull-down
    SET_BITS(port->PUPDR, 2*line, 2, 0b00);

    // ODRy -> 1 Open
    SET_BITS(port->ODR, line, 1, 1);
}

// Configure a GPIO line as input
//     port: GPIO port
//     line: GPIO line to set as output
//     pullUpDown: 0 to disable pull-up / pull-down, 1 to enable pull-up
//                 resistor, 2 to enable pull-down resistor
void GPIO_ModeInput(GPIO_TypeDef *port, int32_t line, int32_t pullUpDown) {
    line &= 0xF;

    // MODERy[1:0] -> 00 Input
    SET_BITS(port->MODER, 2*line, 2, 0b00);

    // PUPDRy[1:0] -> [as requested]
    SET_BITS(port->PUPDR, 2*line, 2, pullUpDown & 0b11);

    // ODRy -> 1
    SET_BITS(port->ODR, line, 1, 1);
}

// Converts from integer to string using the indicated radix
// Modified from:
// https://www.fooe.net/trac/llvm-msp430/browser/trunk/mspgcc/msp430-libc/src/stdlib/itoa.c
// Sign is considered for all radix

// Integer to string conversion in the given radix
//      num:   Number to convert
//      str:   Pointer to the string where the result should be located
//               the user should reserve the needed space fo the string.
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal

char *itoa(int32_t num, char *str, int32_t radix) {
    int32_t sign = 0;   // To remember the sign if negative
    int32_t pos = 0;    // String position
    int32_t i;          // Generic use variable

    //Save sign
    if (num < 0) {
        sign = 1;
        num = -num;
    }

    //Construct a backward string of the number.
    do {
        i = num % radix;
        if (i < 10)
            str[pos] = i + '0';
        else
            str[pos] = i - 10 + 'A';
        pos++;
        num /= radix;
    }    while (num > 0);

    //Now add the sign
    if (sign)
        str[pos] = '-';
    else
        pos--;

    // Add the final null
    str[pos + 1] = 0;

    // Pos is now the position of the final digit (before null)

    // Now reverse the string
    i = 0;
    do {
        sign = str[i];
        str[i++] = str[pos];
        str[pos--] = sign;
    } while (i < pos);

    return str;
}

char *itoa_fix(int32_t num, char *str, int32_t radix, int32_t fixed) {
    int32_t sign = 0;   // To remember the sign if negative
    int32_t pos = 0;    // String position
    int32_t i;          // Generic use variable

    //Save sign
    if (num < 0) {
        sign = 1;
        num = -num;
    }

    //Construct a backward string of the number.
    do {
        i = num % radix;
        if (i < 10)
            str[pos] = i + '0';
        else
            str[pos] = i - 10 + 'A';
        pos++;
        num /= radix;

        if (fixed > 0 && pos == fixed) {
            str[pos] = '.';
            pos++;
        }
    }    while (num > 0 || pos <= fixed);

    //Now add the sign
    if (sign)
        str[pos] = '-';
    else
        pos--;

    // Add the final null
    str[pos + 1] = 0;

    // Pos is now the position of the final digit (before null)

    // Now reverse the string
    i = 0;
    do {
        sign = str[i];
        str[i++] = str[pos];
        str[pos--] = sign;
    } while (i < pos);

    return str;
}
