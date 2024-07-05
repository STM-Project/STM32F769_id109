/*
 * LCD_fonts_images.h
 *
 *  Created on: 20.04.2021
 *      Author: Elektronika RM
 */

#ifndef LCD_LCD_FONTS_IMAGES_H_
#define LCD_LCD_FONTS_IMAGES_H_

#include "stm32f7xx_hal.h"

#define MAX_SIZE_TOUCHIDX_FOR_STRVAR		5

#define Ą "\xA5"
#define ą "\xB9"
#define Ć "\xC6"
#define ć "\xE6"
#define Ę "\xCA"
#define ę "\xEA"
#define Ł "\xA3"
#define ł "\xB3"
#define Ń "\xD1"
#define ń "\xF1"
#define Ó "\xD3"
#define ó "\xF3"
#define Ś "\x8C"
#define ś "\x9C"
#define Ź "\x8F"
#define ź "\x9F"
#define Ż "\xAF"
#define ż "\xBF"

#define DESCR_PARAM_11	-1,0,0,0,0,0,0,0,0,0
#define DESCR_PARAM_10	DESCR_PARAM_11,DESCR_PARAM_11
#define DESCR_PARAM_9	DESCR_PARAM_10,DESCR_PARAM_11
#define DESCR_PARAM_8	DESCR_PARAM_9,DESCR_PARAM_11
#define DESCR_PARAM_7	DESCR_PARAM_8,DESCR_PARAM_11
#define DESCR_PARAM_6	DESCR_PARAM_7,DESCR_PARAM_11
#define DESCR_PARAM_5	DESCR_PARAM_6,DESCR_PARAM_11
#define DESCR_PARAM_4	DESCR_PARAM_5,DESCR_PARAM_11
#define DESCR_PARAM_3	DESCR_PARAM_4,DESCR_PARAM_11
#define DESCR_PARAM_2	DESCR_PARAM_3,DESCR_PARAM_11
#define DESCR_PARAM_1	DESCR_PARAM_2,DESCR_PARAM_11
#define LCD_STR_DESCR_PARAM_NUMBER(x)	DESCR_PARAM_##x

enum FontsSize
{
	FONT_8,
	FONT_8_bold,
	FONT_8_italics,
	FONT_9,
	FONT_9_bold,
	FONT_9_italics,
	FONT_10,
	FONT_10_bold,
	FONT_10_italics,
	FONT_11,
	FONT_11_bold,
	FONT_11_italics,
	FONT_12,
	FONT_12_bold,
	FONT_12_italics,
	FONT_14,
	FONT_14_bold,
	FONT_14_italics,
	FONT_16,
	FONT_16_bold,
	FONT_16_italics,
	FONT_18,
	FONT_18_bold,
	FONT_18_italics,
	FONT_20,
	FONT_20_bold,
	FONT_20_italics,
	FONT_22,
	FONT_22_bold,
	FONT_22_italics,
	FONT_24,
	FONT_24_bold,
	FONT_24_italics,
	FONT_26,
	FONT_26_bold,
	FONT_26_italics,
	FONT_28,
	FONT_28_bold,
	FONT_28_italics,
	FONT_36,
	FONT_36_bold,
	FONT_36_italics,
	FONT_48,
	FONT_48_bold,
	FONT_48_italics,
	FONT_72,
	FONT_72_bold,
	FONT_72_italics,
	FONT_130,
	FONT_130_bold,
	FONT_130_italics
};

enum FontsStyle
{
	Arial,
	Times_New_Roman,
	Comic_Saens_MS,
	Arial_Narrow,
	Calibri_Light,
	Bodoni_MT_Condenset
};

enum ID_Fonts
{
	fullHight=-3,
	halfHight=-2,
	fontNoChange=-1,
	fontID_1,
	fontID_2,
	fontID_3,
	fontID_4,
	fontID_5,
	fontID_6,
	fontID_7,
	fontID_8,
	fontID_9,
	fontID_10,
	fontID_11,
	fontID_12,
	fontID_13,
	fontID_14,
	fontID_15,
	fontID_16,
	fontID_17
};

enum Var_Fonts
{
	fontVar_1,fontVar_2,fontVar_3,fontVar_4,fontVar_5,fontVar_6,fontVar_7,fontVar_8,fontVar_9,fontVar_10,
	fontVar_11,fontVar_12,fontVar_13,fontVar_14,fontVar_15,fontVar_16,fontVar_17,fontVar_18,fontVar_19,fontVar_20,
	fontVar_21,fontVar_22,fontVar_23,fontVar_24,fontVar_25,fontVar_26,fontVar_27,fontVar_28,fontVar_29,fontVar_30,
	fontVar_31,fontVar_32,fontVar_33,fontVar_34,fontVar_35,fontVar_36,fontVar_37,fontVar_38,fontVar_39,fontVar_40
};

enum PositionIncrement
{
	SetPos,
	GetPos,
	IncPos,
};

enum ConstWidthFontOrNot
{
	NoConstWidth,
	ConstWidth,
};

enum RoundRectangleBkFont
{
	BK_Rectangle,
	BK_Round,
	BK_LittleRound,
};

enum DESCR_DIRECTION{
  Under_center,
  Under_left,
  Under_right,

  Above_center,
  Above_left,
  Above_right,

  Left_down,
  Left_mid,
  Left_up,

  Right_down,
  Right_mid,
  Right_up,
};

typedef struct {
	uint16_t inChar;
	uint16_t inPixel;
	uint16_t height;
}StructTxtPxlLen;

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	StructTxtPxlLen len;
}StructFieldPos;

/* for future use */
/* typedef struct {
	int fontID;
	uint32_t fontColor;
	uint32_t bkColor;
	int interspace;
	int directionDescr;
	char *txt;
	int OnlyDigits;
	int space;
	int maxVal;
	int constWidth;
}LCD_STR_PARAM; */

void LCD_SetStrVar_bkColor(int idVar, uint32_t bkColor);
void LCD_SetStrVar_fontColor(int idVar, uint32_t fontColor);
void LCD_SetStrVar_x(int idVar,int x);
void LCD_SetStrVar_y(int idVar,int y);
void LCD_OffsStrVar_x(int idVar,int x);
void LCD_OffsStrVar_y(int idVar,int y);
void LCD_SetStrVar_heightType(int idVar, int OnlyDigits);
void LCD_SetStrVar_widthType(int idVar, int constWidth);
void LCD_SetStrVar_coeff(int idVar, int coeff);
void LCD_SetStrVar_space(int idVar, int space);
void LCD_SetStrVar_fontID(int idVar, int fontID);
void LCD_SetStrVar_bkRoundRect(int idVar, int bkRoundRect);
void LCD_SetStrVar_bkScreenColor(int idVar, int bkScreenColor);
void LCD_SetStrVar_idxTouch(int idVar, int nr, int idxTouch);
void LCD_SetStrVar_Mov_posWin(int idVar, int posWin);

uint32_t LCD_GetStrVar_bkColor(int idVar);
uint32_t LCD_GetStrVar_fontColor(int idVar);
int LCD_GetStrVar_x(int idVar);
int LCD_GetStrVar_y(int idVar);
int LCD_GetStrVar_heightType(int idVar);
int LCD_GetStrVar_widthType(int idVar);
int LCD_GetStrVar_coeff(int idVar);
int LCD_GetStrVar_space(int idVar);
int LCD_GetStrVar_widthPxl(int idVar);
int LCD_GetStrVar_heightPxl(int idVar);
int LCD_GetStrVar_fontID(int idVar);
int LCD_GetStrVar_bkScreenColor(int idVar);
int LCD_GetStrVar_bkRoundRect(int idVar);
int LCD_GetStrVar_idxTouch(int idVar, int nr);
int LCD_GetStrVar_Mov_posWin(int idVar);
void LCD_SetBkFontShape(int idVar, int bkType);
void LCD_DeleteAllFontAndImages(void);
int LCD_DeleteFont(uint32_t fontID);
int LCD_GetFontSizeMaxNmb(void);
int LCD_GetFontStyleMaxNmb(void);
const char *LCD_GetFontStyleStr(int fontStyle);
const char *LCD_GetFontSizeStr(int fontSize);
char *LCD_FontSize2Str(char *buffTemp, int fontSize);
char *LCD_FontStyle2Str(char *buffTemp, int fontStyle);
char *LCD_FontType2Str(char *buffTemp, int id, int idAlt);
int LCD_GetCoeffDependOnFontType(uint32_t fontColor, uint32_t bkColor, int correct);
void DisplayFontsStructState(void);
void InfoForImagesFonts(void);

int LCD_LoadFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, uint32_t fontID);
int LCD_LoadFont_WhiteBlack(int fontSize, int fontStyle, uint32_t fontID);
int LCD_LoadFont_DarkgrayGreen(int fontSize, int fontStyle, uint32_t fontID);
int LCD_LoadFont_DarkgrayWhite(int fontSize, int fontStyle, uint32_t fontID);
int LCD_LoadFont_ChangeColor(int fontSize, int fontStyle, uint32_t fontID);
uint32_t LCD_LoadFont_DependOnColors(int fontSize, int fontStyle, uint32_t bkColor, uint32_t fontColor, uint32_t fontID);

StructTxtPxlLen LCD_Str(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrWindow(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrIndirect(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrWindowIndirect(uint32_t posBuff, int Xwin, int Ywin, uint32_t BkpSizeX, uint32_t BkpSizeY, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrChangeColor(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth);
StructTxtPxlLen LCD_StrChangeColorWindow(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth);
StructTxtPxlLen LCD_StrChangeColorIndirect(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth);
StructTxtPxlLen LCD_StrChangeColorWindowIndirect(uint32_t posBuff, int Xwin, int Ywin,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth);

StructTxtPxlLen LCD_StrVar(int idVar,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor);
StructTxtPxlLen LCD_StrVarIndirect(int idVar, char *txt);

StructTxtPxlLen LCD_StrDescrVar(int idVar,int fontID,  int Xpos, 		 int Ypos, 				char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor, \
														  int fontID2, int interspace, int directionDescr, char *txt2,int OnlyDigits2, int space2, uint32_t bkColor2, uint32_t fontColor2, int maxVal2, int constWidth2);

StructTxtPxlLen LCD_StrChangeColorVar(int idVar,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth, uint32_t bkScreenColor);
StructTxtPxlLen LCD_StrChangeColorVarIndirect(int idVar, char *txt);

StructTxtPxlLen LCD_StrChangeColorDescrVar(int idVar,int fontID, int Xpos, 		int Ypos, 				char *txt, int OnlyDigits, 	int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth, uint32_t bkScreenColor, \
																		int fontID2, int interspace, int directionDescr, char *txt2, int OnlyDigits2, int space2, uint32_t bkColor2, uint32_t fontColor2,uint8_t maxVal2, int constWidth2 );

StructTxtPxlLen LCD_StrDependOnColors(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,int maxVal, int constWidth);
StructTxtPxlLen LCD_StrDependOnColorsIndirect(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,int maxVal, int constWidth);
StructTxtPxlLen LCD_StrDependOnColorsVar(int idVar, int fontID, uint32_t fontColor, uint32_t bkColor, uint32_t bkScreenColor, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,int maxVal, int constWidth);
StructTxtPxlLen LCD_StrDependOnColorsVarIndirect(int idVar, char *txt);

StructTxtPxlLen LCD_StrDependOnColorsDescrVar(int idVar,int fontID, uint32_t fontColor, uint32_t bkColor, uint32_t bkScreenColor, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,int maxVal, int constWidth, \
																			int fontID2, uint32_t fontColor2, uint32_t bkColor2, int interspace, int directionDescr, char *txt2, int OnlyDigits2, int space2,int maxVal2, int constWidth2);

StructTxtPxlLen LCD_StrDependOnColorsWindow(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth);
StructTxtPxlLen LCD_StrDependOnColorsWindowIndirect(uint32_t posBuff, int Xwin, int Ywin,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth);

void LCD_ResetStrMovBuffPos(void);
void LCD_DisplayStrMovBuffState(void);

StructTxtPxlLen LCD_StrRot(int rot, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrChangeColorRot(int rot, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth);
StructTxtPxlLen LCD_StrRotVar(int idVar, int rot, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor);
StructTxtPxlLen LCD_StrChangeColorRotVar(int idVar, int rot, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth, uint32_t bkScreenColor);
StructTxtPxlLen LCD_StrRotVarIndirect(int idVar, char *txt);
StructTxtPxlLen LCD_StrChangeColorRotVarIndirect(int idVar, char *txt);
StructTxtPxlLen LCD_StrRotWin(int rot, int winWidth, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor);
StructTxtPxlLen LCD_StrChangeColorRotWin(int rot, int winWidth, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth, uint32_t bkScreenColor);

StructTxtPxlLen LCD_StrMovH(int idVar, int rot, int posWin, int winWidth, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrChangeColorMovH(int idVar, int rot, int posWin, int winWidth, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth);
int LCD_StrMovHIndirect(int idVar, int incrDecr);
StructTxtPxlLen LCD_StrMovV(int idVar, int rot, int posWin, int winWidth,int winHeight, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth);
StructTxtPxlLen LCD_StrChangeColorMovV(int idVar, int rot, int posWin, int winWidth,int winHeight, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int coeff, int constWidth);
int LCD_StrMovVIndirect(int idVar, int incrDecr);

int LCD_GetWholeStrPxlWidth(int fontID, char *txt, int space, int constWidth);
int LCD_GetStrPxlWidth(int fontID, char *txt, int len, int space, int constWidth);
int LCD_GetStrLenForPxlWidth(int fontID, char *txt, int lenInPxl, int space, int constWidth);
int LCD_GetFontWidth(int fontID, char font);
int LCD_GetFontHeight(int fontID);
int LCD_GetFontHalfHeight(int fontID);

int CopyCharsTab(char *buf, int len, int offset, int fontID);

int LCD_SelectedSpaceBetweenFontsIncrDecr(uint8_t incrDecr, uint8_t fontStyle, uint8_t fontSize, char char1, char char2);
void LCD_DisplayRemeberedSpacesBetweenFonts(void);
void LCD_WriteSpacesBetweenFontsOnSDcard(void);
void LCD_ResetSpacesBetweenFonts(void);
void LCD_SetSpacesBetweenFonts(void);

uint16_t LCD_Ypos(StructTxtPxlLen structTemp, int cmd, int offs);
uint16_t LCD_Xpos(StructTxtPxlLen structTemp, int cmd, int offs);
uint16_t LCD_Ymiddle(int nr, int cmd, uint32_t val);
uint16_t LCD_Xmiddle(int nr, int cmd, uint32_t val, char *txt, int space, int constWidth);
uint32_t SetPosAndWidth(uint16_t pos, uint16_t width);

void SCREEN_ResetAllParameters(void);

/* LCD_STR_PARAM LCD_SetStrDescrParam(int fontID, uint32_t fontColor, uint32_t bkColor, int interspace, int directionDescr, char *txt, int OnlyDigits, int space,int maxVal, int constWidth); */ 	/* for further implementation */
StructFieldPos LCD_StrDependOnColorsDescrVar_array(int idVar,int fontID, uint32_t fontColor, uint32_t bkColor, uint32_t bkScreenColor, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,int maxVal, int constWidth, \
		int fontID1, uint32_t fontColor1, uint32_t bkColor1, int interspace1, int directionDescr1, char *txt1, int OnlyDigits1, int space1,int maxVal1, int constWidth1, \
		int fontID2, uint32_t fontColor2, uint32_t bkColor2, int interspace2, int directionDescr2, char *txt2, int OnlyDigits2, int space2,int maxVal2, int constWidth2, \
		int fontID3, uint32_t fontColor3, uint32_t bkColor3, int interspace3, int directionDescr3, char *txt3, int OnlyDigits3, int space3,int maxVal3, int constWidth3, \
		int fontID4, uint32_t fontColor4, uint32_t bkColor4, int interspace4, int directionDescr4, char *txt4, int OnlyDigits4, int space4,int maxVal4, int constWidth4, \
		int fontID5, uint32_t fontColor5, uint32_t bkColor5, int interspace5, int directionDescr5, char *txt5, int OnlyDigits5, int space5,int maxVal5, int constWidth5, \
		int fontID6, uint32_t fontColor6, uint32_t bkColor6, int interspace6, int directionDescr6, char *txt6, int OnlyDigits6, int space6,int maxVal6, int constWidth6, \
		int fontID7, uint32_t fontColor7, uint32_t bkColor7, int interspace7, int directionDescr7, char *txt7, int OnlyDigits7, int space7,int maxVal7, int constWidth7, \
		int fontID8, uint32_t fontColor8, uint32_t bkColor8, int interspace8, int directionDescr8, char *txt8, int OnlyDigits8, int space8,int maxVal8, int constWidth8, \
		int fontID9, uint32_t fontColor9, uint32_t bkColor9, int interspace9, int directionDescr9, char *txt9, int OnlyDigits9, int space9,int maxVal9, int constWidth9, \
		int fontID10, uint32_t fontColor10, uint32_t bkColor10, int interspace10, int directionDescr10, char *txt10, int OnlyDigits10, int space10,int maxVa110, int constWidth10, \
		int fontID11, uint32_t fontColor11, uint32_t bkColor11, int interspace11, int directionDescr11, char *txt11, int OnlyDigits11, int space11,int maxVal11, int constWidth11, \
		int fontID12, uint32_t fontColor12, uint32_t bkColor12, int interspace12, int directionDescr12, char *txt12, int OnlyDigits12, int space12,int maxVal12, int constWidth12 );

StructFieldPos LCD_StrDependOnColorsDescrVar_array_Nakladka(int idVar,int fontID, uint32_t fontColor, uint32_t bkColor, uint32_t bkScreenColor, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,int maxVal, int constWidth, \
		int fontID1, uint32_t fontColor1, uint32_t bkColor1, int interspace1, int directionDescr1, char *txt1, int OnlyDigits1, int space1,int maxVal1, int constWidth1, \
		int fontID2, uint32_t fontColor2, uint32_t bkColor2, int interspace2, int directionDescr2, char *txt2, int OnlyDigits2, int space2,int maxVal2, int constWidth2, \
		int fontID3, uint32_t fontColor3, uint32_t bkColor3, int interspace3, int directionDescr3, char *txt3, int OnlyDigits3, int space3,int maxVal3, int constWidth3, \
		int fontID4, uint32_t fontColor4, uint32_t bkColor4, int interspace4, int directionDescr4, char *txt4, int OnlyDigits4, int space4,int maxVal4, int constWidth4, \
		int fontID5, uint32_t fontColor5, uint32_t bkColor5, int interspace5, int directionDescr5, char *txt5, int OnlyDigits5, int space5,int maxVal5, int constWidth5, \
		int fontID6, uint32_t fontColor6, uint32_t bkColor6, int interspace6, int directionDescr6, char *txt6, int OnlyDigits6, int space6,int maxVal6, int constWidth6, \
		int fontID7, uint32_t fontColor7, uint32_t bkColor7, int interspace7, int directionDescr7, char *txt7, int OnlyDigits7, int space7,int maxVal7, int constWidth7, \
		int fontID8, uint32_t fontColor8, uint32_t bkColor8, int interspace8, int directionDescr8, char *txt8, int OnlyDigits8, int space8,int maxVal8, int constWidth8, \
		int fontID9, uint32_t fontColor9, uint32_t bkColor9, int interspace9, int directionDescr9, char *txt9, int OnlyDigits9, int space9,int maxVal9, int constWidth9, \
		int fontID10, uint32_t fontColor10, uint32_t bkColor10, int interspace10, int directionDescr10, char *txt10, int OnlyDigits10, int space10,int maxVa110, int constWidth10, \
		int fontID11, uint32_t fontColor11, uint32_t bkColor11, int interspace11, int directionDescr11, char *txt11, int OnlyDigits11, int space11,int maxVal11, int constWidth11, \
		int fontID12, uint32_t fontColor12, uint32_t bkColor12, int interspace12, int directionDescr12, char *txt12, int OnlyDigits12, int space12,int maxVal12, int constWidth12 );


#endif /* LCD_LCD_FONTS_IMAGES_H_ */
