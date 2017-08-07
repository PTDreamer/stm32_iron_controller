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
uint16_t readTipTemperatureCompensated();

#endif /* GENERALIO_TEMPSENSORS_H_ */
