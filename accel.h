/**************************************************************

 a c c e l . h

 Accelerometer module header file

***************************************************************/

#ifndef _ACCEL_H   // Definition so that this file
#define _ACCEL_H   // is included only once

/************ Public function prototypes **************/

// Init the accelerometer SPI bus
// This function has to be developed in the lab
void initAccel(void);

// Read an accelerometer register
//      reg   : Number of the register to read
//      sign  : If true, will read as int8_t (-128 a 127)
//              If false, will read as uint8_t (0 a 255)
//
// In case of error returns numbers greater than 1000
//             1001 : Try to access a reserved register

int32_t readAccel(int32_t reg,int32_t sign);

// Integer to string conversion in the given radix
//      num:   Number to convert
//      str:   Pointer to the string where the result should be located
//               the user should reserve the needed space fo the string.
//      radix: Radix to use. Typically it will be:
//                  2  Binary
//                  10 Decimal
//                  16 Hexadecimal
char *itoa(int32_t num, char *str, int32_t radix);

/************ Public constants **************/

// Register constants

#define LIS_R_WHO_AM_I          0x0F
#define LIS_R_CTRL_REG1         0x20
#define LIS_R_CTRL_REG2         0x21
#define LIS_R_CTRL_REG3         0x22
#define LIS_R_HP_FILTER_RESET   0x23
#define LIS_R_STATUS_REG        0x27
#define LIS_R_OUT_X             0x29
#define LIS_R_OUT_Y             0x2B
#define LIS_R_OUT_Z             0x2D

#endif // accel.h




