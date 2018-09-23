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

// Write an accelerometer register
//      reg   : Number of the register to write
//      val   : Value to write, only 8 lower bits will be used
//
// In case of error returns numbers greater than 1000, otherwise 0
//             1001 : Try to access a reserved register
//             1002 : Try to access a read-only register

int32_t writeAccel(int32_t reg, int32_t val);

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
#define LIS_R_FF_WU_CFG_1       0x30
#define LIS_R_FF_WU_SRC_1       0x31
#define LIS_R_FF_WU_THS_1       0x32
#define LIS_R_FF_WU_DURATION_1  0x33
#define LIS_R_FF_WU_CFG_2       0x34
#define LIS_R_FF_WU_SRC_2       0x35
#define LIS_R_FF_WU_THS_2       0x36
#define LIS_R_FF_WU_DURATION_2  0x37
#define LIS_R_CLICK_CFG         0x38
#define LIS_R_CLICK_SRC         0x39
#define LIS_R_CLICK_THSY_X      0x3B
#define LIS_R_CLICK_THSZ        0x3C
#define LIS_R_CLICK_TIMELIMIT   0x3D
#define LIS_R_CLICK_LATENCY     0x3E
#define LIS_R_CLICK_WINDOW      0x3F

#endif // accel.h




