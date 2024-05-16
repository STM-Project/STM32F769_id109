
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
	_FONT_SIZE_Title,
	_FONT_SIZE_Name,
	_FONT_SIZE_PosLog,
	_FONT_SIZE_PosPhys,

	_FONT_STYLE_Title,
	_FONT_STYLE_Name,
	_FONT_STYLE_PosLog,
	_FONT_STYLE_PosPhys,

	_BK_SCREEN_color,
	_CIRCLE_FRAME_color,
	_CIRCLE_FILL_color,
	_CIRCLE_FILL_pressColor,

	_TITLE_color,
	_NAME_color,
	_POS_LOG_color,
	_POS_PHYS_color,

	_MAXVAL_NAME,
	_MAXVAL_LOG,
	_MAXVAL_PHYS,

	_DEBUG_ON

}_SCREEN_Calibration_enum;

void 	SCREEN_Calibration_main		(void);

int 	SCREEN_Calibration_funcGet	(int offs);
void 	SCREEN_Calibration_funcSet	(int offs, int val);
void 	SCREEN_Calibration_debug	(void);

#endif /* LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_ */
