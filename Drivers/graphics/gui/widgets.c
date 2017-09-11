/*
 * widgets.c
 *
 *  Created on: Aug 11, 2017
 *      Author: jose
 */

#include "widgets.h"
#include "screen.h"

displayOnly_wiget_t * extractDisplayPartFromWidget(widget_t *widget) {
	switch (widget->type) {
		case widget_display:
			return &widget->displayWidget;
			break;
		case widget_editable:
			return &widget->editable.inputData;
			break;
		case widget_multi_option:
			return &widget->multiOptionWidget.editable.inputData;
		default:
			return NULL;
			break;
	}
}

editable_wiget_t * extractEditablePartFromWidget(widget_t *widget) {
	switch (widget->type) {
		case widget_editable:
			return &widget->editable;
			break;
		case widget_multi_option:
			return &widget->multiOptionWidget.editable;
		default:
			return NULL;
			break;
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
			w->editable.max_value = 0xFFFF;
			w->editable.min_value = 0;
			break;
		case widget_multi_option:
			w->multiOptionWidget.editable.big_step = 10;
			w->multiOptionWidget.editable.inputData.getData = NULL;
			w->multiOptionWidget.editable.inputData.number_of_dec = 0;
			w->multiOptionWidget.editable.inputData.type = field_uinteger16;
			w->multiOptionWidget.editable.inputData.update = &default_widgetUpdate;
			w->multiOptionWidget.editable.previous_state = widget_idle;
			w->multiOptionWidget.editable.setData = NULL;
			w->multiOptionWidget.editable.processInput = &default_widgetProcessInput;
			w->multiOptionWidget.editable.state = widget_idle;
			w->multiOptionWidget.editable.step = 1;
			w->multiOptionWidget.editable.tab = 0;
			w->multiOptionWidget.editable.max_value = 0xFF;
			w->multiOptionWidget.editable.min_value = 0;
			w->multiOptionWidget.currentOption = 0;
			w->multiOptionWidget.defaultOption = 0;
			w->multiOptionWidget.numberOfOptions = 0;
			w->multiOptionWidget.options = NULL;
			break;
		case widget_label:
			break;
		default:
			break;
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
	switch (widget->type) {
	case widget_multi_option:
		strcpy(widget->displayString, widget->multiOptionWidget.options[*(uint8_t*)data]);
		widget->multiOptionWidget.currentOption = *(uint8_t*)data;
		break;
	default:
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
	if((widget->type == widget_editable || widget->type == widget_multi_option)) {
		editable_wiget_t *edit = NULL;
		if(widget->type == widget_editable)
			edit = &widget->editable;
		else if(widget->type == widget_multi_option)
			edit = &widget->multiOptionWidget.editable;
		if((edit->state != widget_selected) && (edit->previous_state == widget_selected)) {
			draw_frame = 1;
			color = C_BLACK;
		}
			switch (edit->state) {
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
	char space[sizeof(widget->displayString)] = "           ";
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
	editable_wiget_t *edit = extractEditablePartFromWidget(widget);
	if(edit) {
		if(input == Click) {
			switch (edit->state) {
				case widget_selected:
					edit->state = widget_edit;
					edit->previous_state = widget_selected;
					break;
				case widget_edit:
					edit->state = widget_selected;
					edit->previous_state = widget_edit;
					break;
				default:
					break;
			}
			return 1;
		}
		if((widget->type == widget_editable) && (widget->editable.state == widget_edit)) {
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
		else if((widget->type == widget_multi_option) && (widget->editable.state == widget_edit)) {
			int temp = widget->multiOptionWidget.currentOption;
			if(input == Rotate_Increment)
				++temp;
			else if(input == Rotate_Decrement)
				--temp;
			if(temp < 0)
				temp = widget->multiOptionWidget.numberOfOptions - 1;
			else if(temp > widget->multiOptionWidget.numberOfOptions -1)
				temp = 0;
			widget->multiOptionWidget.editable.setData(&temp);
		}
		else if (edit->state == widget_selected) {
			uint8_t next = 0xFF;
			int previous = -1;
			widget_t *next_w = NULL;
			widget_t *previous_w = NULL;
			widget_t *first_w = widget;
			widget_t *last_w = widget;
			widget_t *w = widget->parent->widgets;
			while(w) {
				editable_wiget_t *e = extractEditablePartFromWidget(w);
				if(e) {
					if((e->tab > edit->tab) && (e->tab < next)) {
						next = e->tab;
						next_w = w;
					}
					if((e->tab < edit->tab) && (e->tab > previous)) {
						previous = e->tab;
						previous_w = w;
					}
					if(e->tab < extractEditablePartFromWidget(first_w)->tab)
						first_w = w;
					if(e->tab > extractEditablePartFromWidget(last_w)->tab)
						last_w = w;
				}
				w = w->next_widget;
			}
			if(next_w == NULL)
				next_w = first_w;
			if(previous_w == NULL)
				previous_w = last_w;
			if((input == Rotate_Increment) && next_w && (next_w != widget)) {
				edit->state = widget_idle;
				edit->previous_state = widget_selected;
				widget->parent->current_widget = next_w;
				widget->parent->current_widget_state = widget_idle;
				extractEditablePartFromWidget(next_w)->previous_state = extractEditablePartFromWidget(next_w)->state;
				extractEditablePartFromWidget(next_w)->state = widget_selected;
				return 1;
			}
			else if((input == Rotate_Decrement) && previous_w && (previous_w != widget)) {
				edit->state = widget_idle;
				edit->previous_state = widget_selected;
				widget->parent->current_widget = previous_w;
				widget->parent->current_widget_state = widget_idle;
				extractEditablePartFromWidget(previous_w)->previous_state = extractEditablePartFromWidget(previous_w)->state;
				extractEditablePartFromWidget(previous_w)->state = widget_selected;
				return 1;
			}
		}
	}
	return 0;
}
