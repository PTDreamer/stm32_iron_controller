/*
 * pid.h
 *
 *  Created on: Sep 11, 2017
 *      Author: jose
 */

#ifndef PID_H_
#define PID_H_

#include "stm32f1xx_hal.h"

void setupPID(double max, double min, double Kp, double Kd, double Ki );
double calculatePID( double setpoint, double pv );

#endif /* PID_H_ */
