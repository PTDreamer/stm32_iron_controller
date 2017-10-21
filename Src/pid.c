/*
 * pid.c
 *
 *  Created on: Sep 11, 2017
 *      Author: jose
 */

#include "pid.h"

static double max, min, Kp, Kd, Ki, pre_error, integral, p, i, d, mset, mpv;
uint32_t lastTime;

double getError() {
	return pre_error;
}
double getIntegral() {
	return integral;
}

void setupPIDFromStruct() {
	setupPID(currentPID.max, currentPID.min, currentPID.Kp, currentPID.Kd, currentPID.Ki);
}
void setupPID(double _max, double _min, double _Kp, double _Kd, double _Ki ) {
	max = _max;
	min = _min;
	Kp = _Kp;
	Kd = _Kd;
	Ki = _Ki;
	pre_error = 0;
	integral = 0;
}

void resetPID() {
	pre_error = 0;
	integral = 0;
}
double calculatePID( double setpoint, double pv )
{
	mset = setpoint;
	mpv = pv;
    double dt = (HAL_GetTick() - lastTime) ;
    dt = dt / 1000;
    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = Kp * error;

    // Integral term
    integral += error * dt;
    if(integral > 200)
    	integral = 200;
    else if(integral < -50)
        	integral = -50;
    double Iout = Ki * integral;

    // Derivative term
    double derivative;
    	if(error == pre_error)
    		derivative = 0;
    	else
    		derivative = (error - pre_error) / dt;
    double Dout = Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;
    p = Pout;
    i = Iout;
    d = Dout;
    // Restrict to max/min
    if( output > max )
        output = max;
    else if( output < min )
        output = min;

    // Save error to previous error
    pre_error = error;
    lastTime = HAL_GetTick();
    return output;
}
double getPID_D() {
	return d;
}
double getPID_P() {
	return p;
}
double getPID_I() {
	return i;
}
double getPID_SetPoint() {
	return mset;
}
double getPID_PresentValue() {
	return mpv;
}
