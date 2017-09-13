/*
 * rotary_encoder.c

 *
 *  Created on: Jul 18, 2017
 *      Author: jose
 */

#include "rotary_encoder.h"

/* Return with status macro */
#define RETURN_WITH_STATUS(p, s) (p)->Rotation = s; return s

void RE_Init(RE_State_t* data, GPIO_TypeDef* GPIO_A_Port, uint16_t GPIO_A_Pin, GPIO_TypeDef* GPIO_B_Port, uint16_t GPIO_B_Pin, GPIO_TypeDef* GPIO_BUTTON_Port, uint16_t GPIO_BUTTON_Pin) {
	/* Save parameters */
	data->GPIO_A = GPIO_A_Port;
	data->GPIO_B = GPIO_B_Port;
	data->GPIO_PIN_A = GPIO_A_Pin;
	data->GPIO_PIN_B = GPIO_B_Pin;
	data->GPIO_BUTTON = GPIO_BUTTON_Port;
	data->GPIO_PIN_BUTTON = GPIO_BUTTON_Pin;
	data->pv_click = RE_BT_HIDLE;

	/* Set default mode */
	data->Mode = RE_Mode_One;

	/* Set default */
	data->RE_Count = 0;
	data->Diff = 0;
	data->Absolute = 0;
	data->LastA = 1;
}

RE_Rotation_t RE_Get(RE_State_t* data) {
	/* Calculate everything */
	data->Diff = data->RE_Count - data->Absolute;
	data->Absolute += data->Diff;

	/* Check */
	if(data->pv_click == RE_BT_CLICKED) {
		data->pv_click = RE_BT_HIDLE;
		RETURN_WITH_STATUS(data, Click);
	}
	else if (data->Diff < 0) {
		if(data->pv_click == RE_BT_DRAG) {
			//data->pv_click = RE_BT_HIDLE;
			RETURN_WITH_STATUS(data, Rotate_Decrement_while_click);
		}
			RETURN_WITH_STATUS(data, Rotate_Decrement);
	} else if (data->Diff > 0) {
		if(data->pv_click == RE_BT_DRAG) {
			//data->pv_click = RE_BT_HIDLE;
			RETURN_WITH_STATUS(data, Rotate_Increment_while_click);
		}
			RETURN_WITH_STATUS(data, Rotate_Increment);
	}
	RETURN_WITH_STATUS(data, Rotate_Nothing);
}

void RE_SetMode(RE_State_t* data, RE_Mode_t mode) {
	/* Set mode */
	data->Mode = mode;
}

void RE_Process(RE_State_t* data) {
	uint32_t debounce = 0;
	if((HAL_GetTick() - debounce) < 300)
		return;
	debounce = HAL_GetTick();
	uint8_t now_a;
	uint8_t now_b;
	uint8_t now_button;
	/* Read inputs */
	now_a = HAL_GPIO_ReadPin(data->GPIO_A, data->GPIO_PIN_A);
	now_b = HAL_GPIO_ReadPin(data->GPIO_B, data->GPIO_PIN_B);
	now_button = HAL_GPIO_ReadPin(data->GPIO_BUTTON, data->GPIO_PIN_BUTTON);
	/* Check difference */
	if (now_a != data->LastA) {
		data->LastA = now_a;
		if (data->LastA == 0) {
			/* Check mode */
			if(now_button == 0) {//button pressed
				data->pv_click = RE_BT_DRAG;
			}
			if (data->Mode == RE_Mode_Zero) {
				if (now_b == 1) {
					data->RE_Count--;
				} else {
					data->RE_Count++;
				}
			} else {
				if (now_b == 1) {
					data->RE_Count++;
				} else {
					data->RE_Count--;
				}
			}
		}
	}
	else if((data->pv_click == RE_BT_DRAG) && (now_button == 1))
		data->pv_click = RE_BT_HIDLE;
	else if(data->pv_click != RE_BT_DRAG) {
		if((data->pv_click == RE_BT_HIDLE) && (now_button == 0)) {
			data->pv_click = RE_BT_PRESSED;
		}
		if((data->pv_click == RE_BT_PRESSED) && (now_button == 1)) {
			data->pv_click = RE_BT_CLICKED;
		}
	}
}



