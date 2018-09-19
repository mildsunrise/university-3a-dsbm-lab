/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions
#include "lcd.h"      // LCD module header file
#include "accel.h"    // Accelerometer module header file

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

void putNamesOnDisplay(void) {
    // Enable backlight
    LCD_Backlight(TRUE);
    // Define a character
    uint8_t e_acute [] = {
        0b00000000,
        0b00000001,
        0b00000111,
        0b00011111,
        0b00000111,
        0b00000001,
        0b00000000,
        0b00000000,
    };
    LCD_CustomChar(2, e_acute);
    // Write things
    LCD_ClearDisplay();
    LCD_SendString("Alba \x02\x02");
    LCD_GotoXY(0, 1);
    LCD_SendString("Mendez");
    // No cursor*/
    LCD_Config(TRUE, FALSE, FALSE);
}

void accelWhoAmI(void) {
    LCD_ClearDisplay();

    // Read register, convert to string
    int32_t value = readAccel(LIS_R_WHO_AM_I, 0);
    char valueStr [4];
    itoa(value, valueStr, 16);

    // Show in display
    LCD_SendString("Who_Am_I:");
    LCD_GotoXY(0, 1);
    LCD_SendString("0x");
    LCD_SendString(valueStr);
}

void accelPollY(void) {
    LCD_ClearDisplay();
    LCD_SendString("Y acceleration:");
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Wait before reading
        SLEEP_MS(150);

        // Read acceleration and convert to string
        int32_t yacc = readAccel(LIS_R_OUT_Y, 1);
        char yaccStr [5];
        itoa(yacc, yaccStr, 10);

        // Write at display
        LCD_GotoXY(0, 1);
        LCD_SendString(yaccStr);
        LCD_SendString("   ");    // erase any left-over characters
    }
}

int main(void) {
    // Basic initializations
    baseInit();
    LCD_Init();
    initAccel();

    // Show Y acceleration on LCD
    // This function never returns
    accelPollY();

    // Basic accelerometer test
    //accelWhoAmI();

    // LCD test
    //putNamesOnDisplay();

    // Call the LED sequence function
    // This function never returns
    ledSequence();

    // Call the LED blink function
    // This function never returns
    //ledBlink();

    // Return so that the compiler doesn't complain
    // It is not really needed as ledBlink never returns
    return 0;
}

