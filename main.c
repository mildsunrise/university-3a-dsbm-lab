/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions
#include "lcd.h"      // LCD module header file
#include "accel.h"    // Accelerometer module header file
#include "int.h"      // Interrupt test program
#include "keyboard.h" // 4x4 keyboard module header file

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

// Acceleration value which maps to the first/last column in the LCD
#define DRAW_AXIS_SCALE 42
// Time to wait between refreshes
#define DRAW_AXIS_REFRESH 60
// Number of readings to average
#define DRAW_AXIS_AVG 6

inline int32_t clampColumn(int32_t col) {
    if (col < 0) return 0;
    if (col > 15) return 15;
    return col;
}

void accelDrawAxis(void) {
    // Initialize averaging buffers
    int32_t xacc_cache [DRAW_AXIS_AVG];
    int32_t yacc_cache [DRAW_AXIS_AVG];
    int32_t fill = 0, offset = 0;

    // Prepare LCD
    uint8_t bullet [] = {
        0b00000100,
        0b00000100,
        0b00001110,
        0b00011111,
        0b00011111,
        0b00001110,
        0b00000100,
        0b00000100,
    };
    LCD_ClearDisplay();
    LCD_Config(TRUE, FALSE, FALSE);
    LCD_Backlight(TRUE);
    LCD_CustomChar(2, bullet);

    // Read initial acceleration values
    int32_t xacc_0 = readAccel(LIS_R_OUT_X, 1);
    int32_t yacc_0 = readAccel(LIS_R_OUT_Y, 1);

    // Start the main loop
    while (1) {
        // Wait before reading
        SLEEP_MS(DRAW_AXIS_REFRESH);

        // Read acceleration on both axis, subtracted from initial
        int32_t xacc = readAccel(LIS_R_OUT_X, 1) - xacc_0;
        int32_t yacc = readAccel(LIS_R_OUT_Y, 1) - yacc_0;

        // Insert on buffers
        xacc_cache[offset] = xacc;
        yacc_cache[offset] = yacc;
        if (fill < DRAW_AXIS_AVG) fill++;

        // Get average value
        int32_t xacc_o = 0, yacc_o = 0, i;
        for (i = 0; i < fill; i++) {
            // FIXME: multiply by window function instead of plain average
            xacc_o += xacc_cache[(DRAW_AXIS_AVG + offset - i) % DRAW_AXIS_AVG];
            yacc_o += yacc_cache[(DRAW_AXIS_AVG + offset - i) % DRAW_AXIS_AVG];
        }
        xacc_o /= fill;
        yacc_o /= fill;

        // Move buffer slot
        offset = (offset + 1) % DRAW_AXIS_AVG;

        // Map acceleration to columns, and clamp
        // Formula: col = clamp(round(((acc_o / DRAW_AXIS_SCALE) + 1) / 2 * 15))
        int32_t xcol = clampColumn((xacc_o + DRAW_AXIS_SCALE) * 15 / (DRAW_AXIS_SCALE * 2));
        int32_t ycol = clampColumn((yacc_o + DRAW_AXIS_SCALE) * 15 / (DRAW_AXIS_SCALE * 2));

        // (Re)draw screen
        LCD_ClearDisplay();
        LCD_GotoXY(xcol, 0);
        LCD_SendChar(2);
        LCD_GotoXY(ycol, 1);
        LCD_SendChar(2);
    }
}

void keyboardPoll(void) {
    LCD_ClearDisplay();
    LCD_SendString("Pressed key:");
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Wait before reading
        SLEEP_MS(100);

        // Read current key code
        int32_t key = readKeyboard();

        // Write key char to LCD, or clear if no key
        LCD_GotoXY(0, 1);
        LCD_SendChar((key == KEY_NOT_FOUND) ? ' ' : KEY_CHARS[key]);
    }
}

void keyboardMultiPoll(void) {
    LCD_ClearDisplay();
    LCD_SendString("Pressed keys:");
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Wait before reading
        SLEEP_MS(100);

        // Read current keys
        int32_t keys = readMultikey();

        // Clear bottom row
        LCD_GotoXY(0, 1);
        LCD_SendString("            ");

        // Write each pressed key to LCD
        LCD_GotoXY(0, 1);
        int32_t key;
        for (key = 0; key < 16; key++) {
            if ((keys & BIT(key)) != 0)
                LCD_SendChar(KEY_CHARS[key]);
        }

        // Write error state to LCD
        LCD_GotoXY(16 - 4, 1);
        LCD_SendString(verifyPresses(keys) ? "     " : " ERR!");
    }
}

void keyboardPollInterrupt(void) {
    // Initialize interrupts and stuff
    initConfigKeyboard();

    // Initialize LCD
    LCD_ClearDisplay();
    LCD_SendString("Pressed key:");
    LCD_Config(TRUE, FALSE, FALSE);

    // Main loop
    while (1) {
        // Wait before reading
        SLEEP_MS(100);

        // Read current key code
        int32_t key = detectedKey;

        // Write key char to LCD, or clear if no key
        LCD_GotoXY(0, 1);
        LCD_SendChar((key == KEY_NOT_FOUND) ? ' ' : KEY_CHARS[key]);
    }
}

int main(void) {
    // Basic initializations
    baseInit();
    LCD_Init();
    initAccel();
    initKeyboard();

    // Keyboard single-key test *with interrupt detection*
    // This function never returns
    keyboardPollInterrupt();

    // Keyboard multiple-key test
    // This function never returns
    //keyboardMultiPoll();

    // Keyboard single-key test
    // This function never returns
    //keyboardPoll();

    // Interrupt test
    // This function never returns
    //interruptTest();

    // Final accelerator program
    // This function never returns
    //accelDrawAxis();

    // Show Y acceleration on LCD
    // This function never returns
    //accelPollY();

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

