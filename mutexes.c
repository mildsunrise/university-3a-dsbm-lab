/**************************************************************

 m u t e x e s . c

 Introduction to semaphores and mutexes source file

***************************************************************/

#include "Base.h"     // Basic definitions
#include "lcd.h"      // LCD module header file
#include "mutexes.h"  // This module header file

/******************* SEMAPHORE EXAMPLE ********************/

// Binary semaphore
BinarySemaphore semaf;

// Working area for the semaphore child thread
static WORKING_AREA(waSem, 128);

// Child thread function prototype
static msg_t thSem(void *arg);

// Process syncronization example that uses semaphores

void semaphoreExample(void) {
    // Global initialization
    baseInit();

    // Initializes the semaphore as not taken
    chBSemInit(&semaf, FALSE);

    // Creates a child thread
    chThdCreateStatic(waSem, sizeof (waSem), NORMALPRIO + 1, thSem, NULL);

    while (1) {
        SLEEP_MS(300);          // Wait 300ms
        chBSemSignal(&semaf);   // Send signal to child
    }
}


// Child thread that changes the state of the orange LED
// at each semaphore syncronization

static msg_t thSem(void *arg) {
    while (1) {
        chBSemWait(&semaf);                 // Wait for syncronization
        (LEDS_PORT->ODR) ^= ORANGE_LED_BIT; // Toggle LED
    }

    return 0;
}

/************************ MUTEX EXAMPLE *******************************/

// Working area for the child thread
static WORKING_AREA(waThEM, 128);

// Child thread function prototype
static msg_t thMtx(void *arg);

// Test where two threads access to the LCD
// The main thread write digits 0..9 on the first LCD line
// The child thread write letters A..Z on the second LCD line

void mutexExample(void) {
    int32_t x, i, car;

    // Global initialization
    baseInit();

    // Initialize the LCD
    LCD_Init();

    // Clear the LCD and turn off the backlight
    LCD_ClearDisplay();
    LCD_Backlight(0);

    // Creates a child thread
    chThdCreateStatic(waThEM, sizeof (waThEM), NORMALPRIO, thMtx, NULL);

    // First digit to show
    car = '0';

    // Infinite loop
    while (1) {
        for (i = 0; i < 20; i++)    // 20 times for each digit
            for (x = 0; x < LCD_COLUMNS; x++) { // For each column on the LCD...
                LCD_GotoXY(x, 0);       // Jump to that column
                LCD_SendChar(car);      // Write the digit
                DELAY_US(17000);        // Some delay
            }
        if (++car > '9') car = '0'; // Go to next digit
    }
}

// Child thread entry point
// Write letters on the second row of the LCD

static msg_t thMtx(void *arg) {
    int32_t x, i, car;

    // First char to show
    car = 'A';

    // Infinite loop
    while (1) {
        for (i = 0; i < 20; i++)    // 20 times for each char
            for (x = 0; x < LCD_COLUMNS; x++) { // For each LCD column
                LCD_GotoXY(x, 1);       // Jump to that column
                LCD_SendChar(car);      // Write the char
                DELAY_US2(10000);       // Some delay
            }
        if (++car > 'Z') car = 'A'; // Go to next char
    }
    return 0;
}

