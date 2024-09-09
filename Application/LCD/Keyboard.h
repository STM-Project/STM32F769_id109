/*
 * Keyboard.h
 *
 *  Created on: Sep 8, 2024
 *      Author: maraf
 */

#ifndef LCD_KEYBOARD_H_
#define LCD_KEYBOARD_H_

#include "stm32f7xx_hal.h"
#include "common.h"

typedef struct{
	figureShape shape;
	uint8_t bold;
	uint16_t x;
	uint16_t y;
	uint16_t widthKey;
	uint16_t heightKey;
	uint16_t widthKey2;
	uint16_t heightKey2;
	uint8_t interSpace;
	uint8_t forTouchIdx;
	uint8_t startTouchIdx;
	uint8_t nmbTouch;
	uint8_t param;
	uint16_t param2;
}KEYBOARD_SETTINGS;

extern KEYBOARD_SETTINGS s[], c;

void _deleteAllTouchs(void);
void _deleteAllTouchParams(void);
void _deleteTouchs(int nr);
void _deleteTouchParams(int nr);

#endif /* LCD_KEYBOARD_H_ */
