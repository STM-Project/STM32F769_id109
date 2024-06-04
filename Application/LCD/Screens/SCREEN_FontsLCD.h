/*
 * SCREEN_FontsLCD.h
 *
 *  Created on: May 16, 2024
 *      Author: RafałMarkielowski
 */

#ifndef LCD_SCREENS_SCREEN_FONTSLCD_H_
#define LCD_SCREENS_SCREEN_FONTSLCD_H_

#include "stm32f7xx_hal.h"

typedef enum{
	FONTS_FONT_SIZE_Title,
	FONTS_FONT_SIZE_FontColor,
	FONTS_FONT_SIZE_BkColor,
	FONTS_FONT_SIZE_FontType,
	FONTS_FONT_SIZE_FontSize,
	FONTS_FONT_SIZE_FontStyle,
	FONTS_FONT_SIZE_Coeff,
	FONTS_FONT_SIZE_LenWin,
	FONTS_FONT_SIZE_OffsWin,
	FONTS_FONT_SIZE_LoadFontTime,
	FONTS_FONT_SIZE_PosCursor,
	FONTS_FONT_SIZE_CPUusage,
	FONTS_FONT_SIZE_Speed,
	FONTS_FONT_SIZE_Fonts,

	FONTS_FONT_STYLE_Title,
	FONTS_FONT_STYLE_FontColor,
	FONTS_FONT_STYLE_BkColor,
	FONTS_FONT_STYLE_FontType,
	FONTS_FONT_STYLE_FontSize,
	FONTS_FONT_STYLE_FontStyle,
	FONTS_FONT_STYLE_Coeff,
	FONTS_FONT_STYLE_LenWin,
	FONTS_FONT_STYLE_OffsWin,
	FONTS_FONT_STYLE_LoadFontTime,
	FONTS_FONT_STYLE_PosCursor,
	FONTS_FONT_STYLE_CPUusage,
	FONTS_FONT_STYLE_Speed,
	FONTS_FONT_STYLE_Fonts,

	FONTS_FONT_COLOR_Title,
	FONTS_FONT_COLOR_FontColor,
	FONTS_FONT_COLOR_BkColor,
	FONTS_FONT_COLOR_FontType,
	FONTS_FONT_COLOR_FontSize,
	FONTS_FONT_COLOR_FontStyle,
	FONTS_FONT_COLOR_Coeff,
	FONTS_FONT_COLOR_LenWin,
	FONTS_FONT_COLOR_OffsWin,
	FONTS_FONT_COLOR_LoadFontTime,
	FONTS_FONT_COLOR_PosCursor,
	FONTS_FONT_COLOR_CPUusage,
	FONTS_FONT_COLOR_Speed,
	FONTS_FONT_COLOR_Fonts,

	FONTS_FONT_BKCOLOR_Title,
	FONTS_FONT_BKCOLOR_FontColor,
	FONTS_FONT_BKCOLOR_BkColor,
	FONTS_FONT_BKCOLOR_FontType,
	FONTS_FONT_BKCOLOR_FontSize,
	FONTS_FONT_BKCOLOR_FontStyle,
	FONTS_FONT_BKCOLOR_Coeff,
	FONTS_FONT_BKCOLOR_LenWin,
	FONTS_FONT_BKCOLOR_OffsWin,
	FONTS_FONT_BKCOLOR_LoadFontTime,
	FONTS_FONT_BKCOLOR_PosCursor,
	FONTS_FONT_BKCOLOR_CPUusage,
	FONTS_FONT_BKCOLOR_Speed,
	FONTS_FONT_BKCOLOR_Fonts,

	FONTS_SIZE_Press,
	FONTS_STYLE_Press,
	FONTS_COLOR_Press,
	FONTS_BKCOLOR_Press,

	FONTS_COLOR_BkScreen,
	FONTS_COLOR_MainFrame,
	FONTS_COLOR_Frame,
	FONTS_COLOR_FillMainFrame,
	FONTS_COLOR_FillFrame,
	FONTS_COLOR_FillFramePress,
	FONTS_DEBUG_ON,

}_SCREEN_Fonts_enum;

void SCREEN_Fonts_main(int argNmb, char **argVal);

int SCREEN_Fonts_funcGet(int offs);
void SCREEN_Fonts_funcSet(int offs, int val);
void SCREEN_Fonts_setDefaultAllParam(int rst);
void SCREEN_Fonts_printInfo(void);

void SCREEN_Fonts_debugRcvStr(void);
void SCREEN_Fonts_setTouch(void);

#endif /* LCD_SCREENS_SCREEN_FONTSLCD_H_ */
