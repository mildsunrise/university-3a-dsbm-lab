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

#define TURN_ON(bits) (LEDS_PORT->BSRR.H.set) = (bits);
#define TURN_OFF(bits) (LEDS_PORT->BSRR.H.clear) = (bits);

#define TURN_ON_DURING(time, bits) \
        TURN_ON(bits); SLEEP_MS(time); TURN_OFF(bits);

void ledSequence(void) {
    while (1) {
        TURN_ON_DURING(500, GREEN_LED_BIT);
        TURN_ON_DURING(500, ORANGE_LED_BIT);
        TURN_ON_DURING(500, RED_LED_BIT);
        TURN_ON_DURING(500, BLUE_LED_BIT);
    }
}

int main(void) {
    // Basic initializations
    baseInit();

    // Call the LED blink function
    // This function never returns
    //ledBlink();

    // Call the LED sequence function
    // This function never returns
    ledSequence();

    // Return so that the compiler doesn't complain
    // It is not really needed as ledBlink never returns
    return 0;
}

