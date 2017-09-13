/*
 * debug_screen.c
 *
 *  Created on: Aug 2, 2017
 *      Author: jose
 */

#include "settings_screen.h"
#include "../../../../Src/pid.h"

static widget_t *combo = NULL;
static volatile widget_t *problem = NULL;

static uint16_t KP = 0;
static uint16_t KI = 0;
static uint16_t KD = 0;

static void * getKp() {
	KP = currentPID.Kp * 10000000;
	return &KP;
}
static void setKp(uint16_t *val) {
	KP = *val;
	currentPID.Kp = (double)KP / 10000000;
	setupPIDFromStruct();
}
static void * getKi() {
	KI = currentPID.Ki * 10000000;
	return &KI;
}
static void setKi(uint16_t *val) {
	KI = *val;
	currentPID.Ki = (double)KI / 10000000;
	setupPIDFromStruct();
}
static void * getKd() {
	KD = currentPID.Kd * 10000000;
	return &KD;
}
static void setKd(uint16_t *val) {
	KD = *val;
	currentPID.Kd = (double)KD / 10000000;
	setupPIDFromStruct();
}
static void settings_screen_init(screen_t *scr) {
	UG_FontSetHSpace(0);
	UG_FontSetVSpace(0);
	default_init(scr);
	scr->current_widget = combo;
	scr->current_widget->comboBoxWidget.selectable.state = widget_selected;
}

void settings_screen_setup(screen_t *scr) {
	scr->draw = &default_screenDraw;
	scr->processInput = &default_screenProcessInput;
	scr->init = &settings_screen_init;
	scr->update = &default_screenUpdate;

	problem = screen_addWidget(scr);
	widgetDefaultsInit(problem, widget_label);
	char *s = "Settings";
	strcpy(problem->displayString, s);
	problem->posX = 10;
	problem->posY = 0;
	problem->font_size = &FONT_8X14;
	problem->reservedChars = 8;
	problem->draw = &default_widgetDraw;
	widget_t *widget = screen_addWidget(scr);
	widget->posX = 0;
	widgetDefaultsInit(widget, widget_combo);
	widget->posY = 17;
	widget->font_size = &FONT_6X8;
	comboAddItem(widget, "PID", screen_edit_pid);
	comboAddItem(widget, "SCREEN", 0);
	comboAddItem(widget, "BOOST", 0);
	comboAddItem(widget, "SLEEP", 0);
	comboAddItem(widget, "TIPS", 0);
	comboAddItem(widget, "EXIT", screen_main);
	combo = widget;

	screen_t *sc = oled_addScreen(screen_edit_pid);
	sc->draw = &default_screenDraw;
	sc->processInput = &default_screenProcessInput;
	sc->init = &default_init;
	sc->update = &default_screenUpdate;
	widget_t *w = screen_addWidget(sc);

	widgetDefaultsInit(w, widget_label);
	s = "PID";
	strcpy(w->displayString, s);
	w->posX = 50;
	w->posY = 0;
	w->font_size = &FONT_8X14;
	w->reservedChars = 3;

	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_label);
	s = "Kp:";
	strcpy(w->displayString, s);
	w->posX = 30;
	w->posY = 17;
	w->font_size = &FONT_6X8;
	w->reservedChars = 3;

	//
	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_editable);
	w->posX = 55;
	w->posY = 17;
	w->font_size = &FONT_6X8;
	w->editable.inputData.getData = &getKp;
	w->editable.inputData.number_of_dec = 2;
	w->editable.inputData.type = field_uinteger16;
	w->editable.big_step = 10;
	w->editable.step = 1;
	w->editable.selectable.tab = 0;
	w->editable.setData = &setKp;
	w->reservedChars = 6;

	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_label);
	s = "Ki:";
	strcpy(w->displayString, s);
	w->posX = 30;
	w->posY = 29;
	w->font_size = &FONT_6X8;
	w->reservedChars = 3;

	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_editable);
	w->posX = 55;
	w->posY = 29;
	w->font_size = &FONT_6X8;
	w->editable.inputData.getData = &getKi;
	w->editable.inputData.number_of_dec = 2;
	w->editable.inputData.type = field_uinteger16;
	w->editable.big_step = 10;
	w->editable.step = 1;
	w->editable.selectable.tab = 1;
	w->editable.setData = &setKi;
	w->reservedChars = 6;

	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_label);
	s = "Kd:";
	strcpy(w->displayString, s);
	w->posX = 30;
	w->posY = 41;
	w->font_size = &FONT_6X8;
	w->reservedChars = 3;

	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_editable);
	w->posX = 55;
	w->posY = 41;
	w->font_size = &FONT_6X8;
	w->editable.inputData.getData = &getKd;
	w->editable.inputData.number_of_dec = 2;
	w->editable.inputData.type = field_uinteger16;
	w->editable.big_step = 10;
	w->editable.step = 1;
	w->editable.selectable.tab = 2;
	w->editable.setData = &setKd;
	w->reservedChars = 6;

	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_button);
	w->font_size = &FONT_6X8;
	w->posX = 2;
	w->posY = 56;
	s = "SAVE";
	strcpy(w->displayString, s);
	w->reservedChars = 4;
	w->buttonWidget.selectable.tab = 3;
	w = screen_addWidget(sc);
	widgetDefaultsInit(w, widget_button);
	w->font_size = &FONT_6X8;
	w->posX = 90;
	w->posY = 56;
	s = "CANCEL";
	strcpy(w->displayString, s);
	w->reservedChars = 6;
	w->buttonWidget.selectable.tab = 4;
}

