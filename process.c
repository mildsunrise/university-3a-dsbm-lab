/**************************************************************

 p r o c e s s . c

 Introduction to processes source file

***************************************************************/

#include "Base.h"      // Basic definitions
#include "lcd.h"       // LCD module header file
#include "process.h"   // This module header file

/************** TWO THREAD SIMPLE EXAMPLE *********************/

// Working area for the child thread
static WORKING_AREA(waChild, 128);

// Function prototype for the child thread
// Needed because thChild is referenced before its definition
// It is not included in process.h because it's a static function
static msg_t thChild(void *arg);

// Test for simple two thread operation
// This function:
//     - Initializes the system
//     - Creates a child thread that blinks the orange LED
//     - Blinks the blue LED

void test2threads(void) {
    // Basic system initialization
    baseInit();

    // Child thread creation
    chThdCreateStatic(waChild, sizeof (waChild), NORMALPRIO, thChild, NULL);

    while (TRUE) {
        // Turn on blue LED using BSRR
        (LEDS_PORT->BSRR.H.set) = BLUE_LED_BIT;

        // Pause
        busyWait(3);

        // Turn off blue LED using BSRR
        (LEDS_PORT->BSRR.H.clear) = BLUE_LED_BIT;

        // Pausa
        busyWait(3);
    }
}


// Child thread that bliks the orange LED

static msg_t thChild(void *arg) {
    while (TRUE) {
        // Turn on orange LED using BSRR
        (LEDS_PORT->BSRR.H.set) = ORANGE_LED_BIT;

        // Pausa
        busyWait(5);

        // Turn off orange LED using BSRR
        (LEDS_PORT->BSRR.H.clear) = ORANGE_LED_BIT;

        // Pausa
        busyWait(5);
    }
    return 0;
}

// Busy waits in a thread doing some operations
// The greater n, the longer the wait

void busyWait(uint32_t n) {
    uint32_t i;
    volatile uint32_t x = 0;
    for (i = 0; i < n * 1000000; i++)
        x = (x + 2) / 3;
}


/************ TWO THREAD SIMPLE EXAMPLE ENDS *******************/

