/**************************************************************

 a n a l o g . c

 ADC module source file

***************************************************************/

#include "Base.h"    // Basic definitions
#include "analog.h"  // Header file for this module

// Initialize the ADC peripheral and GPIO pins
void initADC(void) {
    // Enable clock on ADC1
    RCC->APB2ENR = RCC_APB2ENR_ADC1EN;

    // Program ADC clock prescaler as fc(APB2) / 4 = 21MHz
    ADC->CCR = (ADC->CCR & (~ADC_CCR_ADCPRE)) | ADC_CCR_ADCPRE_DIV4;

    // Set PB0 as analog input (IN8) and disconnect pull ups
    GPIOB->MODER = (GPIOB->MODER & (~GPIO_MODER_MODER0)) | (0b11 * GPIO_MODER_MODER0_0);
    GPIOB->PUPDR = (GPIOB->PUPDR & (~GPIO_PUPDR_PUPDR0));
    // Program sample time for IN8
    ADC1->SMPR2 = (ADC1->SMPR2 & (~ADC_SMPR2_SMP8)) | ADC_SMPR2_SMP_AN8(ADC_SAMPLE_15);

    // Power up ADC1
    ADC1->CR2 |= ADC_CR2_ADON;
}

// Read a single specified ADC channel
//      channel  : ADC channel to read
int32_t readChannel(int32_t channel) {
    // Program channel to be read
    ADC1->SQR3 = (ADC1->SQR3 & (~ADC_SQR3_SQ1)) | ADC_SQR3_SQ1_N(channel);

    // Start conversion!
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Wait for conversion to end
    while ((ADC1->SR | ADC_SR_EOC) == 0);

    // Read converted value
    return ADC1->DR | ADC_DR_DATA;
}

// Read the internal temperature sensor; returned
// value is in tenths of Celsius degrees
int32_t readT(void) {
    
}

// Using the ADC voltage reference, return Vdd voltage
// in millivolts
int32_t readVdd(void) {
    
}
