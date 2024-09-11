/*
 * Keyboard.h
 *
 *  Created on: Sep 8, 2024
 *      Author: maraf
 */

#ifndef LCD_KEYBOARD_H_
#define LCD_KEYBOARD_H_

#include "stm32f7xx_hal.h"
#include "LCD_fonts_images.h"
#include "LCD_Common.h"
#include "common.h"
#include "touch.h" //to usunac !!!

	#define MIDDLE_NR		1
	#define GET_X(txt)	LCD_Xmiddle(MIDDLE_NR,GetPos,fontID,txt,0,NoConstWidth)
	#define GET_Y			LCD_Ymiddle(MIDDLE_NR,GetPos,fontID)

typedef enum{
	KeysAutoSize,		/* Keys auto size to fontPress size*/
	KeysNotDel,
	KeysDel
}KEYBOARD_ANOTHER_PARAM;

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

extern int fontID;
extern int fontID_descr;
extern int frameColor;
extern int fillColor;
extern int framePressColor;
extern int fillPressColor;
extern int bkColor;

extern int frameColor_c[], fillColor_c[], bkColor_c[];
extern uint16_t widthAll, widthAll_c;
extern uint16_t heightAll, heightAll_c;

void Wpisz__(int a1,int a2,int a3,int a4,int a5,int a6,int a7);
void _deleteAllTouchs(void);
void _deleteAllTouchParams(void);
void _deleteTouchs(int nr);
void _deleteTouchParams(int nr);
int _startUp(int type, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther);
void _Str(const char *txt, uint32_t color);
void _StrDescr_XYoffs(XY_Touch_Struct pos,int offsX,int offsY, const char *txt, uint32_t color);  //XY_Touch_Struct dac w common.h
void _StrDescrWin_XYoffs(int nr,int offsX,int offsY, const char *txt, uint32_t color);
void _Str_Xmidd_Yoffs(int nr,XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color);
void _StrWin_Xmidd_Yoffs(int nr,int offsY, const char *txt, uint32_t color);
void _StrLeft(int nr, const char *txt, XY_Touch_Struct pos, uint32_t color);
void _StrDescr_Xmidd_Yoffs(XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color);
void _StrDescr(int nr,XY_Touch_Struct pos, const char *txt, uint32_t color);
void _StrPress(const char *txt, uint32_t color);
void _StrPressLeft(int nr,const char *txt, XY_Touch_Struct pos, uint32_t color);
void _StrDisp(int nr,const char *txt, uint32_t color);
void _StrPressDisp(int nr,const char *txt, uint32_t color);
void _TxtPos(int nr,XY_Touch_Struct pos);
void _Key(int nr,XY_Touch_Struct pos);
void _KeyPressWin(int nr);
void _KeyStr(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color);
void _KeyStrleft(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color);
void _KeyStrDisp(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color);
void _KeyPress(int nr, XY_Touch_Struct pos);
void _KeyStrPress(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt);
void _KeyStrPressLeft(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt);
void _KeyStrPressDisp(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt);
void _KeyStrPressDisp_oneBlock(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt);
void _KeyShapePressDisp_oneBlock(int nr, XY_Touch_Struct pos, ShapeFunc pShape, SHAPE_PARAMS param);
;void _SetTouchSlider(int nr,uint16_t idx, SHAPE_PARAMS posElemSlider);

void _SetTouchSlider(int nr,uint16_t idx, SHAPE_PARAMS posElemSlider);
void _ElemSliderPressDisp_oneBlock(int nr,uint16_t x, XY_Touch_Struct posSlider, uint32_t spaceTringLine, int selElem, uint32_t lineColor, uint32_t lineSelColor, int *pVal, int valType, int maxSlidVal, VOID_FUNCTION *pfunc);
void _SetTouch(int nr, uint16_t ID, uint16_t idx, uint8_t paramTouch, XY_Touch_Struct pos);

void KEYBOARD_SetPosKey(int nr, XY_Touch_Struct pos[], const uint16_t dim[], int head);
int KEYBOARD_GetHead(int nr);
void KEYBOARD_SetDimKey(int nr, figureShape shape, uint16_t width, uint16_t height, const char *txt);
void KEYBOARD_SetDimAll(int nr, const uint16_t dim[], int head);
void KEYBOARD_KeysAllRelease(int nr, XY_Touch_Struct posKeys[], const char *txtKeys[], const COLORS_DEFINITION colorTxtKeys[], const uint16_t dim[], char* headTxt, uint32_t colorDescr);
void KEYBOARD_SetTouch(int nr, uint16_t startTouchIdx, const uint16_t dim[], XY_Touch_Struct posKeys[]);


void _ServiceRGB__(int k, int selBlockPress, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther,\
		int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr, const char *txtKey[],const COLORS_DEFINITION colorTxtKey[], const COLORS_DEFINITION colorTxtPressKey[], const uint16_t dimKeys[]);
#endif /* LCD_KEYBOARD_H_ */
