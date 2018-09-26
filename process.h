/**************************************************************

 p r o c e s s . c

 Introduction to processes header file

***************************************************************/

#ifndef _PROCESS_H   // Definitions so that
#define _PROCESS_H   // this file is included only once

/* Public function prototypes */

void test2threads(void);
void busyWait(uint32_t n);

void test2threadsPlus1(void);
void test2threadsMinus1(void);

void test2threadsSleep(void);

#endif /* process.h */


