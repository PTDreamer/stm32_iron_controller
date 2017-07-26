/*
 * tempsensors.h
 *
 *  Created on: Jul 21, 2017
 *      Author: jose
 */

#ifndef GENERALIO_TEMPSENSORS_H_
#define GENERALIO_TEMPSENSORS_H_

#include "stm32f1xx_hal.h"

extern volatile uint16_t ADC1ConvertedValue[2];
uint16_t Get_ADC1Value(uint8_t i);
int readColdJunctionSensorTemp_X10(void);
int readColdJunctionSensorTemp_X102(void);
int coldJunctionTempTo_uV(int tempX10);
int readTipSensorADC_Avg(void);
int readTipTemperatureCompensated();

#endif /* GENERALIO_TEMPSENSORS_H_ */
