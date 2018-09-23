/****************************************************

 l c d . c

 P2 LCD Functions

 *****************************************************/

#include "Base.h"  // Basic definitions
#include "lcd.h"   // LCD definitions
#include "util.h"  // Generic utilities

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
    GPIO_ModePushPull(LCD_PORT, LCD_BL_PAD);
    GPIO_ModePushPull(LCD_PORT, LCD_E_PAD);
    GPIO_ModePushPull(LCD_PORT, LCD_RS_PAD);
    GPIO_ModePushPull(LCD_PORT, LCD_DB4_PAD);
    GPIO_ModePushPull(LCD_PORT, LCD_DB5_PAD);
    GPIO_ModePushPull(LCD_PORT, LCD_DB6_PAD);
    GPIO_ModePushPull(LCD_PORT, LCD_DB7_PAD);
}

// Send 4 bits to the LCD and generates an enable pulse
//     nibbleCmd : Bits 3..0 Nibble to send to DB7...DB4
//     RS        : TRUE (RS="1")   FALSE (RS="0")

static void lcdNibble(uint32_t nibbleCmd, int32_t RS) {
    // Set RS and DB4..DB7 and sleep 10us
    LCD_PORT->BSRR.H.set = (nibbleCmd & 0b1111) << LCD_DB4_PAD | (RS ? LCD_RS_BIT : 0);
    LCD_PORT->BSRR.H.clear = (~nibbleCmd & 0b1111) << LCD_DB4_PAD | (!RS ? LCD_RS_BIT : 0);
    DELAY_US(10);
    // Enable E for 10us
    LCD_PORT->BSRR.H.set = LCD_E_BIT;
    DELAY_US(10);
    // Disable E and wait another 10us
    LCD_PORT->BSRR.H.clear = LCD_E_BIT;
    DELAY_US(10);
}

// Send a full 8-bit value to the LCD by first writing the higher nibble
// and then the lower nibble

static inline void lcdValue(uint32_t cmd, int32_t RS) {
    lcdNibble((cmd >> 4) & 0xF, RS);
    lcdNibble(cmd & 0xF, RS);
}


/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
 ******************************************************************/

// Backlight configuration
//     on evaluates to TRUE   Turn on backlight
//     on evaluates to FALSE  Turn off backlight

void LCD_Backlight(int32_t on) {
    if (on)
        LCD_PORT->BSRR.H.set = LCD_BL_BIT;
    else
        LCD_PORT->BSRR.H.clear = LCD_BL_BIT;
}

// Clear the LCD and set the cursor position at 0,0

void LCD_ClearDisplay(void) {
    lcdValue(0b00000001, 0);
    DELAY_US(1520);
}

// Configures the display
//     If Disp is TRUE turn on the display, if not, turns off
//     If Cursor is TRUE show the cursor, if not, hides it
//     If Blink is TRUE turn on blinking, if not, deactivate blinking

void LCD_Config(int32_t Disp, int32_t Cursor, int32_t Blink) {
    lcdValue(0b000001000 | (Disp ? BIT2 : 0) | (Cursor ? BIT1 : 0) | (Blink ? BIT0 : 0), 0);
    DELAY_US(40);
}

// Set the cursor at the given position
//    col: Columnn (0..LCD_COLUMNS-1)
//    row: Row     (0..LCD_ROWS-1)

void LCD_GotoXY(int32_t col, int32_t row) {
    // Restrict coordinates for safety
    if (row < 0 || row >= 2) row = 0;
    if (col < 0 || col >= 40) col = 0;

    // Send command and wait
    uint32_t address = row * 0x40 + col;
    lcdValue(0b10000000 | (address & 0b1111111), 0);
    DELAY_US(40);
}

// Send a character to the LCD at the current position
//     car: Charater to send

void LCD_SendChar(char car) {
    lcdValue(car, 1);
    DELAY_US(40); // FIXME: what time to wait for when writing to data?
}

// Send a string to the LCD at the current position
//     string: String to send

void LCD_SendString(char *string) {
    // Send every character except the NUL terminator
    for (; *string != '\0'; string++)
        LCD_SendChar(*string);
}

// Write a custom character to the CGRAM, and then goes to (0,0)
// to prevent corruption of the CGRAM
//     pos: Character to define (0..7)
//     data: Buffer of 8 bytes to write

void LCD_CustomChar(int32_t pos, uint8_t *data) {
    // Move to the corresponding CGRAM address
    uint32_t address = pos * 8;
    lcdValue(0b01000000 | (address & 0b111111), 0);
    DELAY_US(40);

    // Write each byte
    int32_t offset;
    for (offset = 0; offset < 8; offset++) {
        lcdValue(data[offset], 1);
        DELAY_US(40); // FIXME: what time to wait for when writing to data?
    }

    // Move back to DDRAM
    LCD_GotoXY(0, 0);
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

