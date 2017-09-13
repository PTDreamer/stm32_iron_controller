/*
 * pid.h
 *
 *  Created on: Sep 11, 2017
 *      Author: jose
 */

#ifndef PID_H_
#define PID_H_

#include "stm32f1xx_hal.h"
typedef struct pid_values {
	double max;
	double min;
	double Kp;
	double Ki;
	double Kd;
} pid_values_t;

pid_values_t currentPID;

void setupPIDFromStruct();
void setupPID(double max, double min, double Kp, double Kd, double Ki );
double calculatePID( double setpoint, double pv );

#endif /* PID_H_ */
