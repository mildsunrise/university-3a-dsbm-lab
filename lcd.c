/****************************************************

 l c d . c

 P2 LCD Functions

 *****************************************************/

#include "Base.h"  // Basic definitions
#include "lcd.h"   // LCD definitions

// Some of the function need to be completed
// as is requested on the manual

/***************** PRIVATE FUNCTIONS TO BE IMPLEMENTED *************
 In C, all functions are public by default.
 Those functions, as they are not needed outside of lcd.c
 won't have prototypes on lcd.h.
 They will also be declared static so that the compiler can
 optimize their usage knowing they are not used outside this file.
 ****************************************************************/


// Initializes DB7..DB4, RS, E i BL in Push-Pull output mode
// and sets the ouptuts to "0"

static void lcdGPIOInit(void) {
    // This code is requested in P2.4

}

// Send 4 bits to the LCD and generates an enable pulse
//     nibbleCmd : Bits 3..0 Nibble to send to DB7...DB4
//     RS        : TRUE (RS="1")   FALSE (RS="0")

static void lcdNibble(int32_t nibbleCmd, int32_t RS) {
    // This code is requested in P2.6


}


/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
 ******************************************************************/

// Backlight configuration
//     on evaluates to TRUE   Turn on backlight
//     on evaluates to FALSE  Turn off backlight

void LCD_Backlight(int32_t on) {
    // This code is requested in P2.5

}

// Clear the LCD and set the cursor position at 0,0

void LCD_ClearDisplay(void) {
    // This code is requested in P2.7

}

// Configures the display
//     If Disp is TRUE turn on the display, if not, turns off
//     If Cursor is TRUE show the cursor, if not, hides it
//     If Blink is TRUE turn on blinking, if not, deactivate blinking

void LCD_Config(int32_t Disp, int32_t Cursor, int32_t Blink) {
    // This code is requested in P2.7

}

// Set the cursor at the given position
//    col: Columnn (0..LCD_COLUMNS-1)
//    row: Row     (0..LCD_ROWS-1)

void LCD_GotoXY(int32_t col, int32_t row) {
    // This code is requested in P2.7

}

// Send a character to the LCD at the current position
//     car: Charater to send

void LCD_SendChar(char car) {
    // This code is requested in P2.7

}

// Send a string to the LCD at the current position
//     string: String to send

void LCD_SendString(char *string) {
    // This code is requested in P2.7

}

/********** PUBLIC FUNCTIONS ALREADY IMPLEMENTED ***************
 Those functions are provided to reduce the codding effort
 for the LCD module
 ****************************************************************/

// Init the LCD in 4 bits mode
// This function depends on the implementation of the
// private functions lcdSendNibble and lcdGPIOInit

void LCD_Init(void) {
    // Initializes the LCD GPIOs
    lcdGPIOInit();

    // Wait 50ms for vdd to stabilize
    SLEEP_MS(50);

    // Init command 1
    lcdNibble(0x03, 0);
    SLEEP_MS(10); // Wait > 4.1ms

    // Repeat command
    lcdNibble(0x03, 0);

    // Wait > 100us
    DELAY_US(500);

    // Repeat command
    lcdNibble(0x03, 0);

    // Init command 2
    lcdNibble(0x02, 0);

    // Mode 4 bits 2 lines 5x8 chars
    lcdNibble(0x08, 0);
    lcdNibble(0x02, 0);
    DELAY_US(50);

    // Display on blink on and cursor on
    lcdNibble(0x00, 0);
    lcdNibble(0x0E, 0);
    DELAY_US(50);

    // Clear Display
    lcdNibble(0x00, 0);
    lcdNibble(0x01, 0);
    SLEEP_MS(5);

    // Cursor set direction
    lcdNibble(0x00, 0);
    lcdNibble(0x06, 0);
    DELAY_US(50);

    // Set ram address
    lcdNibble(0x08, 0);
    lcdNibble(0x00, 0);
    DELAY_US(50);

    // Send OK
    lcdNibble(0x04, 1);
    lcdNibble(0x0F, 1);
    DELAY_US(50);
    lcdNibble(0x04, 1);
    lcdNibble(0x0B, 1);
    DELAY_US(50);
}

