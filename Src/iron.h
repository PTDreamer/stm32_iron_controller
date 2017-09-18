/*
 * iron.h
 *
 *  Created on: Sep 14, 2017
 *      Author: jose
 */

#ifndef IRON_H_
#define IRON_H_

#include "stm32f1xx_hal.h"

typedef struct ironBoost_t {
	uint16_t temperature;
	uint16_t time;
} ironBoost_t;

typedef struct ironSleep_t {
	uint16_t sleepTime;
	uint16_t standbyTime;
	uint16_t sleelpTemperature;
} ironSleep_t;

ironSleep_t currentSleepSettings;
ironBoost_t currentBoostSettings;

void applyBoostSettings();
void applySleepSettings();
#endif /* IRON_H_ */
