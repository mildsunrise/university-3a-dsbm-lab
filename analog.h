/**************************************************************

 a n a l o g . h

 ADC module header file

***************************************************************/

#ifndef _ANALOG_H   // Definition so that this file
#define _ANALOG_H   // is included only once

/************ Public function prototypes **************/

// Initialize the ADC peripheral and GPIO pins
void initADC(void);

// Read a single specified ADC channel
//      channel  : ADC channel to read
int32_t readChannel(int32_t channel);

// Read the internal temperature sensor; returned
// value is in tenths of Celsius degrees
int32_t readT(void);

// Using the ADC voltage reference, return Vdd voltage
// in millivolts
int32_t readVdd(void);

#endif // _ANALOG_H
