/*
 * screen.h
 *
 *  Created on: Jul 31, 2017
 *      Author: jose
 */

#ifndef GRAPHICS_GUI_SCREEN_H_
#define GRAPHICS_GUI_SCREEN_H_

#include "../generalIO/rotary_encoder.h"
#include "ssd1306.h"
#include "ugui.h"

typedef enum widgetStateType {widget_idle, widget_selected, widget_edit, widget_error}widgetStateType;
typedef enum widgetFieldType {field_float, field_integer, field_uinteger16, field_bmp, field_string}widgetFieldType;
typedef enum widgetType {widget_label, widget_display, widget_editable, widget_bmp}widgetType;
enum {screen_splash, screen_main};

typedef struct widget_t widget_t;

typedef struct displayOnly_wiget_t {
	void * (*getData)();
	widgetFieldType type;
	uint8_t	number_of_dec;
	void (*update)(widget_t*);
} displayOnly_wiget_t;

typedef struct editable_t {
	uint8_t (*processInput)(widget_t*, RE_Rotation_t, RE_State_t *);
	displayOnly_wiget_t inputData;
	void (*setData)(void *);
	int tab;
	uint8_t step;
	uint8_t big_step;
	widgetStateType state;
	widgetStateType previous_state;
	uint8_t max_value;
	uint8_t min_value;
} editable_wiget_t;

typedef struct bmp_wiget_t {
	UG_BMP bmp;
} bmp_wiget_t;

typedef struct label_wiget_t {

} label_wiget_t;

struct widget_t
{
	uint8_t inverted;
	char displayString[5];
	uint8_t reservedChars;
	uint8_t posX;
	uint8_t posY;
	UG_FONT *font_size;
	widget_t *next_widget;
	uint8_t enabled;
	widgetType type;
	struct screen_t *parent;
	void (*draw)(widget_t*);
	union {
		label_wiget_t label;
		editable_wiget_t editable;
		displayOnly_wiget_t displayWidget;
		bmp_wiget_t displayBmp;
	};
};

typedef struct screen_t screen_t;

struct screen_t
{
	struct screen_t *next_screen;
	widget_t *widgets;
	widget_t *current_widget;
	widgetStateType current_widget_state;
	uint8_t enabled;
	int (*processInput)(struct screen_t *scr, RE_Rotation_t input, RE_State_t *);
	void (*update)(screen_t *scr);
	void (*draw)(screen_t *scr);
	uint8_t index;
	void (*init)(screen_t *scr);
};

	widget_t *screen_tabToWidget(screen_t * scr, uint8_t tab);
	widget_t *screen_addWidget(screen_t * scr);
	void default_screenDraw(screen_t *scr);
	int default_screenProcessInput(screen_t * scr, RE_Rotation_t input, RE_State_t *);
	void default_screenUpdate(screen_t *scr);
	void default_init(screen_t *scr);

	void widgetDefaultsInit(widget_t *w, widgetType t);
	void default_widgetDraw(widget_t *widget);
	void default_widgetUpdate(widget_t *widget);
	uint8_t default_widgetProcessInput(widget_t *, RE_Rotation_t, RE_State_t *);

#endif /* GRAPHICS_GUI_SCREEN_H_ */
