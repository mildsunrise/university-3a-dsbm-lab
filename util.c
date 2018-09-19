/****************************************************

 u t i l . c

 Generic utilities

 *****************************************************/

#include "Base.h"  // Basic definitions
#include "util.h"   // Generic utilities

void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t line) {
    // MODERy[1:0] -> 01 General purpose output mode
    port->MODER = (port->MODER & ~(0b11 << line * 2)) | (0b01 << line * 2);

    // OTy -> 0 Push-pull output
    port->OTYPER = (port->OTYPER & ~(0b1 << line)) | (0b0 << line);

    // OSPEEDRy[1:0] -> 00 Speed 25MHz
    port->OSPEEDR = (port->OSPEEDR & ~(0b11 << line * 2)) | (0b00 << line * 2);

    // PUPDRy[1:0] -> 00 No pull-up, no pull-down
    port->PUPDR = (port->PUPDR & ~(0b11 << line * 2)) | (0b00 << line * 2);

    // ODRy -> 0
    port->ODR = (port->ODR & ~(0b1 << line)) | (0b0 << line);

    //FIXME: non atomic operations
}
