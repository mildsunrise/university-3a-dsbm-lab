/****************************************************

 u t i l . c

 Generic utilities

 *****************************************************/

#include "Base.h"  // Basic definitions
#include "util.h"   // Generic utilities

// FIXME: non atomic operations

#define SET_BITS(value, offset, nbits, bits) \
    value = (value & ~((BIT(nbits)-1) << offset)) | (bits << offset);

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
