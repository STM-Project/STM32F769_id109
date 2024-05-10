/*
 * SCREEN_CalibrationLCD.c
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */
#include "SCREEN_CalibrationLCD.h"
#include <LCD_BasicGaphics.h>
#include "LCD_fonts_images.h"
#include <stdio.h>
#include "touch.h"
#include "stmpe811.h"
#include "TouchLcdTask.h"
#include "LCD_Common.h"
#include "common.h"
#include "debug.h"
#include "tim.h"
#include "lang.h"

#define FILE_NAME(extend) SCREEN_Calibration_##extend

const char FILE_NAME(Lang)[]="\
Kalibracja LCD,Calibration LCD,\
Ko\xB3o,Circle,\
";\

#define SCREEN_CALIBRATION_SET_PARAMETERS \
/*  id 	 name				default value */ \
	X(0, FONT_SIZE_Title, 	 FONT_16) \
	X(1, FONT_SIZE_Name, 	 FONT_12_bold) \
	X(2, FONT_SIZE_Pos,		 FONT_12_bold) \
	\
	X(3, FONT_STYLE_Title, 	 Arial) \
	X(4, FONT_STYLE_Name, 	 Arial) \
	X(5, FONT_STYLE_Pos, 	 Comic_Saens_MS) \
	\
	X(6, STR_ID_Title,		 fontID_1) \
	X(7, STR_ID_NameCircle,  fontID_2) \
	X(8, STR_ID_PosLog, 		 fontID_3) \
	X(9, STR_ID_PosPhys, 	 fontID_2) \
	\
	X(10, BK_SCREEN_color, 			 	BLACK) \
	X(11, CIRCLE_FRAME_color, 		 	WHITE) \
	X(12, CIRCLE_FILL_color, 		 	ORANGE) \
	X(13, CIRCLE_FILL_pressColor, 	DARKCYAN) \
	X(14, TITLE_color,  	 YELLOW) \
	\
	X(15, NAME_color, 		 WHITE) \
	X(16, POS_LOG_color, 	 ORANGE) \
	X(17, POS_PHYS_color,  	 DARKYELLOW) \
	X(18, MAXVAL_NAME,  	255)	\
	X(19, MAXVAL_LOG,  	255)	\
	X(20, MAXVAL_PHYS,  	255) \
	X(21, DEBUG_ON,  	1)


#define CIRCLE_MACRO \
/*	 Name  width  x	 y */ \
	X("Circle 1 AAAAAA",  48,  50,  50) \
	X("C 2",  48,  LCD_GetXSize()-150, LCD_GetYSize()-150) \
	X("Circle 3", 148, 300, 140) \
	X("d 4",  76,   0, 300) \
	X("Circle Markielowski 5",  84, 170, 300) \
	X("Circle 6", 108, 650,   1) \
	X("Circle 7", 77, 650,   199)


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

static FILE_NAME(struct) var;

int* FILE_NAME(funcGet)(int offs){
	return (int*)((int*)(&var) + offs);
}

void FILE_NAME(funcSet)(int offs, int val){
	*( (int*)((int*)(&var) + offs) ) = val;
}

void FILE_NAME(debug)(void)
{
	Dbg(1,"\r\ntypedef struct{\r\n");
	#define X(a,b,c) DbgVar(1,200,"   %s	= %s\r\n",getName(b),getName(c));
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X
	DbgVar(1,200,"}%s;\r\n",getName(FILE_NAME(struct)));
}

static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys, uint16_t width, char *name)
{
	#define DISPLAY_COMMA_UNDER_COMMA_
	/*Select only one CIRCLE...*/
	//#define CIRCLE_WITH_FRAME_1
	//#define CIRCLE_WITH_FRAME_2
	#define CIRCLE_WITHOUT_FRAME

	uint16_t ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
	{
#if defined(CIRCLE_WITH_FRAME_1)
		int widthCalculated=LCD_CalculateCircleWidth(width);
		LCD_ShapeWindow	     		(LCD_Rectangle,0,widthCalculated,widthCalculated, 0,        0,     	   widthCalculated,  widthCalculated,  SetColorBoldFrame(bkColor,0), 		bkColor,  	bkColor);
		LCD_ShapeWindow	     		(LCD_Circle,	0,widthCalculated,widthCalculated, 0,        0,     	   width,  				width,   			SetColorBoldFrame(frameColor,bold), fillColor,  bkColor);
		LCD_ShapeWindow		  		(LCD_Circle,	0,widthCalculated,widthCalculated, width/4,width/4+1, 	width/2,				width/2, 			SetColorBoldFrame(frameColor,bold), TRANSPARENT,fillColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Frame,		0,widthCalculated,widthCalculated, 0,			0, 			widthCalculated,	widthCalculated, 							frameColor, 		bkColor, 	bkColor);
#elif defined(CIRCLE_WITH_FRAME_2)
		LCD_ShapeWindow	         (LCD_Rectangle,0,width,width, 0,        	0,     	   width,  width,   	SetColorBoldFrame(bkColor,0), 		bkColor,  	 bkColor);
		LCD_ShapeWindow	         (LCD_Circle,	0,width,width, 0,        	0,     	   width,  width,   	SetColorBoldFrame(frameColor,bold), fillColor,   bkColor);
		LCD_ShapeWindow				(LCD_Circle,	0,width,width, width/4+1,	width/4+1, 	width/2,width/2, 	SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Frame,		0,width,width, 0,				0, 			width,	width, 	frameColor, 								bkColor, 	 bkColor);
#else
		LCD_ShapeWindow	         (LCD_Rectangle,0,width,width, 0,        0,     	   width,  width,   SetColorBoldFrame(bkColor,0), 		  bkColor,  	bkColor);
		LCD_ShapeWindow	         (LCD_Circle,	0,width,width, 0,        0,     	   width,  width,   SetColorBoldFrame(frameColor,bold), fillColor,   bkColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Circle,	0,width,width, width/4+1,width/4+1, width/2,width/2, SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
#endif
		return width;
	}

	StructTxtPxlLen lenStr={0};
	int16_t xPos=0;
	char *ptr=NULL;

#if !defined(CIRCLE_WITH_FRAME_1)
	width = CorrectCirclesWidth(width);
#endif	

	LCD_Xmiddle(SetPos,SetPosAndWidth(log.x,width),NULL,0,NoConstWidth);
	ptr = StrAll(3,GetSelTxt(0,FILE_NAME(Lang),1)," ",name);
	width = ShowCircleIndirect(log.x, log.y, width, 0, var.CIRCLE_FRAME_color, var.CIRCLE_FILL_color, var.BK_SCREEN_color);
	lenStr=LCD_StrChangeColorIndirect(var.STR_ID_NameCircle, LCD_Xmiddle(GetPos,var.STR_ID_NameCircle,ptr,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,log.y+width+2), ptr, fullHight, 0, var.BK_SCREEN_color,var.NAME_color,var.MAXVAL_NAME,NoConstWidth);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	xPos = LCD_Xmiddle(GetPos,var.STR_ID_PosLog,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none));
	lenStr=LCD_StrChangeColorIndirect(var.STR_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_LOG_color,var.MAXVAL_LOG,NoConstWidth);
	xPos = xPos + lenStr.inPixel;
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(3,",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	lenStr=LCD_StrChangeColorIndirect(var.STR_ID_PosLog, xPos, LCD_Ypos(lenStr,GetPos,0), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_LOG_color,var.MAXVAL_LOG,NoConstWidth);
#else
	lenStr=LCD_StrChangeColorIndirect(var.STR_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_LOG_color,var.MAXVAL_LOG,NoConstWidth);
#endif

	WaitForTouchState(press);

	BSP_TS_GetState(&TS_State);
	phys->x = TS_State.x;
	phys->y = TS_State.y;

	WaitForTouchState(release);

	width = ShowCircleIndirect(log.x, log.y, width, 0, var.CIRCLE_FRAME_color, var.CIRCLE_FILL_pressColor, var.BK_SCREEN_color);

#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none));
	xPos = xPos - LCD_GetWholeStrPxlWidth(var.STR_ID_PosPhys,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(var.STR_ID_PosPhys, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_PHYS_color,var.MAXVAL_PHYS,NoConstWidth);
#else
	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(var.STR_ID_PosPhys, LCD_Xmiddle(GetPos,var.STR_ID_PosPhys,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_PHYS_color,var.MAXVAL_PHYS,NoConstWidth);
#endif
}

void Touchscreen_Calibration(void)
{
	#define CIRCLES_NUMBER  STRUCT_TAB_SIZE(pos)
	StructTxtPxlLen lenStr={0};
	char *ptr=NULL;

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

	uint16_t width[] = {
		#define X(a,b,c,d) b,
			CIRCLE_MACRO
		#undef X
	};

	XY_Touch_Struct phys[CIRCLES_NUMBER] = {0};

	#define X(a,b,c) FILE_NAME(funcSet)(b,c);
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X

	Delete_TouchLcd_Task();

	LCD_AllRefreshScreenClear();
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();

	FILE_NAME(debug)();

	LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_Title, var.FONT_STYLE_Title, var.STR_ID_Title);
	LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_Name, var.FONT_STYLE_Name, var.STR_ID_NameCircle);
	LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_Pos, var.FONT_STYLE_Pos, var.STR_ID_PosLog);

	DisplayFontsStructState();

	LCD_SetCircleAA(RATIO_AA_VALUE_MAX, RATIO_AA_VALUE_MAX);
	LCD_Clear(var.BK_SCREEN_color);  LCD_Show();

	ptr = GetSelTxt(0,FILE_NAME(Lang),0);
	LCD_Xmiddle(SetPos,SetPosAndWidth(0,LCD_X),NULL,0,NoConstWidth);
	lenStr=LCD_StrChangeColorIndirect(var.STR_ID_Title, LCD_Xmiddle(GetPos,var.STR_ID_Title,ptr,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,0), ptr, fullHight,0,var.BK_SCREEN_color,var.TITLE_color,var.MAXVAL_NAME,NoConstWidth);

	uint8_t status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());

	if(status)
		Dbg(var.DEBUG_ON, TEXT2PRINT(DEBUG_Text_1,1));
	else
	{
	   SetLogXY(pos,CIRCLES_NUMBER);

	   for (int i = 0; i < CIRCLES_NUMBER; i++)
	      GetPhysValues(pos[i], &phys[i], width[i], circlesNames[i]);

	   SetPhysXY(phys,CIRCLES_NUMBER);

	   if(CalcutaleCoeffCalibration(CIRCLES_NUMBER))  //SPRAWDZIC xLog czy wskazuje na srodek czy na poczatek krawedzi !!!!!! musi na srodek !!!!
	   	Dbg(var.DEBUG_ON, TEXT2PRINT(DEBUG_Text_2,1));
	   else{
		   CalibrationWasDone();
		   DisplayCoeffCalibration();
	   }

	   Create_TouchLcd_Task();
	}
}
