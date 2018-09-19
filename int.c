/**************************************************************

 i n t . c

 Interrupt test source file

***************************************************************/

#include "Base.h"     // Basic definitions
#include "int.h"      // This module header file

/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

// Configures the button (PA0) ISR
// PA0 is already configured as input in the halinit() call
// This function must:
//
//    * Activate the SYSCFG clock
//
//    * Assigna port A to EXTI0
//
//    * Enable the EXTI0 interrupt
//    * Acctivate EXTI0 on rising edge
//    * Clear the pending interrupt flag at EXTI0
//
//    * Enable EXTI0 at NVIC

void interruptTest(void) {


}


/********************* ISR FUNCTIONS ******************************
Those functions should never be called directly or indirectly from
the main program. They are automatically called by the NVIC
interrupt subsystem.
*******************************************************************/

// EXTI 0 ISR
// Associated to the user button
// Tasks to do:
//    * Erase the pending interrupt flag
//    * Change the green led status
//    * Activate the flag so that the accelerometer Y axis is read

CH_IRQ_HANDLER(EXTI0_IRQHandler) {
    CH_IRQ_PROLOGUE();
    // Start of the ISR code



    // End of the ISR code
    CH_IRQ_EPILOGUE();
}

