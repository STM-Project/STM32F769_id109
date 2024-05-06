/*
 * SCREEN_CalibrationLCD.h
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */

#ifndef LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_
#define LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_

#include "stm32f7xx_hal.h"

#define FILE_NAME(extand) SCREEN_Calibration_##extand

void Touchscreen_Calibration(void);
int* FILE_NAME(function)(void);

#endif /* LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_ */
