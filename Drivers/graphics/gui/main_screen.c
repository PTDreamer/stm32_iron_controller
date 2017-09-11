/*
 * main_screen.c
 *
 *  Created on: Aug 2, 2017
 *      Author: jose
 */

#include "main_screen.h"
#include "tempsensors.h"

uint16_t test = 111;
uint16_t testm = 0;
static char *modestr[] = {"STB:", "BOO:", "SLP:", "SET:"};
static char *tipstr[] = {"TIP1", "TIP2", "TIP3", "TIP4"};
void * testGet() {
	return &test;
}

void testSet(uint16_t *value) {
	test = *value;
}

void * testGetM() {
	return &testm;
}

void testSetM(uint16_t *value) {
	testm = *value;
}

static uint16_t temp;
const unsigned char therm [] = {
		0x00, 0x00, 0x00, 0xC0, 0x20, 0xC0, 0x00, 0x00,
		0x00, 0x20, 0x70, 0xFF, 0xFE, 0xFF, 0x70, 0x20

};

static void * main_screen_getIronTemp() {
	temp = readTipTemperatureCompensated(0);
	return &temp;
}

static void * main_screen_getAmbTemp() {
	temp = readColdJunctionSensorTemp_mC() / 100;
	return &temp;
}

static void main_screen_init(screen_t *scr) {
	UG_FontSetHSpace(0);
	UG_FontSetVSpace(0);
	default_init(scr);
}

void main_screen_setup(screen_t *scr) {
	scr->draw = &default_screenDraw;
	scr->processInput = &default_screenProcessInput;
	scr->init = &main_screen_init;
	scr->update = &default_screenUpdate;

	//iron tip temperature display
	widget_t *widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_display);
	widget->posX = 45;
	widget->posY = 25;
	widget->font_size = &FONT_12X20;
	widget->displayWidget.getData = &main_screen_getIronTemp;
	widget->displayWidget.number_of_dec = 0;
	widget->displayWidget.type = field_uinteger16;
	widget->reservedChars = 3;

	//power display
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_display);
	widget->posX = 93;
	widget->posY = 1;
	widget->font_size = &FONT_8X14;
	widget->displayWidget.getData = &main_screen_getIronTemp;
	widget->displayWidget.number_of_dec = 0;
	widget->displayWidget.type = field_uinteger16;
	widget->reservedChars = 3;

	//ºC label next to iron tip temperature
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_label);
	char *s = "%";
	strcpy(widget->displayString, s);
	widget->posX = 119;
	widget->posY = 1;
	widget->font_size = &FONT_8X14;
	widget->reservedChars = 1;
	widget->draw = &default_widgetDraw;

	//ºC label next to iron tip temperature
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_label);
	s = "\247C";
	strcpy(widget->displayString, s);
	widget->posX = 50 + 3 * 12 -5;
	widget->posY = 20 + 5;
	widget->font_size = &FONT_12X20;
	widget->reservedChars = 2;
	widget->draw = &default_widgetDraw;

	//Thermometer bmp next to Ambient temperature
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_bmp);
	widget->posX = 85;
	widget->posY = 47;
	widget->displayBmp.bmp.p = therm;
	widget->displayBmp.bmp.width = 8;
	widget->displayBmp.bmp.height = 16;
	widget->displayBmp.bmp.colors = 2;
	widget->displayBmp.bmp.bpp = 8;

	//Ambient temperature display
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_display);
	widget->posX = 95;
	widget->posY = 50;
	widget->font_size = &FONT_8X14;
	widget->displayWidget.getData = &main_screen_getAmbTemp;
	widget->displayWidget.number_of_dec = 1;
	widget->displayWidget.type = field_uinteger16;
	widget->reservedChars = 4;


	// tip temperature setpoint
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_editable);
	widget->posX = 36;
	widget->posY = 1;
	widget->font_size = &FONT_8X14;
	widget->editable.inputData.getData = &testGet;
	widget->editable.inputData.number_of_dec = 0;
	widget->editable.inputData.type = field_uinteger16;
	widget->editable.big_step = 10;
	widget->editable.step = 1;
	widget->editable.tab = 0;
	widget->editable.setData = &testSet;
	widget->reservedChars = 3;
	widget->editable.state = widget_edit;

	// mode
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_multi_option);
	widget->posX = 1;
	widget->posY = 1;
	widget->font_size = &FONT_8X14;
	widget->multiOptionWidget.editable.inputData.getData = &testGetM;
	widget->multiOptionWidget.editable.inputData.number_of_dec = 0;
	widget->multiOptionWidget.editable.inputData.type = field_uinteger16;
	widget->multiOptionWidget.editable.big_step = 0;
	widget->multiOptionWidget.editable.step = 0;
	widget->multiOptionWidget.editable.tab = 2;
	widget->multiOptionWidget.editable.setData = &testSetM;

	widget->reservedChars = 4;

	widget->multiOptionWidget.options = modestr;
	widget->multiOptionWidget.numberOfOptions = 4;
	widget->multiOptionWidget.currentOption = 0;
	widget->multiOptionWidget.defaultOption = 0;

	// tips
	widget = screen_addWidget(scr);
	widgetDefaultsInit(widget, widget_multi_option);
	widget->posX = 1;
	widget->posY = 50;
	widget->font_size = &FONT_8X14;
	widget->multiOptionWidget.editable.inputData.getData = &testGetM;
	widget->multiOptionWidget.editable.inputData.number_of_dec = 0;
	widget->multiOptionWidget.editable.inputData.type = field_uinteger16;
	widget->multiOptionWidget.editable.big_step = 0;
	widget->multiOptionWidget.editable.step = 0;
	widget->multiOptionWidget.editable.tab = 1;
	widget->multiOptionWidget.editable.setData = &testSetM;

	widget->reservedChars = 4;

	widget->multiOptionWidget.options = tipstr;
	widget->multiOptionWidget.numberOfOptions = 4;
	widget->multiOptionWidget.currentOption = 0;
	widget->multiOptionWidget.defaultOption = 0;
}

