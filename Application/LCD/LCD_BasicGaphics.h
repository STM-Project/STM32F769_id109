/*
 * LCD_BasicGaphics.h
 *
 *  Created on: 11.04.2021
 *      Author: Elektronika RM
 */

#ifndef LCD_LCD_BASICGAPHICS_H_
#define LCD_LCD_BASICGAPHICS_H_

#include "stm32f7xx_hal.h"
#include "common.h"

#define RATIO_AA_VALUE_MIN 	1.0
#define RATIO_AA_VALUE_MAX 	0.0

#define LCD_X 	LCD_GetXSize()
#define LCD_Y 	LCD_GetYSize()

#define NMB_SLIDER_ELEMENTS	3

enum CircleParam{
	Half_Circle_0,
	Half_Circle_90,
	Half_Circle_180,
	Half_Circle_270,
	Percent_Circle,
	Degree_Circle
};

enum COPY_TO_SHAPE_STRUCT{
	ToStructAndReturn = 0x80000001,
	ToStructAndDisplay = 0x80000000
};

typedef enum{
	NoDirect = -1,
	Up,
	Down,
	Right,
	Left,
	outside,
	inside,
	LeftLeft,
	RightRight,
	Horizontal,
	Vertical,
}DIRECTIONS;

#define NORMAL_SLIDER_PARAM	2,8,2,2
#define NORMAL_SLIDER_PARAM2	2,6,2,1
typedef enum{
	DelTriang = -1,
	NoSel,
	LeftSel,
	PtrSel,
	RightSel,
	Percent,
	PosXY,
}SLIDER_PARAMS;

extern uint32_t LCD_GetXSize(void);
extern uint32_t LCD_GetYSize(void);

void CorrectLineAA_on(void);
void CorrectLineAA_off(void);
void Set_AACoeff_RoundFrameRectangle(float outCoeff, float inCoeff);
void _FillBuff(int itCount, uint32_t color);
void _CopyDrawPos(void);
void _SetCopyDrawPos(void);
void _IncDrawPos(int pos);
void _StartDrawLine(uint32_t posBuff,uint32_t BkpSizeX,uint32_t x,uint32_t y);
uint32_t _GetPosK(uint32_t posBuff,uint32_t BkpSizeX,uint32_t x,uint32_t y);
uint32_t _GetPosY(uint32_t posBuff,uint32_t BkpSizeX);
uint32_t _GetPosX(uint32_t posBuff,uint32_t BkpSizeX);
structPosition _GetPosXY(uint32_t posBuff,uint32_t BkpSizeX);
void _NextDrawLine(uint32_t BkpSizeX,uint32_t width);
void _DrawRight(int width, uint32_t color);
void _DrawLeft(int width, uint32_t color);
void _DrawDown(int height, uint32_t color,uint32_t BkpSizeX);
void _DrawUp(int height, uint32_t color,uint32_t BkpSizeX);
void Set_AACoeff_Draw(int pixelsInOneSide, uint32_t colorFrom, uint32_t colorTo, float ratioStart);

void _DrawRightDown(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawRightDown_AA(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawArrayRightDown(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...);
void _DrawArrayRightDown_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...);
//void _DrawArrayBuffRightDown_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf);

void _DrawRightUp(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawRightUp_AA(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawArrayRightUp(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...);
void _DrawArrayRightUp_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...);
//void _DrawArrayBuffRightUp_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf);

void _DrawLeftDown(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawLeftDown_AA(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawArrayLeftDown(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...);
void _DrawArrayLeftDown_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...);
//void _DrawArrayBuffLeftDown_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf);

void _DrawLeftUp(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawLeftUp_AA(int width,int height, uint32_t color,uint32_t BkpSizeX);
void _DrawArrayLeftUp(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...);
void _DrawArrayLeftUp_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...);
//void _DrawArrayBuffLeftUp_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf);

void LCD_LineH(uint32_t BkpSizeX, uint16_t x, uint16_t y, uint16_t width,  uint32_t color, uint16_t bold);
void LCD_LineV(uint32_t BkpSizeX, uint16_t x, uint16_t y, uint16_t width,  uint32_t color, uint16_t bold);

void LCD_Display(uint32_t posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height);
void LCD_DisplayPart(uint32_t posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height);
void LCD_Show(void);

void LCD_Shape(uint32_t x,uint32_t y,figureShape pShape,uint32_t width,uint32_t height,uint32_t FrameColor,uint32_t FillColor,uint32_t BkpColor);
void LCD_ShapeWindow(figureShape pShape,uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_ShapeIndirect(uint32_t xPos,uint32_t yPos,figureShape pShape, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_ShapeWindowIndirect(uint32_t xPos,uint32_t yPos,figureShape pShape,uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);

void LCD_Clear(uint32_t color);
void LCD_ClearPartScreen(uint32_t posBuff, uint32_t BkpSizeX, uint32_t BkpSizeY, uint32_t color);
uint32_t SetBold2Color(uint32_t frameColor, uint8_t thickness);

void LCD_Rectangle		(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_Frame				(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_BoldRectangle	(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_BoldFrame		(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);

void LCD_RoundFrame			(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_RoundRectangle		(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_BoldRoundRectangle(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_BoldRoundFrame		(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);

void LCD_LittleRoundRectangle(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_LittleRoundFrame(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);

structPosition DrawLine(uint32_t posBuff,uint16_t x, uint16_t y, uint16_t len, uint16_t degree, uint32_t lineColor,uint32_t BkpSizeX, float ratioAA1, float ratioAA2 ,uint32_t bk1Color, uint32_t bk2Color);



/* ------- Selected Figures ------------------*/
void LCD_SignStar(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_SimpleTriangle(uint32_t posBuff,uint32_t BkpSizeX, uint32_t x,uint32_t y, uint32_t halfWidth,uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor, DIRECTIONS direct);
structPosition LCD_ShapeExample(uint32_t posBuff,uint32_t BkpSizeX, uint32_t x,uint32_t y, uint32_t lineLen, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor, int angleInclination);
SHAPE_PARAMS LCD_Arrow(uint32_t posBuff,uint32_t bkpSizeX,uint32_t bkpSizeY, uint32_t x,uint32_t y, uint32_t width,uint32_t height, uint32_t frameColor, uint32_t fillColor, uint32_t bkpColor, DIRECTIONS direct);
SHAPE_PARAMS LCD_Enter(uint32_t posBuff,uint32_t bkpSizeX,uint32_t bkpSizeY, uint32_t x,uint32_t y, uint32_t width,uint32_t height, uint32_t frameColor, uint32_t fillColor, uint32_t bkpColor);
SHAPE_PARAMS LCD_Exit(uint32_t posBuff,uint32_t bkpSizeX,uint32_t bkpSizeY, uint32_t x,uint32_t y, uint32_t width,uint32_t height, uint32_t frameColor, uint32_t fillColor, uint32_t bkpColor);
SHAPE_PARAMS LCD_KeyBackspace(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);

SHAPE_PARAMS LCD_SimpleSliderH(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t widthParam, uint32_t heightParam, uint32_t ElementsColor, uint32_t LineColor, uint32_t LineSelColor, uint32_t BkpColor, uint32_t slidPos, int elemSel);
SHAPE_PARAMS LCD_SimpleSliderV(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t widthParam, uint32_t heightParam, uint32_t ElementsColor, uint32_t LineColor, uint32_t LineSelColor, uint32_t BkpColor, uint32_t slidPos, int elemSel);
int ChangeElemSliderColor(SLIDER_PARAMS sel, uint32_t color);
uint32_t ChangeElemSliderSize(uint16_t width, uint8_t coeffHeightTriang, uint8_t coeffLineBold, uint8_t coeffHeightPtr, uint8_t coeffWidthPtr);
uint32_t SetSpaceTriangLineSlider(uint16_t height, uint16_t param);
uint32_t SetValType(uint16_t slidPos, uint16_t param);
uint32_t SetLineBold2Width(uint32_t width, uint8_t bold);
uint32_t SetTriangHeightCoeff2Height(uint32_t height, uint8_t coeff);

SHAPE_PARAMS LCDSHAPE_Window(ShapeFunc pShape, uint32_t posBuff, SHAPE_PARAMS param);
SHAPE_PARAMS LCDSHAPE_Arrow(uint32_t posBuff, SHAPE_PARAMS param);
SHAPE_PARAMS LCDSHAPE_Enter(uint32_t posBuff, SHAPE_PARAMS param);
SHAPE_PARAMS LCDSHAPE_Exit(uint32_t posBuff, SHAPE_PARAMS param);
SHAPE_PARAMS LCDSHAPE_KeyBackspace(uint32_t posBuff, SHAPE_PARAMS param);

/* ------- End Selected Figures ------------------*/

void LCD_SetCircleParam(float outRatio, float inRatio, int len, ...);

void LCD_SetCircleDegrees(int len, ...);
void LCD_SetCircleDegreesBuff(int len, uint16_t *buf);
void LCD_SetCircleDegColors(int len, ...);
void LCD_SetCircleDegColorsBuff(int len, uint32_t *buf);
void LCD_SetCirclePercentParam(int degLen, uint16_t *degBuff, uint32_t *degColorBuff);
void LCD_SetCircleDegree(uint8_t degNr, uint8_t deg);
uint16_t LCD_GetCircleDegree(uint8_t degNr);
uint32_t SetParamWidthCircle(uint16_t param, uint32_t width);
uint16_t CenterOfCircle(uint16_t xy, uint16_t width);

void LCD_SetCircleLine(uint8_t lineNr, uint8_t val);
void LCD_OffsCircleLine(uint8_t lineNr, int offs);
void LCD_SetCircleAA(float outRatio, float inRetio);
void LCD_CopyCircleAA(void);
void LCD_SetCopyCircleAA(void);
uint16_t LCD_GetCircleWidth(void);
void LCD_CopyCircleWidth(void);
void LCD_SetCopyCircleWidth(void);
uint16_t LCD_CalculateCircleWidth(uint32_t width);
void LCD_Circle(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x, uint32_t y, uint32_t _width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
void LCD_HalfCircle(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);





void LCD_Circle_TEST__(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x, uint32_t y, uint32_t _width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor);
SHAPE_PARAMS LCDSHAPE_CircleTEST(uint32_t posBuff, SHAPE_PARAMS param);


#endif /* LCD_LCD_BASICGAPHICS_H_ */
