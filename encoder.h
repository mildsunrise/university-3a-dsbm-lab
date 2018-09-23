/**************************************************************

 e n c o d e r . h

 Quadrature encoder module

***************************************************************/

#ifndef _ENCODER_H    // Definition so that this file
#define _ENCODER_H    // is included only once

// Initialize the encoder GPIO pins and interrupts
void initEncoder(void);

// Global variable incremented or decremented when encoder is rotated
extern volatile int32_t encoderCount;

#endif // _ENCODER_H

