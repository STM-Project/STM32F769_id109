
/*
 * SCREEN_CalibrationLCD.h
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */

#ifndef LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_
#define LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_

#include "stm32f7xx_hal.h"

typedef enum{
	_BK_SCREEN_color = 	10,
	_FONT_SIZE_Name = 	 1,
	_TITLE_color = 	10

}_SCREEN_Calibration_enum;

void Touchscreen_Calibration(void);

int* SCREEN_Calibration_funcGet(int offs);
void SCREEN_Calibration_funcSet(int offs, int val);
void SCREEN_Calibration_debug(void);

#endif /* LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_ */
