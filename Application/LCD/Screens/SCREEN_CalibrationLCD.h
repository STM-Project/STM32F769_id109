/*
 * SCREEN_CalibrationLCD.h
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */

#ifndef LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_
#define LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_

#define aaa(str) SCREEN_Calibration_##str

extern struct NAZWA aaa(struct);  //w varioable.c odnies sie do tego tu miejsca poprzez *ptr

void Touchscreen_Calibration(void);

#endif /* LCD_SCREENS_SCREEN_CALIBRATIONLCD_H_ */
