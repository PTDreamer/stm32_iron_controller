/*
 * pid.c
 *
 *  Created on: Sep 11, 2017
 *      Author: jose
 */

#include "pid.h"

static double max, min, Kp, Kd, Ki, pre_error, integral;
uint32_t lastTime;

void setupPID(double _max, double _min, double _Kp, double _Kd, double _Ki ) {
	max = _max;
	min = _min;
	Kp = _Kp;
	Kd = _Kd;
	Ki = _Ki;
	pre_error = 0;
	integral = 0;
}

double calculatePID( double setpoint, double pv )
{
    uint32_t dt = HAL_GetTick() - lastTime;
    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = Kp * error;

    // Integral term
    integral += error * dt;
    double Iout = Ki * integral;

    // Derivative term
    double derivative = (error - pre_error) / dt;
    double Dout = Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > max )
        output = max;
    else if( output < min )
        output = min;

    // Save error to previous error
    pre_error = error;

    return output;
}
