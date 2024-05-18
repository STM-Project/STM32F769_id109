
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
	_FONT_SIZE_CircleName,
	_FONT_SIZE_PosLog,
	_FONT_SIZE_PosPhys,

	_FONT_STYLE_Title,
	_FONT_STYLE_CircleName,
	_FONT_STYLE_PosLog,
	_FONT_STYLE_PosPhys,

	_FONT_COLOR_Title,
	_FONT_COLOR_CircleName,
	_FONT_COLOR_PosLog,
	_FONT_COLOR_PosPhys,

	_COLOR_BkScreen,
	_COLOR_CircleFrame,
	_COLOR_CircleFill,
	_COLOR_CircleFillPress,

	_COEFF_COLOR_CircleName,
	_COEFF_COLOR_PosLog,
	_COEFF_COLOR_PosPhys,

	_DEBUG_ON

}_SCREEN_Calibration_enum;

void 	SCREEN_Calibration_main(void);

int 	SCREEN_Calibration_funcGet(int offs);
void 	SCREEN_Calibration_funcSet(int offs, int val);
void 	SCREEN_Calibration_setDefaultParam(void);
void 	SCREEN_Calibration_debug(void);

#endif /* LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_ */
