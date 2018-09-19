/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions

// Function that blinks the green LED

void ledBlink(void) {
    while (1) {
        // Turn on the green LED using the BSRR register
        (LEDS_PORT->BSRR.H.set) = BIT(GREEN_LED_PAD);

        // Wait 200ms
        SLEEP_MS(200);

        // Turn off the green LED using the BSRR register
        (LEDS_PORT->BSRR.H.clear) = BIT(GREEN_LED_PAD);

        // Wait 200ms
        SLEEP_MS(200);
    }
}

int main(void) {
    // Basic initializations
    baseInit();

    // Call the LED blink function
    // This function never returns
    ledBlink();

    // Return so that the compiler doesn't complain
    // It is not really needed as ledBlink never returns
    return 0;
}

