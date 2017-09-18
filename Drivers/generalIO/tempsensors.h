/*
 * tempsensors.h
 *
 *  Created on: Jul 21, 2017
 *      Author: jose
 */

#ifndef GENERALIO_TEMPSENSORS_H_
#define GENERALIO_TEMPSENSORS_H_

#include "stm32f1xx_hal.h"
#include  "adc_global.h"

uint16_t readColdJunctionSensorTemp_mC(void);
uint16_t coldJunctionTemp_mC_To_uV(int tempX10);
uint16_t readTipSensorADC_Avg(void);
uint16_t readTipTemperatureCompensated(uint8_t new);
uint16_t realTempToADC(uint16_t real);

typedef struct tipData {
	float m_200_300;
	float b_200_300;
	float m_300_400;
	float b_300_400;
	uint16_t adc_at_300;
	char name[5];
} tipData;

tipData *currentTipData;
tipData calibratedTips[10];

#endif /* GENERALIO_TEMPSENSORS_H_ */
