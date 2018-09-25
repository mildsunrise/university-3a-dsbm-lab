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

    // Enable VREFINT and temperature sensor
    ADC->CCR |= ADC_CCR_TSVREFE;
    // Program sample time for IN16 (T) and IN17 (VREFINT) to ensure > 10us
    ADC1->SMPR1 = (ADC1->SMPR1 & (~ADC_SMPR1_SMP16)) | ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_480);
    ADC1->SMPR1 = (ADC1->SMPR1 & (~ADC_SMPR1_SMP17)) | ADC_SMPR1_SMP_VREF(ADC_SAMPLE_480);

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
    return ADC1->DR & ADC_DR_DATA;
}

// Temperature sensor values from datasheet
#define TS_AVG_SLOPE     25 // mV increment every 10ºC
#define TS_V25          760 // mV at 25ºC

// Read the internal temperature sensor; returned
// value is in tenths of Celsius degrees
int32_t readT(void) {
    // Read the temperature sensor and Vdd
    int32_t vsense = readChannel(ADC_CHANNEL_SENSOR);
    int32_t vdd = readVdd();

    // Calculate $T = 250 + \frac{\frac{\text{vsense}}{4095} \, V_{dd} - V_{25}}{\text{TS_AVG_SLOPE} / 100}$
    // Which results in $T = 250 + 100 \frac{\text{vsense} V_{dd} - 4095 \, V_{25}}{\text{4095 \, TS_AVG_SLOPE}}$
    return 250 + (100 * (vsense * vdd - 4095 * TS_V25)) / (4095 * TS_AVG_SLOPE);
}

// Voltage reference values from datasheet [mV]
#define VREFINT_MIN 1180
#define VREFINT_TYP 1210
#define VREFINT_MAX 1240

// Using the ADC voltage reference, return Vdd voltage
// in millivolts
int32_t readVdd(void) {
    // Read the voltage reference
    int32_t vrefint = readChannel(ADC_CHANNEL_VREFINT);

    // Calculate Vdd knowing $\text{vrefint} = 4095 \, \frac{V_{REFINT}}{V_{DD}}$
    // Thus, $V_{DD} = 4095 \, \frac{V_{REFINT}}{\text{vrefint}}$
    return (4095 * VREFINT_TYP) / vrefint;
}
