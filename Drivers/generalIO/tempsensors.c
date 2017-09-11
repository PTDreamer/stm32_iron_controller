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
#define VALIDITY_TRESHOLD 10000


static const uint16_t NTC_R = 1013;

uint16_t readColdJunctionSensorTemp_mC(void) {
	static uint32_t rollingAverage[4];
	static uint8_t rIndex = 0;
	uint32_t ad_sum = 0;
	uint32_t max, min;
	uint32_t ad_value, avg_data, slide_data;

	uint8_t gMeas_cnt = 9;
	ad_sum = min = max = adc_measures[0].cold_junction;
	while (gMeas_cnt > 0) {
		ad_value = adc_measures[gMeas_cnt].cold_junction;
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
	float NTC_RES = (float)(NTC_R) / ((4096.0 / (float)slide_data) - 1.0);
	float adc_read = log(NTC_RES);
	adc_read = 1000 / (0.001129148 + (0.000234125 * adc_read) + (0.0000000876741 * adc_read * adc_read * adc_read));
	adc_read = adc_read - 273150;  // Convert Kelvin to Celsius
	return adc_read;

}

uint16_t readTipTemperatureCompensated(uint8_t new){
	static uint16_t last_value;
	if(!new)
		return last_value;
	if(iron_temp_adc_avg < currentTipData->adc_at_300) {
		last_value = iron_temp_adc_avg * currentTipData->m_200_300 + currentTipData->b_200_300;
	}
	else {
		last_value = iron_temp_adc_avg * currentTipData->m_300_400 + currentTipData->b_300_400;
	}
	last_value = last_value + readColdJunctionSensorTemp_mC() / 1000;
	return last_value;
}

uint16_t realTempToADC(uint16_t real) {
	uint16_t comp_mC = real - readColdJunctionSensorTemp_mC() / 1000;
	uint16_t ret;
	if(real < 300) {
		ret = (comp_mC - currentTipData->b_200_300) / currentTipData->m_200_300;
	}
	else {
		ret = (comp_mC - currentTipData->b_300_400) / currentTipData->m_300_400;
	}
	return ret;
}
