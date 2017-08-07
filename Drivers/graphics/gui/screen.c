/*
 * screen.cpp
 *
 *  Created on: Jul 31, 2017
 *      Author: jose
 */

#include "screen.h"
#include <stdlib.h>
#include "string.h"
#include <math.h>

static displayOnly_wiget_t * extractDisplayPartFromWidget(widget_t *widget) {
	switch (widget->type) {
		case widget_display:
			return &widget->displayWidget;
			break;
		case widget_editable:
			return &widget->editable.inputData;
		default:
			return NULL;
			break;
	}
}

widget_t *screen_addWidget(screen_t *scr) {
	widget_t *last_widget = NULL;
	if(scr->widgets) {
		last_widget = scr->widgets;
		while(last_widget->next_widget)
			last_widget = last_widget->next_widget;
	}

	widget_t *new_widget = malloc(sizeof(widget_t));
	if(new_widget == NULL)
		Error_Handler();
	new_widget->next_widget = NULL;
	new_widget->parent = scr;
	if(last_widget)
		last_widget->next_widget = new_widget;
	else
		scr->widgets = new_widget;
	return new_widget;
}

widget_t * screen_tabToWidget(screen_t * scr, uint8_t tab) {
	widget_t *last_widget = NULL;
	if(scr->widgets) {
		last_widget = scr->widgets;
		while(last_widget) {
			if(last_widget->type == widget_editable) {
				if(last_widget->editable.tab == tab)
					return last_widget;
			}
			last_widget = last_widget->next_widget;
		}
	}
	return last_widget;
}

int default_screenProcessInput(screen_t * scr, RE_Rotation_t input, RE_State_t *state) {
	if(scr->current_widget->editable.processInput && scr->current_widget && (scr->current_widget->type == widget_editable)) {
		if(scr->current_widget->editable.processInput(scr->current_widget, input, state))
		{
			return -1;
		}
	}
	int ret = -1;

	return ret;
}

void default_screenDraw(screen_t *scr) {
	widget_t *last_widget = NULL;
	if(scr->widgets) {
		last_widget = scr->widgets;
		while(last_widget) {
			if(last_widget->draw)
				last_widget->draw(last_widget);
			last_widget = last_widget->next_widget;
		}
	}
}

void default_screenUpdate(screen_t *scr) {
	widget_t *last_widget = NULL;
	if(scr->widgets) {
		last_widget = scr->widgets;
		while(last_widget) {
			if(last_widget->type == widget_display) {
				if(last_widget->displayWidget.update)
					last_widget->displayWidget.update(last_widget);
			}
			else if(last_widget->type == widget_editable) {
				if(last_widget->editable.inputData.update)
					last_widget->editable.inputData.update(last_widget);
			}
			last_widget = last_widget->next_widget;
		}

	}
}
void widgetDefaultsInit(widget_t *w, widgetType t) {
	w->type = t;
	w->draw = &default_widgetDraw;
	w->enabled = 1;
	w->font_size = &FONT_8X14;
	w->inverted = 0;
	w->posX = 0;
	w->posY = 0;
	w->reservedChars = 5;
	switch (t) {
		case widget_bmp:
			w->displayBmp.bmp.p = NULL;
			break;
		case widget_display:
			w->displayWidget.getData = NULL;
			w->displayWidget.number_of_dec = 0;
			w->displayWidget.type = field_uinteger16;
			w->displayWidget.update = &default_widgetUpdate;
			break;
		case widget_editable:
			w->editable.big_step = 10;
			w->editable.inputData.getData = NULL;
			w->editable.inputData.number_of_dec = 0;
			w->editable.inputData.type = field_uinteger16;
			w->editable.inputData.update = &default_widgetUpdate;
			w->editable.previous_state = widget_idle;
			w->editable.setData = NULL;
			w->editable.processInput = &default_widgetProcessInput;
			w->editable.state = widget_idle;
			w->editable.step = 1;
			w->editable.tab = 0;
			w->editable.max_value = 0xFF;
			w->editable.min_value = 0;
			break;
		case widget_label:
			break;
		default:
			break;
	}
}

void default_init(screen_t *scr) {
	scr->current_widget = NULL;
	if(!scr->widgets)
		return;
	int c = 1000;
	widget_t *w = scr->widgets;
	while(w) {
		if(w->type == widget_editable) {
			if(w->editable.tab < c) {
				c = w->editable.tab;
			}
		}
		w = w->next_widget;
	}
	w = scr->widgets;
	while(w) {
		if(w->type == widget_editable) {
			if(w->editable.tab == c) {
				scr->current_widget = w;
				if(w->editable.state == widget_idle)
					w->editable.state = widget_selected;
				return;
			}
		}
		w = w->next_widget;
	}
}



static void insertDot(char *str, uint8_t dec) {
	for(uint8_t x = strlen(str); x > strlen(str) - dec - 2; --x) {
		str[x + 1] = str[x];
	}
	str[strlen(str) - dec - 1] = '.';
}

void default_widgetUpdate(widget_t *widget) {
	//uint16_t x = 333;
	void * data;
	displayOnly_wiget_t *dis = extractDisplayPartFromWidget(widget);
	if(!dis)
		return;
	data = dis->getData();
	UG_FontSelect(widget->font_size);
	uint16_t val_ui16;
	switch (dis->type) {
		case field_uinteger16:
			val_ui16 = *((uint16_t*)(data));
			sprintf(widget->displayString,"%d", val_ui16);
			if(dis->number_of_dec) {
				insertDot(widget->displayString, dis->number_of_dec);
			}
			break;
		default:
			break;
	}
}

void default_widgetDraw(widget_t *widget) {
	if(!widget->enabled)
		return;
	//UG_SetBackcolor ( C_WHITE ) ;
	//UG_SetForecolor ( C_BLACK ) ;
	UG_FontSetHSpace(0);
	UG_FontSetVSpace(0);
	if(!widget->inverted) {
		UG_SetBackcolor ( C_BLACK ) ;
		UG_SetForecolor ( C_WHITE ) ;
	} else {
		UG_SetBackcolor ( C_WHITE ) ;
		UG_SetForecolor ( C_BLACK ) ;
	}
	UG_COLOR color;
	uint8_t draw_frame = 0;
	if(widget->type == widget_bmp) {
		UG_DrawBMP(widget->posX ,widget->posY , &widget->displayBmp.bmp);
		return;
	}
	if(widget->type == widget_editable) {
		if((widget->editable.state != widget_selected) && (widget->editable.previous_state == widget_selected)) {
			draw_frame = 1;
			color = C_BLACK;
		}
			switch (widget->editable.state) {
				case widget_edit:
					UG_SetBackcolor ( C_WHITE ) ;
					UG_SetForecolor ( C_BLACK ) ;
					break;
				case widget_selected:
					draw_frame = 1;
					color = C_WHITE;
					break;
				default:
					UG_SetBackcolor ( C_BLACK ) ;
					UG_SetForecolor ( C_WHITE ) ;
					break;
			}
	}
	UG_FontSelect(widget->font_size);
	char space[5] = "     ";
	space[widget->reservedChars] = (char)'\0';
	UG_PutString(widget->posX ,widget->posY , space);
	widget->displayString[widget->reservedChars] = (char)'\0';
	UG_PutString(widget->posX ,widget->posY , widget->displayString);
	if(draw_frame) {
		UG_DrawFrame(widget->posX - 1, widget->posY - 1,
											widget->posX + widget->reservedChars * widget->font_size->char_width + 1,
											widget->posY + widget->font_size->char_height -1, color);
	}
}

uint8_t default_widgetProcessInput(widget_t *widget, RE_Rotation_t input, RE_State_t *state) {
	if(input == Rotate_Nothing)
		return 1;
	if(widget->type == widget_editable) {
		if(input == Click) {
			switch (widget->editable.state) {
				case widget_selected:
					widget->editable.state = widget_edit;
					widget->editable.previous_state = widget_selected;
					break;
				case widget_edit:
					widget->editable.state = widget_selected;
					widget->editable.previous_state = widget_edit;
					break;
				default:
					break;
			}
			return 1;
		}
		if(widget->editable.state == widget_edit) {
			uint16_t ui16;
			int8_t inc;
			if(fabs(state->Diff) > 2) {
				inc = widget->editable.big_step;
				if(state->Diff < 0)
					inc = -1 * inc;
			}
			else
				inc = widget->editable.step * state->Diff;
			switch (widget->editable.inputData.type) {
			case field_uinteger16:
				ui16 = *(uint16_t*)widget->editable.inputData.getData();
				ui16 = ui16 + inc;
				if(ui16 > widget->editable.max_value)
					ui16 = widget->editable.max_value;
				else if(ui16 < widget->editable.min_value)
					ui16 = widget->editable.min_value;
				widget->editable.setData(&ui16);
				break;
			default:
				break;
			}
			return 1;
		}
		else if (widget->editable.state == widget_selected) {
			uint8_t next = 0xFF;
			int previous = -1;
			widget_t *next_w = NULL;
			widget_t *previous_w = NULL;
			widget_t *first_w = widget;
			widget_t *last_w = widget;
			widget_t *w = widget->parent->widgets;
			while(w) {
				if(w->type == widget_editable) {
					if((w->editable.tab > widget->editable.tab) && (w->editable.tab < next)) {
						next = w->editable.tab;
						next_w = w;
					}
					if((w->editable.tab < widget->editable.tab) && (w->editable.tab > previous)) {
						previous = w->editable.tab;
						previous_w = w;
					}
					if(w->editable.tab < first_w->editable.tab)
						first_w = w;
					if(w->editable.tab > last_w->editable.tab)
						last_w = w;
				}
				w = w->next_widget;
			}
			if(next_w == NULL)
				next_w = first_w;
			if(previous_w == NULL)
				previous_w = last_w;
			if((input == Rotate_Increment) && next_w && (next_w != widget)) {
				widget->editable.state = widget_idle;
				widget->editable.previous_state = widget_selected;
				widget->parent->current_widget = next_w;
				widget->parent->current_widget_state = widget_idle;
				next_w->editable.previous_state = next_w->editable.state;
				next_w->editable.state = widget_selected;
				return 1;
			}
			else if((input == Rotate_Decrement) && previous_w && (previous_w != widget)) {
				widget->editable.state = widget_idle;
				widget->editable.previous_state = widget_selected;
				widget->parent->current_widget = previous_w;
				widget->parent->current_widget_state = widget_idle;
				previous_w->editable.previous_state = previous_w->editable.state;
				previous_w->editable.state = widget_selected;
				return 1;
			}
		}
	}
	return 0;
}
