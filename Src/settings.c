/*
 * settings.c
 *
 *  Created on: Sep 13, 2017
 *      Author: jose
 */

#include "settings.h"
#define FLASH_ADDR (0x8000000|64512)/*Flash start OR'ed with the maximum amount of flash - 256 bytes*/

void saveSettings() {
	//First we erase the flash
	HAL_FLASH_Unlock(); //unlock flash writing
	//FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_EraseInitTypeDef erase;
	erase.NbPages = 1;
	erase.PageAddress = FLASH_ADDR;
	erase.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t error;
	HAL_FLASHEx_Erase(&erase, &error);
	//while (FLASH_ErasePage(FLASH_ADDR) != FLASH_COMPLETE)
		//; //wait for it
	//erased the chunk
	//now we program it
	uint16_t *data = (uint16_t*) &systemSettings;
	for (uint8_t i = 0; i < (sizeof(systemSettings) / 2); i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_ADDR + (i * 2), data[i]);
	}
	HAL_FLASH_Lock();
}

void restoreSettings() {
	//We read the flash
	uint16_t *data = (uint16_t*) &systemSettings;
	for (uint8_t i = 0; i < (sizeof(systemSettings) / 2); i++) {
		data[i] = *(uint16_t *) (FLASH_ADDR + (i * 2));
	}
	//if the version is correct were done
	//if not we reset and save
	if (systemSettings.version != SETTINGSVERSION) {
		//probably not setup
		resetSettings();
		saveSettings();
	}

}

void resetSettings() {

	systemSettings.PID.Kp = 0.00091328;
	systemSettings.PID.Kd = 0.00025182;
	systemSettings.PID.Ki = 0.000038516;
	systemSettings.PID.min = 0;
	systemSettings.PID.max = 1;
	systemSettings.version = SETTINGSVERSION;
	systemSettings.contrast = 0x7F;
	systemSettings.boost.temperature = 400;
	systemSettings.boost.time = 60;
	systemSettings.sleep.sleepTime = 60;
	systemSettings.sleep.standbyTime = 5;
	systemSettings.sleep.sleelpTemperature = 100;
	systemSettings.currentTip = 0;
	systemSettings.currentNumberOfTips = 1;
	strcpy(systemSettings.ironTips[0].name, "DFLT");
	systemSettings.ironTips[0].adc_at_300 = 1330;
	systemSettings.ironTips[0].b_200_300 = 48;
	systemSettings.ironTips[0].b_300_400 = -15.5304347826;
	systemSettings.ironTips[0].m_200_300 = 0.1893939394;
	systemSettings.ironTips[0].m_300_400 = 0.2173913043;

}
