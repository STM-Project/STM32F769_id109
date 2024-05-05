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

typedef enum{
	BK_color 			= MYGRAY,
	FRAME_color 		= WHITE,
	FILL_color 			= ORANGE,
	FILL_pressColor 	= RED,
}CIRCLE_COLOR;

typedef enum{
	TITLE_color  	= YELLOW,
	NAME_color 		= DARKGREEN,
	POS_LOG_color 	= ORANGE,
	POS_PHYS_color = DARKYELLOW,
}STR_COLOR;

typedef enum{
	STR_ID_Title 		= fontID_1,
	STR_ID_NameCircle = fontID_2,
	STR_ID_PosLog 		= fontID_2,
	STR_ID_PosPhys 	= fontID_2
}LCD_STR_ID;

#define DEBUG_ON	1
#define DEBUG_Text_1  "error_touch"  //odrazu tlumacvznie uruchomic
#define DEBUG_Text_2  "error_calculation_coeff"

#define TITLE_1  "Calibration LCD"  //odrazu tlumacvznie uruchomic
#define TITLE_2  "Calibration LCDA"

#define CIRCLE_MACRO \
	/*	 Name		 width	x	  y */ \
		X("Circle 1",  50,   0,   0) \
		X("Circle 2",  51, 100,   0) \
		X("Circle 3",  52, 200,   0) \
		X("Circle 4",  53, 300,   0) \
		X("Circle 5",  54, 400,   0) \
		X("Circle 6",  55, 500,   0) \
		X("Circle 7",  56, 600,   0) \
		X("Circle 8",  57, 700,   0) \
		X("Circle 9",  58,   0,  70) \
		X("Circle 10", 59, 100,  70) \
		X("Circle 11", 60, 200,  70) \
		X("Circle 12", 61, 300,  70) \
		X("Circle 13", 62, 400,  70) \
		X("Circle 14", 63, 500,  70) \
		X("Circle 15", 64, 600,  70) \
		X("Circle 16", 65, 700,  70) \
		X("Circle 17", 66,   0,  170) \
		X("Circle 18", 67, 100,  170) \
		X("Circle 19", 68, 200,  170) \
		X("Circle 20", 69, 300,  170) \
		X("Circle 21", 70, 400,  170) \
		X("Circle 22", 71, 500,  170) \
		X("Circle 23", 72, 600,  170) \
		X("Circle 24", 73, 700,  170) \
		X("Circle 25", 74,   0,  270) \
		X("Circle 26", 75, 100,  270) \
		X("Circle 27", 76, 200,  270) \
		X("Circle 28", 77, 300,  270) \
		X("Circle 29", 78, 400,  270) \
		X("Circle 30", 79, 500,  270) \
		X("Circle 31", 80, 600,  270) \
		X("Circle 32", 81, 700,  270)

static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys, uint16_t width, char *name)
{
	#define DISPLAY_COMMA_UNDER_COMMA_

	uint16_t __ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
	{
		LCD_ShapeWindow	         (LCD_Rectangle,0,width,width, 0,        0,     	   width,  width,   SetColorBoldFrame(bkColor,0), 		  bkColor,  	bkColor);
		LCD_ShapeWindow	         (LCD_Circle,	0,width,width, 0,        0,     	   width,  width,   SetColorBoldFrame(frameColor,bold), fillColor,   bkColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Circle,	0,width,width, width/4+1,width/4+1, width/2,width/2, SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
		return width;
	}

	StructTxtPxlLen lenStr={0};
	int16_t xPos=0;
	char *ptr=NULL;

	width = CorrectCirclesWidth(width);

	LCD_Xmiddle(SetPos,SetPosAndWidth(log.x,width),NULL,0,NoConstWidth);
	width = __ShowCircleIndirect(log.x, log.y, width, 0, FRAME_color, FILL_color, BK_color);
	lenStr=LCD_StrChangeColorIndirect(STR_ID_NameCircle, LCD_Xmiddle(GetPos,STR_ID_NameCircle,name,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,log.y+width+2), name, fullHight, 0, BK_color,NAME_color,254,NoConstWidth);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	xPos = LCD_Xmiddle(GetPos,STR_ID_PosLog,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none));
	lenStr=LCD_StrChangeColorIndirect(STR_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_color,POS_LOG_color,254,NoConstWidth);
	xPos = xPos + lenStr.inPixel;
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(3,",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	lenStr=LCD_StrChangeColorIndirect(STR_ID_PosLog, xPos, LCD_Ypos(lenStr,GetPos,0), ptr, fullHight, 0, BK_color,POS_LOG_color,254,NoConstWidth);
	#else
	lenStr=LCD_StrChangeColorIndirect(STR_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_color,POS_LOG_color,254,NoConstWidth);
	#endif

	WaitForTouchState(press);

	BSP_TS_GetState(&TS_State);
	phys->x = TS_State.x;
	phys->y = TS_State.y;

	WaitForTouchState(release);

	width = __ShowCircleIndirect(log.x, log.y, width, 0, FRAME_color, FILL_pressColor, BK_color);

	#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none));
	xPos = xPos - LCD_GetWholeStrPxlWidth(STR_ID_PosPhys,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(STR_ID_PosPhys, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_color,POS_PHYS_color,254,NoConstWidth);
	#else
	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(STR_ID_PosPhys, LCD_Xmiddle(GetPos,STR_ID_PosPhys,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_color,POS_PHYS_color,254,NoConstWidth);
	#endif
}

void Touchscreen_Calibration(void)
{
	#define CIRCLES_NUMBER  STRUCT_TAB_SIZE(pos)
	StructTxtPxlLen lenStr={0};

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

	DeleteTouchLcdTask();

	LCD_AllRefreshScreenClear();
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();

	LCD_LoadFont_DarkgrayGreen(FONT_16, Arial, STR_ID_Title);
	LCD_LoadFont_DarkgrayGreen(FONT_12_bold, Arial, STR_ID_NameCircle);

	LCD_SetCircleAA(RATIO_AA_VALUE_MAX, RATIO_AA_VALUE_MAX);
	LCD_Clear(BK_color);  LCD_Show();

	lenStr=LCD_StrChangeColorIndirect(STR_ID_Title, LCD_Xpos(lenStr,SetPos,200), LCD_Ypos(lenStr,SetPos,0), TITLE_1, fullHight,0,BK_color,TITLE_color,255,NoConstWidth);
	lenStr=LCD_StrChangeColorIndirect(STR_ID_Title, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,0), TITLE_2, fullHight,0,BK_color,TITLE_color,255,NoConstWidth);

	uint8_t status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());

	if(status)
		Dbg(DEBUG_ON, TEXT2PRINT(DEBUG_Text_1,0));
	else
	{
	   SetLogXY(pos);

	   for (int i = 0; i < CIRCLES_NUMBER; i++)
	      GetPhysValues(pos[i], &phys[i], width[i], circlesNames[i]);

	   SetPhysXY(phys);

	   if(CalcutaleCoeffCalibration())
	   	Dbg(DEBUG_ON, TEXT2PRINT(DEBUG_Text_2,0));
	   else{
		   CalibrationWasDone();
		   DisplayCoeffCalibration();
	   }

	   HAL_Delay(1000);
	   CreateTouchLcdTask();
	}
}
