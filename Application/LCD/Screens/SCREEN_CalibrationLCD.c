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
	STR_ID_Title 		= fontID_1,
	STR_ID_NameCircle = fontID_2,
	STR_ID_PosLog 		= fontID_2,
	STR_ID_PosPhys 	= fontID_2
}LCD_STR_ID;

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

#define DEBUG_ON	1
#define DEBUG_Text_1  "error_touch"  //odrazu tlumacvznie uruchomic
#define DEBUG_Text_2  "error_calculation_coeff"

#define CIRCLE_MACRO \
	/*	 Name		 width	x	  y */ \
	X("Circle 1 AAAAAA",  48,  50,  50) \
	X("C 2",  48,  LCD_GetXSize()-150, LCD_GetYSize()-150) \
	X("Circle 3", 148, 300, 140) \
	X("d 4",  76,   0, 300) \
	X("Circle Markielowski 5",  84, 170, 300) \
	X("Circle 6", 108, 650,   1)

static void ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
{
	int widthCalculated = LCD_CalculateCircleWidth(width);

	LCD_ShapeWindow	         (LCD_Rectangle,0,widthCalculated,widthCalculated, 0,        0,     	  width,  width,   SetColorBoldFrame(bkColor,0), 		 bkColor,  	  bkColor);
	LCD_ShapeWindow	         (LCD_Circle,	0,widthCalculated,widthCalculated, 0,        0,     	  width,  width,   SetColorBoldFrame(frameColor,bold), fillColor,   bkColor);
	LCD_ShapeWindowIndirect(x,y,LCD_Circle,	0,widthCalculated,widthCalculated, width/4+1,width/4+1, width/2,width/2, SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
}

static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys, uint8_t width, char *name)
{
	#define DISPLAY_COMMA_UNDER_COMMA_

	StructTxtPxlLen lenStr={0};
	int16_t xPos=0;
	char *ptr=NULL;

	LCD_Xmiddle(SetPos,SetPosAndWidth(log.x,width),NULL,0,NoConstWidth);
	ShowCircleIndirect(log.x, log.y, width, 0, FRAME_color, FILL_color, BK_color);
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

	ShowCircleIndirect(log.x, log.y, width, 0, FRAME_color, FILL_pressColor, BK_color);

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
	StructTxtPxlLen lenStr;

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

	uint8_t width[] = {
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

	lenStr=LCD_StrChangeColorIndirect(STR_ID_Title, LCD_Xpos(lenStr,SetPos,200), LCD_Ypos(lenStr,SetPos,0), "Cali,brat,ion LCD", fullHight,0,BK_color,TITLE_color,255,NoConstWidth);
	lenStr=LCD_StrChangeColorIndirect(STR_ID_Title, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,10), "Cali,brat,ion LCD", fullHight,5,BK_color,TITLE_color,255,NoConstWidth);

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
