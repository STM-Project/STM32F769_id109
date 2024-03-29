/*
 * LCD_Common.h
 *
 *  Created on: 11.04.2021
 *      Author: Elektronika RM
 */

#ifndef LCD_LCD_COMMON_H_
#define LCD_LCD_COMMON_H_

#define BLUE          ((uint32_t)0xFF0000FF)
#define GREEN         ((uint32_t)0xFF00FF00)
#define RED           ((uint32_t)0xFFFF0000)
#define CYAN          ((uint32_t)0xFF00FFFF)
#define MAGENTA       ((uint32_t)0xFFFF00FF)
#define YELLOW        ((uint32_t)0xFFFFFF00)
#define LIGHTBLUE     ((uint32_t)0xFF8080FF)
#define LIGHTGREEN    ((uint32_t)0xFF80FF80)
#define LIGHTRED      ((uint32_t)0xFFFF8080)
#define LIGHTCYAN     ((uint32_t)0xFF80FFFF)
#define LIGHTMAGENTA  ((uint32_t)0xFFFF80FF)
#define LIGHTYELLOW   ((uint32_t)0xFFFFFF80)
#define DARKBLUE      ((uint32_t)0xFF000080)
#define DARKGREEN     ((uint32_t)0xFF008000)
#define DARKRED       ((uint32_t)0xFF800000)
#define DARKCYAN      ((uint32_t)0xFF008080)
#define DARKMAGENTA   ((uint32_t)0xFF800080)
#define DARKYELLOW    ((uint32_t)0xFF808000)
#define WHITE         ((uint32_t)0xFFFFFFFF)
#define LIGHTGRAY     ((uint32_t)0xFFD0D0D0)
#define GRAY          ((uint32_t)0xFF808080)
#define DARKGRAY      ((uint32_t)0xFF404040)
#define BLACK         ((uint32_t)0xFF000000)
#define BROWN         ((uint32_t)0xFFA52A2A)
#define ORANGE        ((uint32_t)0xFFFFA500)
#define TRANSPARENT   ((uint32_t)0x00000000)

#define MYGRAY    	 ((uint32_t)0xFF404040)
#define MYBLUE    	 ((uint32_t)0xFF50C8FF)
#define MYRED    	 	 ((uint32_t)0xffff7878)
#define MYGREEN    	 ((uint32_t)0xff50FF50)
#define MYLIGHTGRAY   ((uint32_t)0xFFB0B0B0)

#define RGB2INT(R,G,B)		(R<<16|G<<8|B)|0xFF000000
#define R_PART(rgb)		(rgb>>16)&0x000000FF
#define G_PART(rgb)		(rgb>>8)&0x000000FF
#define B_PART(rgb)		 rgb&0x000000FF

#define COLOR_TO_Y(color)		(0.299*((color>>16)&0x000000FF) + 0.587*((color>>8)&0x000000FF) + 0.114*(color&0x000000FF))
#define COLOR_TO_Cb(color)		(128 - 0.168736*((color>>16)&0x000000FF) - 0.331264*((color>>8)&0x000000FF) + 0.5*(color&0x000000FF))
#define COLOR_TO_Cr(color)		(128 +      0.5*((color>>16)&0x000000FF) - 0.418688*((color>>8)&0x000000FF) - 0.081312*(color&0x000000FF))

#define _Y(r,g,b)		(0.299*r + 0.587*g + 0.114*b)
#define _Cb(r,g,b)	(128 - 0.168736*r - 0.331264*g + 0.5*b)
#define _Cr(r,g,b)	(128 +      0.5*r - 0.418688*g - 0.081312*b)

#define _R(y,cb,cr)	(y                        + 1.402   * (cr - 128))
#define _G(y,cb,cr)	(y - 0.34414 * (cb - 128) - 0.71414 * (cr - 128))
#define _B(y,cb,cr)	(y + 1.772   * (cb - 128))

enum Refresh_Screens_Param{
	refresh_1,refresh_2,refresh_3,refresh_4,refresh_5,refresh_6,refresh_7,refresh_8,refresh_9,refresh_10,
	refresh_11,refresh_12,refresh_13,refresh_14,refresh_15,refresh_16,refresh_17,refresh_18,refresh_19,refresh_20,
	refresh_21,refresh_22,refresh_23,refresh_24,refresh_25,refresh_26,refresh_27,refresh_28,refresh_29,refresh_30,
	refresh_31,refresh_32,refresh_33,refresh_34,refresh_35,refresh_36,refresh_37,refresh_38,refresh_39,refresh_40
};

enum LCD_Rotate{
	Rotate_0,
	Rotate_90,
	Rotate_180
};

void LCD_AllRefreshScreenClear(void);
void LCD_RefreshScreenClear(int nrRefresh);
int LCD_IsRefreshScreenTimeout(int nrRefresh, int timeout);

uint32_t GetTransitionColor(uint32_t colorFrom, uint32_t colorTo, float transCoeff);
float GetTransitionCoeff(uint32_t colorFrom, uint32_t colorTo, uint32_t colorTrans);

#endif /* LCD_LCD_COMMON_H_ */
