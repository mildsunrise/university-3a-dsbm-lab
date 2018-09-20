/****************************************************

 u t i l . c

 Generic utilities

 *****************************************************/

#include "Base.h"  // Basic definitions
#include "util.h"   // Generic utilities

// Configure a GPIO line as push-pull output, at the lowest speed,
// and write a low value
//     port: GPIO port
//     line: GPIO line to set as output
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

// Configure a GPIO line as open drain output, at the lowest speed,
// and write a high value (open state)
//     port: GPIO port
//     line: GPIO line to set as output
void GPIO_ModeOpenDrain(GPIO_TypeDef *port, int32_t line) {
    // MODERy[1:0] -> 01 General purpose output mode
    port->MODER = (port->MODER & ~(0b11 << line * 2)) | (0b01 << line * 2);

    // OTy -> 1 Open drain output
    port->OTYPER = (port->OTYPER & ~(0b1 << line)) | (0b1 << line);

    // OSPEEDRy[1:0] -> 00 Speed 25MHz
    port->OSPEEDR = (port->OSPEEDR & ~(0b11 << line * 2)) | (0b00 << line * 2);

    // PUPDRy[1:0] -> 00 No pull-up, no pull-down
    port->PUPDR = (port->PUPDR & ~(0b11 << line * 2)) | (0b00 << line * 2);

    // ODRy -> 1 Open
    port->ODR = (port->ODR & ~(0b1 << line)) | (0b1 << line);

    //FIXME: non atomic operations
}

// Configure a GPIO line as input
//     port: GPIO port
//     line: GPIO line to set as output
//     pullUpDown: 0 to disable pull-up / pull-down, 1 to enable pull-up
//                 resistor, 2 to enable pull-down resistor
void GPIO_ModeInput(GPIO_TypeDef *port, int32_t line, int32_t pullUpDown) {
    // MODERy[1:0] -> 00 Input
    port->MODER = (port->MODER & ~(0b11 << line * 2)) | (0b00 << line * 2);

    // PUPDRy[1:0] -> 00 No pull-up, no pull-down
    port->PUPDR = (port->PUPDR & ~(0b11 << line * 2)) | ((pullUpDown & 0b11) << line * 2);

    // ODRy -> 1
    port->ODR = (port->ODR & ~(0b1 << line)) | (0b1 << line);

    //FIXME: non atomic operations
}
