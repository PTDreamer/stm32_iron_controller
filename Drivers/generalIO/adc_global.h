/*
 * adc_global.h
 *
 *  Created on: Jul 27, 2017
 *      Author: jose
 */

#ifndef GENERALIO_ADC_GLOBAL_H_
#define GENERALIO_ADC_GLOBAL_H_

#include "stm32f1xx_hal.h"
void MX_ADC2_Init(ADC_HandleTypeDef * hadc2);
void MX_ADC1_Init(ADC_HandleTypeDef * hadc1);

typedef struct {
	uint16_t iron;
	uint16_t supply;
	uint16_t cold_junction;
	uint16_t reference;
} adc_measures_t;

extern volatile adc_measures_t adc_measures[10];

#endif /* GENERALIO_ADC_GLOBAL_H_ */
