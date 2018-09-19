/****************************************************

 u t i l . h

 Generic utilities

 General use functions called from other parts of the code

*****************************************************/

#ifndef _UTIL_H   // Definition so that the file
#define _UTIL_H   // is only included once

// Configure a GPIO line as push-pull output, at the lowest speed,
// and write a low value
//     port: GPIO port
//     line: GPIO line to set as output
void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t linia);

#endif //_UTIL_H
