#ifndef ADC_H
#define ADC_H

#include "stm32f30x_conf.h"
#include <stdint.h>

/*
 * Assignment 2.4 - ADC
 *
 * ADC_setup_PA()
 * Configures ADC1 for measurements on PA0 and PA1.
 *
 * ADC_measure_PA()
 * Performs a single ADC measurement.
 * ch = 1 -> PA0 / ADC channel 1
 * ch = 2 -> PA1 / ADC channel 2
 */

void ADC_setup_PA(void);
uint16_t ADC_measure_PA(uint8_t ch);

#endif