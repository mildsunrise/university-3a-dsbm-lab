/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions
#include "util.h"     // Generic utilities
#include "lcd.h"      // LCD module header file
#include "accel.h"    // Accelerometer module header file
#include "int.h"      // Interrupt test program
#include "keyboard.h" // 4x4 keyboard module header file
#include "encoder.h"  // Quadrature encoder module header file
#include "analog.h"   // ADC module header file
#include "process.h"  // Introduction to processes header file
#include "mutexes.h"  // Introduction to semaphores and mutexes header file

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

// P1.8

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

// P2.5

void backlightToggle(void) {
    LCD_Backlight(1);   // Turn on LCD backlight
    SLEEP_MS(2000);     // Wait 2s
    LCD_Backlight(0);   // Turn off LCD backlight
}

// P2.6

void lcdJustInit(void) {
    // Just (re)initialize the LCD and do nothing
    LCD_Init();
    while (1);
}

// P2.7

void lcdHello(void) {
    // Just (re)initialize the LCD and print "Hello"
    LCD_Init();
    LCD_SendString("Hello");
    while (1);
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

    while (1);
}

// P3.4

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
    LCD_SendString((value == 0x3B) ? " LIS302DL" : " LIS3DSH");

    while (1);
}

// P3.5

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

// P3.6

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

// C1.2

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

// C1.3

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

// C2.4

void potentiometerPoll(void) {
    LCD_ClearDisplay();
    LCD_SendString("Potentiometer:");
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Read acceleration and convert to string
        int32_t pot = readChannel(ADC_CHANNEL_POT);
        char potStr [5];
        itoa(pot, potStr, 10);

        // Write at display
        LCD_GotoXY(0, 1);
        LCD_SendString(potStr);
        LCD_SendString("   ");    // erase any left-over characters

        // Wait before next refresh
        SLEEP_MS(150);
    }
}

// C2.5

void temperaturePoll(void) {
    uint8_t degree [] = {
        0b00000110,
        0b00001001,
        0b00001001,
        0b00001001,
        0b00000110,
        0b00000000,
        0b00000000,
        0b00000000,
    };
    LCD_CustomChar(2, degree);
    LCD_ClearDisplay();
    LCD_SendString("Temperature:");
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Read temperature and convert to string
        int32_t t = readT();
        char tStr [8];
        itoa_fix(t, tStr, 10, 1);

        // Write at display
        LCD_GotoXY(0, 1);
        LCD_SendString(tStr);
        LCD_SendString(" \x02""C  ");

        // Wait before next refresh
        SLEEP_MS(200);
    }
}

// C2.6

void vddPoll(void) {
    LCD_ClearDisplay();
    LCD_SendString("Vdd =");
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Read temperature and convert to string
        int32_t vdd = readVdd();
        char vddStr [8];
        itoa_fix(vdd, vddStr, 10, 3);

        // Write at display
        LCD_GotoXY(6, 0);
        LCD_SendString(vddStr);
        LCD_SendString(" V");

        // Wait before next refresh
        SLEEP_MS(200);
    }
}

// C3.3

void encoderPoll(void) {
    int32_t pos;

    // Initialize LCD
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
    LCD_CustomChar(2, bullet);
    LCD_ClearDisplay();
    LCD_SendString("Encoder:");
    LCD_Config(TRUE, FALSE, FALSE);

    // Main loop
    while (1) {
        // Read encoder count, convert to string
        // We cast to int16_t to ensure smaller bounds (-32768 to 32767)
        int32_t count = (int16_t) encoderCount;
        char countStr [7];
        itoa(count, countStr, 10);

        // Update screen
        LCD_GotoXY(9, 0);
        LCD_SendString(countStr);
        for (pos = strlen(countStr); pos < 6; pos++)
            LCD_SendChar(' '); // fill rest of screen with spaces

        // Print indicator
        LCD_GotoXY((count + 8) & 0xF, 1);
        LCD_SendChar(2);

        // Sleep before reading again
        SLEEP_MS(100);

        // Erase indicator
        LCD_GotoXY((count + 8) & 0xF, 1);
        LCD_SendChar(' ');
    }
}

// R1.3

// Input parameters struct for the thread function
typedef struct { uint32_t bits; int32_t sleepTime; } FourThreadsChildParams;

// Thread function
static msg_t fourThreadsChild(void *arg) {
    FourThreadsChildParams *params = (FourThreadsChildParams*) arg;
    while (1) {
        LEDS_PORT->BSRR.H.set = params->bits;
        SLEEP_MS(params->sleepTime);
        LEDS_PORT->BSRR.H.clear = params->bits;
        SLEEP_MS(params->sleepTime);
    }
    return 0;
}

// Main program
void fourThreadsTest(void) {
    // Allocate working space for each child thread
    static struct { WORKING_AREA(wa, 128); } children [3];

    // Define parameters for each thread
    FourThreadsChildParams params [] = {
        { BLUE_LED_BIT, 300 },
        { ORANGE_LED_BIT, 500 },
        { GREEN_LED_BIT, 400 },
        { RED_LED_BIT, 700 },
    };

    // Start child threads
    int32_t i;
    for (i = 0; i < 3; i++)
        chThdCreateStatic(children[i].wa, sizeof(children[i].wa), NORMALPRIO, fourThreadsChild, &params[i+1]);

    // Start main thread's routine
    fourThreadsChild(&params[0]);
}



typedef struct {
    const char *text;
    void (*program)(void);
} MenuEntry;

MenuEntry menuEntries [] = {
    { "P1.8 LED Seq. ", ledSequence },
    { "P2.5 Backlight", backlightToggle },
    { "P2.6 LCD Init ", lcdJustInit },
    { "P2.7 LCD Hello", lcdHello },
    { "P2.7 LCD Names", putNamesOnDisplay },
    { "P3.4 Who_Am_I ", accelWhoAmI },
    { "P3.5 Accel Y  ", accelPollY },
    { "P3.6 Accel Dot", accelDrawAxis },
    { "P4.2 Interrupt", interruptTest },
    { "C1.2 Show key ", keyboardPoll },
    { "C1.2 Multi key", keyboardMultiPoll },
    { "C1.3 Intr. key", keyboardPollInterrupt },
    { "C2.4 Pot show ", potentiometerPoll },
    { "C2.5 Temp show", temperaturePoll },
    { "C2.6 Vdd show ", vddPoll },
    { "C3.3 Encoder  ", encoderPoll },
    { "R1.3 2Thd     ", test2threads },
    { "R1.3 2Thd +1  ", test2threadsPlus1 },
    { "R1.3 2Thd -1  ", test2threadsMinus1 },
    { "R1.3 2ThdSleep", test2threadsSleep },
    { "R1.3 4Thd     ", fourThreadsTest },
    { "R2.2 Sem      ", semaphoreExample },
    { "R2.2 Sem 2Thd ", semaphoreTwoThreads },
    { "R2.4 Mutex LCD", mutexExample },
};
#define MENU_ENTRIES_COUNT ((int32_t)(sizeof(menuEntries) / sizeof(MenuEntry)))

void programSelector(void) {
    int32_t shift = 0, selection = 0, lastShift = -1, markerRow = -1;
    int32_t lastEncoder = encoderCount;

    // Initialize LCD
    LCD_Config(TRUE, FALSE, FALSE);

    while (1) {
        // Draw screen
        // -----------

        if (shift != lastShift) {
            // Redraw text labels
            int32_t row;
            for (row = 0; row < LCD_ROWS; row++) {
                LCD_GotoXY(1, row);
                LCD_SendString(menuEntries[shift + row].text);
            }
            lastShift = shift;
        }

        if (markerRow != selection - shift) {
            // Erase old selection marker
            if (markerRow != -1) {
                LCD_GotoXY(0, markerRow);
                LCD_SendChar(' ');
            }
            // Draw marker in new position
            markerRow = selection - shift;
            LCD_GotoXY(0, markerRow);
            LCD_SendChar('>');
        }

        // Sleep and update state
        // ----------------------

        SLEEP_MS(50);

        // If user button is pressed, execute selected program
        // (this would be better more reliable with interrupts,
        // but setting an RSI would conflict with interruptTest)
        if ((BUTTON_PORT->IDR & BUTTON_BIT) != 0) {
            LCD_ClearDisplay();
            menuEntries[selection].program();
            while (1); // infinite loop, just in case the program returns
        }

        // Read encoder increment
        int32_t encoderIncrement = encoderCount - lastEncoder;
        lastEncoder += encoderIncrement;

        // Apply increment to selection, keeping it within valid bounds
        selection += encoderIncrement;
        if (selection < 0) selection = 0;
        if (selection >= MENU_ENTRIES_COUNT) selection = MENU_ENTRIES_COUNT - 1;

        // Scroll entries as needed to keep selection visible
        if (selection < shift) shift = selection;
        if (selection > (shift + LCD_ROWS - 1)) shift = selection - (LCD_ROWS - 1);
    }
}

int main(void) {
    // Basic initializations
    baseInit();
    LCD_Init();
    initAccel();
    initKeyboard();
    initEncoder();
    initADC();

    // Start selector
    programSelector();
    return 0;
}

