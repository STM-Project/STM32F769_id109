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

#define DEBUG_ON	1
#define DEBUG_Text_1  "error_touch"  //odrazu tlumacvznie uruchomic
#define DEBUG_Text_2  "error_calculation_coeff"

#define BK_COLOR	MYGRAY //Zmienna do zapisu

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
   uint16_t _CenterOfCircle(uint16_t x, uint16_t width){
   	return x+width/2;
   }

	StructTxtPxlLen lenStr={0}/*, lenStrTemp={0}*/;
	char *ptr=NULL;
	int16_t xPos=0;

	LCD_Xmiddle(SetPos,SetPosAndWidth(log.x,width),NULL,0,NoConstWidth);
	ShowCircleIndirect(log.x, log.y, width, 0, WHITE, ORANGE, BK_COLOR);
	lenStr=LCD_StrChangeColorIndirect(fontID_2, LCD_Xmiddle(GetPos,fontID_2,name,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,log.y+width+2), name, fullHight, 0, BK_COLOR,DARKGREEN,254,NoConstWidth);

	ptr = StrAll(5,"(",Int2Str(_CenterOfCircle(log.x,width),None,3,Sign_none),",",Int2Str(_CenterOfCircle(log.y,width),None,3,Sign_none),")");
	xPos = LCD_Xmiddle(GetPos,fontID_2,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);




	ptr = StrAll(2,"(",Int2Str(_CenterOfCircle(log.x,width),None,3,Sign_none));
	lenStr=LCD_StrChangeColorIndirect(fontID_2, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_COLOR,ORANGE,254,NoConstWidth);
	//lenStrTemp = lenStr;
	xPos = xPos+lenStr.inPixel;
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(3,",",Int2Str(_CenterOfCircle(log.y,width),None,3,Sign_none),")");
	lenStr=LCD_StrChangeColorIndirect(fontID_2, xPos, LCD_Ypos(lenStr,GetPos,0), ptr, fullHight, 0, BK_COLOR,ORANGE,254,NoConstWidth);

//	ptr = StrAll(5,"(",Int2Str(_CenterOfCircle(log.x,width),None,3,Sign_none),",",Int2Str(_CenterOfCircle(log.y,width),None,3,Sign_none),")");
//	lenStr=LCD_StrChangeColorIndirect(fontID_2, LCD_Xmiddle(GetPos,fontID_2,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_COLOR,ORANGE,254,NoConstWidth);




	WaitForTouchState(press);

	BSP_TS_GetState(&TS_State);
	phys->x = TS_State.x;
	phys->y = TS_State.y;

	WaitForTouchState(release);

	ShowCircleIndirect(log.x, log.y, width, 0, WHITE, RED, BK_COLOR);



	ptr = StrAll(2,"(",Int2Str(_CenterOfCircle(phys->x,width),None,3,Sign_none));
	xPos = xPos - LCD_GetWholeStrPxlWidth(fontID_2,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(5,"(",Int2Str(_CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(_CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(fontID_2, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_COLOR,DARKYELLOW,254,NoConstWidth);


//	ptr = StrAll(5,"(",Int2Str(_CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(_CenterOfCircle(phys->y,width),None,3,Sign_none),")");
//	LCD_StrChangeColorIndirect(fontID_2, LCD_Xmiddle(GetPos,fontID_2,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_COLOR,DARKYELLOW,254,NoConstWidth);



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

	LCD_LoadFont_DarkgrayWhite(FONT_16, Arial, fontID_1);  //definy dla fontID_2 zeby lepiej rozumiec ktore
	LCD_LoadFont_DarkgrayGreen(FONT_12_bold, Arial, fontID_2);

	LCD_SetCircleAA(RATIO_AA_VALUE_MAX, RATIO_AA_VALUE_MAX);
	LCD_Clear(BK_COLOR);  LCD_Show();
	lenStr=LCD_StrIndirect(fontID_1, LCD_Xpos(lenStr,SetPos,200), LCD_Ypos(lenStr,SetPos,0), "Cali,brat,ion LCD", fullHight,0,BK_COLOR,0,0);
	lenStr=LCD_StrIndirect(fontID_1, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,10), "Calibration LCD", fullHight,5,BK_COLOR,0,0);

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
