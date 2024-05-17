/*
 * SCREEN_CalibrationLCD.c
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */
#include "SCREEN_CalibrationLCD.h"
#include "LCD_BasicGaphics.h"
#include "LCD_fonts_images.h"
#include <stdio.h>
#include "touch.h"
#include "stmpe811.h"
#include "TouchLcdTask.h"
#include "LCD_Common.h"
#include "common.h"
#include "debug.h"
#include "lang.h"

/*----------------- Main Settings ------------------*/

#define FILE_NAME(extend) SCREEN_Calibration_##extend

static const char FILE_NAME(Lang)[]="\
Kalibracja LCD,Calibration LCD,\
Ko\xB3o,Circle,\
";\

#define SCREEN_CALIBRATION_SET_PARAMETERS \
/*  id 	 name					default value */ \
	X(0, FONT_SIZE_Title, 	 	FONT_16) \
	X(1, FONT_SIZE_CircleName, FONT_12_bold) \
	X(2, FONT_SIZE_PosLog,		FONT_12_bold) \
	X(3, FONT_SIZE_PosPhys,		FONT_12_bold) \
	\
	X(4, FONT_STYLE_Title, 	 		Arial) \
	X(5, FONT_STYLE_CircleName, 	Arial) \
	X(6, FONT_STYLE_PosLog, 		Comic_Saens_MS) \
	X(7, FONT_STYLE_PosPhys, 		Comic_Saens_MS) \
	\
	X(8, 	FONT_COLOR_Title,  	 	YELLOW) \
	X(9, 	FONT_COLOR_CircleName, 	WHITE) \
	X(10, FONT_COLOR_PosLog, 	 	ORANGE) \
	X(11, FONT_COLOR_PosPhys,  	DARKYELLOW) \
	\
	X(12, COLOR_BkScreen, 			BLACK) \
	X(13, COLOR_CircleFrame, 		WHITE) \
	X(14, COLOR_CircleFill, 		ORANGE) \
	X(15, COLOR_CircleFillPress, 	DARKCYAN) \
	\
	X(16, COEFF_COLOR_CircleName, 255)	\
	X(17, COEFF_COLOR_PosLog,  	255)	\
	X(18, COEFF_COLOR_PosPhys,  	255) \
	\
	X(19, DEBUG_ON,  	1) \
	\
	X(20, FONT_ID_Title,		 	fontID_1) \
	X(21, FONT_ID_CircleName, 	fontID_2) \
	X(22, FONT_ID_PosLog, 	 	fontID_3) \
	X(23, FONT_ID_PosPhys, 	 	fontID_4)

/*------------ End Main Settings -----------------*/

#define CIRCLE_MACRO \
	/*	 Name  	 width   x	  y  */ \
		X("Circle 1", 57,  50,  50) \
		X("Circle 2", 58, 650, 330) \
		X("Circle 3", 59, 300, 140) \
		X("Circle 4", 60,   0, 300) \
		X("Circle 5", 61, 150, 250) \
		X("Circle 6", 62, 603,   1) \
		X("Circle 7", 63, 650, 199) \
		X("Circle 8", 64,   90,   0) \
		X("Circle 8", 65,   90,   0) \
		X("Circle 8", 66,   90,   0) \
		X("Circle 8", 67,   90,   0) \
		X("Circle 8", 68,   90,   0) \
		X("Circle 8", 69,   90,   0) \
		X("Circle 8", 70,   90,   0) \
		X("Circle 8", 71,   90,   0) \
		X("Circle 8", 72,   90,   0) \
		X("Circle 8", 73,   90,   0) \
		X("Circle 8", 74,   90,   0) \
		X("Circle 8", 75,   90,   0) \
		X("Circle 8", 76,   90,   0) \
		X("Circle 8", 77,   90,   0) \
		X("Circle 8", 78,   90,   0) \
		X("Circle 8", 79,   90,   0) \
		X("Circle 8", 80,   90,   0) \
		X("Circle 8", 81,   90,   0) \
		X("Circle 8", 82,   90,   0) \
		X("Circle 8", 83,   90,   0) \
		X("Circle 8", 84,   90,   0) \
		X("Circle 8", 85,   90,   0) \
		X("Circle 8", 86,   90,   0) \
		X("Circle 8", 87,   90,   0) \
		X("Circle 8", 88,   90,   0) \
		X("Circle 8", 89,   90,   0) \
		X("Circle 8", 90,   90,   0) \
		X("Circle 8", 91,   90,   0) \
		X("Circle 8", 92,   90,   0) \
		X("Circle 8", 93,   90,   0) \
		X("Circle 8", 94,   90,   0) \
		X("Circle 8", 95,   90,   0) \
		X("Circle 8", 96,   90,   0) \
		X("Circle 8", 97,   90,   0) \
		X("Circle 8", 98,   90,   0) \
		X("Circle 8", 99,   90,   0) \
		X("Circle 8", 100,   90,   0) \
		X("Circle 8", 101,   90,   0) \
		X("Circle 8", 102,   90,   0) \
		X("Circle 8", 103,   90,   0) \
		X("Circle 8", 104,   90,   0) \


#define DEBUG_Text_1  "error_touch"
#define DEBUG_Text_2  "error_calculation_coeff"

typedef enum{
	#define X(a,b,c) b,
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X
}FILE_NAME(enum);

typedef struct{
	#define X(a,b,c) int b;
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X
}FILE_NAME(struct);

static FILE_NAME(struct) var = {0};
static uint64_t SelectBits = 0;

static void FILE_NAME(funcSet__)(int offs, int val){
	if(CHECK_bit(SelectBits,offs))
		return;
	else
		*( (int*)((int*)(&var) + offs) ) = val;
}

static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys, uint16_t width, char *name)
{
	#define DISPLAY_COMMA_UNDER_COMMA_
	/*Select only one CIRCLE...*/
	//#define CIRCLE_WITH_FRAME_1
	//#define CIRCLE_WITH_FRAME_2
	#define CIRCLE_WITHOUT_FRAME

	StructTxtPxlLen lenStr={0};
	int16_t xPos=0;
	char *ptr=NULL;

	uint16_t _ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
	{
#if defined(CIRCLE_WITH_FRAME_1)
		int widthCalculated=LCD_CalculateCircleWidth(width);
		LCD_ShapeWindow	     		(LCD_Rectangle,0,widthCalculated,widthCalculated, 0,        0,     	   widthCalculated,  widthCalculated,  SetColorBoldFrame(bkColor,0), 		bkColor,  	bkColor);
		LCD_ShapeWindow	     		(LCD_Circle,	0,widthCalculated,widthCalculated, 0,        0,     	   width,  				width,   			SetColorBoldFrame(frameColor,bold), fillColor,  bkColor);
		LCD_ShapeWindow		  		(LCD_Circle,	0,widthCalculated,widthCalculated, width/4+1,width/4+1, 	width/2,				width/2, 			SetColorBoldFrame(frameColor,bold), TRANSPARENT,fillColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Frame,		0,widthCalculated,widthCalculated, 0,			0, 			widthCalculated,	widthCalculated, 							frameColor, 		bkColor, 	bkColor);
#elif defined(CIRCLE_WITH_FRAME_2)
		LCD_ShapeWindow	         (LCD_Rectangle,0,width,width, 0,        	0,     	   width,  width,   	SetColorBoldFrame(bkColor,0), 		bkColor,  	 bkColor);
		LCD_ShapeWindow	         (LCD_Circle,	0,width,width, 0,        	0,     	   width,  width,   	SetColorBoldFrame(frameColor,bold), fillColor,   bkColor);
		LCD_ShapeWindow				(LCD_Circle,	0,width,width, width/4+1,	width/4+1, 	width/2,width/2, 	SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Frame,		0,width,width, 0,				0, 			width,	width, 	frameColor, 								bkColor, 	 bkColor);
#else
		LCD_ShapeWindow	         (LCD_Rectangle,0,width,width, 0,        0,     	   width,  width,   SetColorBoldFrame(bkColor,0), 		  bkColor,  	bkColor);
		LCD_ShapeWindow	         (LCD_Circle,	0,width,width, 0,        0,     	   width,  width,   SetColorBoldFrame(frameColor,bold), fillColor,   bkColor);

		int width_delta 	= width - LCD_CalculateCircleWidth(width/2);   //i przetestowac dla poszczegolnych definów !!!!
		//int pos_Circle 	= (width_delta%2) ? width_delta/2+2 : width_delta/2+3;
		//int pos_Circle 	= (width_delta%2) ? width_delta/2+2 : width_delta/2+3;
		int pos_Circle 	= width_delta/2;

		LCD_ShapeWindowIndirect(x,y,LCD_Circle,	0,width,width, pos_Circle,pos_Circle, width/2,width/2, SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
#endif
		return width;
	}

#if !defined(CIRCLE_WITH_FRAME_1)
	width = CorrectCirclesWidth(width);
#endif	

	LCD_Xmiddle(SetPos,SetPosAndWidth(log.x,width),NULL,0,NoConstWidth);
	ptr = StrAll(3,GetSelTxt(0,FILE_NAME(Lang),1)," ",name);
	width = _ShowCircleIndirect(log.x, log.y, width, 0, var.COLOR_CircleFrame, var.COLOR_CircleFill, var.COLOR_BkScreen);
	lenStr=LCD_StrChangeColorIndirect(var.FONT_ID_CircleName, LCD_Xmiddle(GetPos,var.FONT_ID_CircleName,ptr,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,log.y+width+2), ptr, fullHight, 0, var.COLOR_BkScreen,var.FONT_COLOR_CircleName,var.COEFF_COLOR_CircleName,NoConstWidth);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	xPos = LCD_Xmiddle(GetPos,var.FONT_ID_PosLog,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none));
	lenStr=LCD_StrChangeColorIndirect(var.FONT_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.COLOR_BkScreen,var.FONT_COLOR_PosLog,var.COEFF_COLOR_PosLog,NoConstWidth);
	xPos = xPos + lenStr.inPixel;
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(3,",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	lenStr=LCD_StrChangeColorIndirect(var.FONT_ID_PosLog, xPos, LCD_Ypos(lenStr,GetPos,0), ptr, fullHight, 0, var.COLOR_BkScreen,var.FONT_COLOR_PosLog,var.COEFF_COLOR_PosLog,NoConstWidth);
#else
	lenStr=LCD_StrChangeColorIndirect(var.FONT_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.COLOR_BkScreen,var.FONT_COLOR_PosLog,var.MAXVAL_LOG,NoConstWidth);
#endif

	WaitForTouchState(press);

	BSP_TS_GetState(&TS_State);
	phys->x = TS_State.x;
	phys->y = TS_State.y;

	WaitForTouchState(release);

	width = _ShowCircleIndirect(log.x, log.y, width, 0, var.COLOR_CircleFrame, var.COLOR_CircleFillPress, var.COLOR_BkScreen);

#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(phys->x,None,3,Sign_none));
	xPos = xPos - LCD_GetWholeStrPxlWidth(var.FONT_ID_PosPhys,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(5,"(",Int2Str(phys->x,None,3,Sign_none),",",Int2Str(phys->y,None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(var.FONT_ID_PosPhys, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.COLOR_BkScreen,var.FONT_COLOR_PosPhys,var.COEFF_COLOR_PosPhys,NoConstWidth);
#else
	ptr = StrAll(5,"(",Int2Str(phys->x,None,3,Sign_none),",",Int2Str(phys->y,None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(var.FONT_ID_PosPhys, LCD_Xmiddle(GetPos,var.FONT_ID_PosPhys,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.COLOR_BkScreen,var.FONT_COLOR_PosPhys,var.COEFF_COLOR_PosPhys,NoConstWidth);
#endif
}

int FILE_NAME(funcGet)(int offs){
	return *( (int*)((int*)(&var) + offs) );
}

void FILE_NAME(funcSet)(int offs, int val){
	*( (int*)((int*)(&var) + offs) ) = val;
	SET_bit(SelectBits,offs);
}

void FILE_NAME(debug)(void)
{
	if(var.DEBUG_ON){
		Dbg(1,Clr_ CoG2_"\r\ntypedef struct{\r\n"_X);
		#define X(a,b,c) DbgVar2(1,200,CoGr_"%*d"_X	"%*s" 	CoGr_"= "_X	 	"%*s" 	"(%s0x%x)\r\n",-4,a,		-23,getName(b),	-15,getName(c), 	CHECK_bit(SelectBits,a)?CoR_"change to: "_X:"", var.b);
			SCREEN_CALIBRATION_SET_PARAMETERS
		#undef X
		DbgVar(1,200,CoG2_"}%s;\r\n"_X,getName(FILE_NAME(struct)));
	}
}

void FILE_NAME(main)(void)
{
	#define CIRCLES_NUMBER  STRUCT_TAB_SIZE(pos)

	StructTxtPxlLen lenStr={0};
	char *ptr=NULL;
	int i=0;

	char *circlesNames[]={
		#define X(a,b,c,d) a,
			CIRCLE_MACRO
		#undef X
	};

	XY_Touch_Struct pos[] = {
		#define X(a,b,c,d) {c,d},
			CIRCLE_MACRO
		#undef X
	};

	XY_Touch_Struct phys[CIRCLES_NUMBER] = {0};

	uint16_t width[] = {
		#define X(a,b,c,d) b,
			CIRCLE_MACRO
		#undef X
	};

	#define X(a,b,c) FILE_NAME(funcSet__)(b,c);
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X

	Delete_TouchLcd_Task();

	LCD_AllRefreshScreenClear();  //dac ogpolnie jako SCREEN_ResetAllParameters()
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();

	i=LCD_LoadFont_ChangeColor(var.FONT_SIZE_Title, 	 	var.FONT_STYLE_Title, 		var.FONT_ID_Title);			if(0<=i) var.FONT_ID_Title = i;
	i=LCD_LoadFont_ChangeColor(var.FONT_SIZE_CircleName, 	var.FONT_STYLE_CircleName, var.FONT_ID_CircleName);	if(0<=i) var.FONT_ID_CircleName = i;
	i=LCD_LoadFont_ChangeColor(var.FONT_SIZE_PosLog,  	 	var.FONT_STYLE_PosLog,  	var.FONT_ID_PosLog);			if(0<=i) var.FONT_ID_PosLog = i;
	i=LCD_LoadFont_ChangeColor(var.FONT_SIZE_PosPhys, 	 	var.FONT_STYLE_PosPhys, 	var.FONT_ID_PosPhys);		if(0<=i) var.FONT_ID_PosPhys = i;

	FILE_NAME(debug)();
	DisplayFontsStructState();

	LCD_SetCircleAA(RATIO_AA_VALUE_MAX, RATIO_AA_VALUE_MAX);
	LCD_Clear(var.COLOR_BkScreen);  LCD_Show();

	ptr = GetSelTxt(0,FILE_NAME(Lang),0);
	LCD_Xmiddle(SetPos,SetPosAndWidth(0,LCD_X),NULL,0,NoConstWidth);
	lenStr=LCD_StrChangeColorIndirect(var.FONT_ID_Title, LCD_Xmiddle(GetPos,var.FONT_ID_Title,ptr,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,0), ptr, fullHight,0,var.COLOR_BkScreen,var.FONT_COLOR_Title,var.COEFF_COLOR_CircleName,NoConstWidth);

	uint8_t status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());

	if(status)
		Dbg(var.DEBUG_ON, TEXT2PRINT(DEBUG_Text_1,1));
	else
	{
	   SetLogXY(pos,width,CIRCLES_NUMBER);

	   for (i = 0; i < CIRCLES_NUMBER; i++)
	      GetPhysValues(pos[i], &phys[i], width[i], circlesNames[i]);

	   SetPhysXY(phys,CIRCLES_NUMBER);

	   if(CalcutaleCoeffCalibration(CIRCLES_NUMBER))
	   	Dbg(var.DEBUG_ON, TEXT2PRINT(DEBUG_Text_2,1));
	   else{
		   CalibrationWasDone();
		   DisplayCoeffCalibration();
	   }

	   Create_TouchLcd_Task();
	}
}
