/*
 * SCREEN_ReadPanel.h
 *
 *  Created on: 14.04.2021
 *      Author: RafalMar
 */

#ifndef LCD_SCREENS_SCREEN_READPANEL_H_
#define LCD_SCREENS_SCREEN_READPANEL_H_

#include "LCD_fonts_images.h"

extern int argNmb;
extern char **argVal;
extern int startScreen;

void SCREEN_ReadPanel(void);

int ConfigTouchForStrVar(uint16_t ID_touch, uint16_t idx_touch, uint8_t param_touch, int idVar, StructTxtPxlLen lenStr);
int SetTouchForNewEndPos(int idVar, StructTxtPxlLen lenStr);

#endif /* LCD_SCREENS_SCREEN_READPANEL_H_ */
