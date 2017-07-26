/*
 * tempsensors.c
 *
 *  Created on: Jul 21, 2017
 *      Author: jose
 */

#include "tempsensors.h"
#include "math.h"

#define T0	310.189760024283
#define V0	12.6313864176219
#define P1	24.0619489545393
#define P2	4.01586218921246
#define P3	0.268539169819842
#define P4	-0.00971885437864459
#define Q1	0.169958719202669
#define Q2	0.0114130690852614
#define Q3	-0.000392751549487929
volatile uint16_t ADC1ConvertedValue[2];

uint16_t Get_ADC1Value(uint8_t i) {
	return ADC1ConvertedValue[i];
}
int readColdJunctionSensorTemp_X10(void) {
	static uint32_t rollingAverage[4];
	static uint8_t rIndex = 0;
	uint32_t ad_sum = 0;
	uint32_t max, min;
	uint32_t ad_value, avg_data, slide_data;

	uint8_t gMeas_cnt = 9;
	ad_sum = min = max = Get_ADC1Value(1);
	while (gMeas_cnt > 0) {
		ad_value = Get_ADC1Value(1);
		ad_sum += ad_value;
		if (ad_value > max)
			max = ad_value;
		if (ad_value < min)
			min = ad_value;
		gMeas_cnt--;
	}
	ad_sum = ad_sum - max - min;
	avg_data = ad_sum / 8;
	//^ Removes the two outliers from the data spread

	rollingAverage[rIndex] = avg_data; //store this result
	rIndex = (rIndex + 1) % 4; //move the index
	slide_data = (rollingAverage[0] + rollingAverage[1] + rollingAverage[2]
			+ rollingAverage[3]) / 4; //get the average
	//return slide_data;
	float NTC_RES = 10013.0 / ((4096.0 / (float)slide_data) - 1.0);
	float adc_read = log(NTC_RES);
	adc_read = 1 / (0.001129148 + (0.000234125 * adc_read) + (0.0000000876741 * adc_read * adc_read * adc_read));
	adc_read = adc_read - 273.15;  // Convert Kelvin to Celsius
	return adc_read * 10;

}

int readColdJunctionSensorTemp_X102(void) {
	static uint32_t rollingAverage[4];
	static uint8_t rIndex = 0;
	uint32_t ad_sum = 0;
	uint32_t max, min;
	uint32_t ad_value, avg_data, slide_data;

	uint8_t gMeas_cnt = 9;
	ad_sum = min = max = Get_ADC1Value(1);
	while (gMeas_cnt > 0) {
		ad_value = Get_ADC1Value(1);
		ad_sum += ad_value;
		if (ad_value > max)
			max = ad_value;
		if (ad_value < min)
			min = ad_value;
		gMeas_cnt--;
	}
	ad_sum = ad_sum - max - min;
	avg_data = ad_sum / 8;
	//^ Removes the two outliers from the data spread

	rollingAverage[rIndex] = avg_data; //store this result
	rIndex = (rIndex + 1) % 4; //move the index
	slide_data = (rollingAverage[0] + rollingAverage[1] + rollingAverage[2]
			+ rollingAverage[3]) / 4; //get the average
	//http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/temperature-measurement/ntc-thermistors/example-code-equations
	float x = (((float)slide_data) / 4096.0) - 0.5;
	float T = 25.0 + ( x * (-99.296 + x * (24.624 + x * 147.26))) / (1 + x * (0.0195 + x * 2.8454));
	return T * 10;
}

int coldJunctionTempTo_uV(int tempX10){
	float realTemp = tempX10 / 10;
	float tDiff = realTemp - 25.0;
	float nominator = (tDiff) * (0.0405148536182522 + tDiff * (-0.0000387896383378116 + tDiff * (-0.00000286084782705613 + tDiff * -0.000000000953670410293728)));
	float denominator = 1 + tDiff * (-0.00139486750341965 + (-0.0000679766265955419 * tDiff));
	float voltage = 1.0003453470161 + nominator / denominator;
	return voltage * 1000;
}

int readTipSensorADC_Avg(void) {
	static uint32_t rollingAverage[16];
	static uint8_t rIndex = 0;

	/*The head has a thermocouple inline with the heater
	 This is read by turning off the heater
	 Then read the output of the op-amp that is connected across the connections
	 */
	uint32_t ad_sum = 0;
	uint32_t max = 0, min;
	uint32_t ad_value, avg_data;
	//uint32_t timer = getIronTimer();
	//setIronTimer(0); //set the remaining time to zero
	//HEAT_OFF(); //heater must be off
	//delayMs(5); //wait for the heater to time out
	uint8_t gMeas_cnt = 9; //how many measurements to make
	max = ad_sum = min = Get_ADC1Value(0);

	while (gMeas_cnt > 0) {
		ad_value = Get_ADC1Value(0);
		ad_sum += ad_value;
		if (ad_value > max)
			max = ad_value;
		if (ad_value < min)
			min = ad_value;

		gMeas_cnt--;
	}
	//setIronTimer(timer);
	ad_sum = ad_sum - max - min; //remove the two outliers
	avg_data = ad_sum / 8; //take the average
	rollingAverage[rIndex] = avg_data;
	rIndex = (rIndex + 1) % 16;
	return (rollingAverage[0] + rollingAverage[1] + rollingAverage[2]
			+ rollingAverage[3] + rollingAverage[4] + rollingAverage[5]
			+ rollingAverage[6] + rollingAverage[7] + rollingAverage[8]
			+ rollingAverage[9] + rollingAverage[10] + rollingAverage[11]
			+ rollingAverage[12] + rollingAverage[13] + rollingAverage[14]
			+ rollingAverage[15]) / 16; //get the average

}

int readTipTemperatureCompensated(){
	static float gain = 100;
	float Vm = 3300 * (readTipSensorADC_Avg() / 4096.0);//(mV)
	float Vtc = Vm / gain + coldJunctionTempTo_uV(readColdJunctionSensorTemp_X10()) / 1000;
	float Vdiff = Vtc - V0;
	float T = T0 + (Vdiff * (P1 + Vdiff * (P2 + Vdiff * (P3 + P4 * Vdiff)))) / (1 + Vdiff * (Q1 + Vdiff * (Q2 + Q3 * Vdiff)));
	return T;
}
