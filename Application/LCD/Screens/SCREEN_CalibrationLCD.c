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

#define FILE_NAME(extand) SCREEN_Calibration_##extand

// zdefiniuj fajne kolory dla X MACRO !!:)  //C/C++/Editor/Syntax Coloring/ -> Preprocessor/Others
//STR_ID_Title  nie moze byc w structurze !!!!
#define SCREEN_CALIBRATION_SET_PARAMETERS \
	/*	 Name				 	param */ \
	X(FONT_SIZE_Title, 	 FONT_16) \
	X(FONT_SIZE_Name, 	 FONT_12_bold) \
	X(FONT_SIZE_Pos,		 FONT_12_bold) \
	\
	X(FONT_STYLE_Title, 	 Arial) \
	X(FONT_STYLE_Name, 	 Arial) \
	X(FONT_STYLE_Pos, 	 Arial) \
	\
	X(STR_ID_Title,		 fontID_1) \
	X(STR_ID_NameCircle,  fontID_2) \
	X(STR_ID_PosLog, 		 fontID_2) \
	X(STR_ID_PosPhys, 	 fontID_2) \
	\
	X(BK_SCREEN_color, 			 MYGRAY) \
	X(CIRCLE_FRAME_color, 		 WHITE) \
	X(CIRCLE_FILL_color, 		 ORANGE) \
	X(CIRCLE_FILL_pressColor, 	 DARKCYAN) \
	X(TITLE_color,  	 YELLOW) \
	X(NAME_color, 		 DARKGREEN) \
	X(POS_LOG_color, 	 ORANGE) \
	X(POS_PHYS_color,  DARKYELLOW)

typedef enum{
	#define X(a,b) a = b,
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X
}FILE_NAME(enum);

typedef struct{
	#define X(a,b) int a;
		SCREEN_CALIBRATION_SET_PARAMETERS
	#undef X
}FILE_NAME(struct);

FILE_NAME(struct) FILE_NAME(var);



int* FILE_NAME(function)(void)
{
	return (int*)(&FILE_NAME(var));
}


// typedef enum{
// 	FONT_SIZE_Title 	= FONT_16,
// 	FONT_SIZE_Name 	= FONT_12_bold,
// 	FONT_SIZE_Pos 		= FONT_12_bold,

// 	FONT_STYLE_Title 	= Arial,
// 	FONT_STYLE_Name 	= Arial,
// 	FONT_STYLE_Pos 	= Arial,

// 	STR_ID_Title 		= fontID_1,
// 	STR_ID_NameCircle = fontID_2,
// 	STR_ID_PosLog 		= fontID_2,
// 	STR_ID_PosPhys 	= fontID_2,

// 	BK_SCREEN_color 			= MYGRAY,
// 	CIRCLE_FRAME_color 		= WHITE,
// 	CIRCLE_FILL_color 		= ORANGE,
// 	CIRCLE_FILL_pressColor 	= RED,

// 	TITLE_color  	= YELLOW,
// 	NAME_color 		= DARKGREEN,
// 	POS_LOG_color 	= ORANGE,
// 	POS_PHYS_color = DARKYELLOW,
// }SCREEN_CALIBRATION;

//sprawdz polskie znaki
const char LANG_ScreenCalibration[]="\
Calibration LCD,Kalibracja LCD,\
Circle,Kolo,\ 
";  

#define TITLE_1  "Calibration LCD"  //odrazu tlumacvznie uruchomic
#define TITLE_2  "Calibration LCDA"

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

//		X("1",  50,   0,   0) \
//		X("2",  51, 100,   0) \
//		X("3",  52, 200,   0) \
//		X("4",  53, 300,   0) \
//		X("5",  54, 400,   0) \
//		X("6",  55, 500,   0) \
//		X("7",  56, 600,   0) \
//		X("8",  57, 700,   0) \
//		X("9",  58,   0,  70) \
//		X("10", 59, 100,  70) \
//		X("11", 60, 200,  70) \
//		X("12", 61, 300,  70) \
//		X("13", 62, 400,  70) \
//		X("14", 63, 500,  70) \
//		X("15", 64, 600,  70) \
//		X("16", 65, 700,  70) \
//		X("17", 66,   0,  170) \
//		X("18", 67, 100,  170) \
//		X("19", 68, 200,  170) \
//		X("20", 69, 300,  170) \
//		X("21", 70, 400,  170) \
//		X("22", 71, 500,  170) \
//		X("23", 72, 600,  170) \
//		X("24", 73, 700,  170) \
//		X("25", 74,   0,  270) \
//		X("26", 75, 100,  270) \
//		X("27", 76, 200,  270) \
//		X("28", 77, 300,  270) \
//		X("29", 78, 400,  270) \
//		X("30", 79, 500,  270) \
//		X("31", 80, 600,  270) \
//		X("32", 81, 700,  270)



static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys, uint16_t width, char *name)
{
	#define DISPLAY_COMMA_UNDER_COMMA_

	uint16_t ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
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
	width = ShowCircleIndirect(log.x, log.y, width, 0, CIRCLE_FRAME_color, CIRCLE_FILL_color, BK_SCREEN_color);
	lenStr=LCD_StrChangeColorIndirect(STR_ID_NameCircle, LCD_Xmiddle(GetPos,STR_ID_NameCircle,name,0,NoConstWidth), LCD_Ypos(lenStr,SetPos,log.y+width+2), name, fullHight, 0, BK_SCREEN_color,NAME_color,254,NoConstWidth);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	xPos = LCD_Xmiddle(GetPos,STR_ID_PosLog,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(log.x,width),None,3,Sign_none));
	lenStr=LCD_StrChangeColorIndirect(STR_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_SCREEN_color,POS_LOG_color,254,NoConstWidth);
	xPos = xPos + lenStr.inPixel;
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(3,",",Int2Str(CenterOfCircle(log.y,width),None,3,Sign_none),")");
	lenStr=LCD_StrChangeColorIndirect(STR_ID_PosLog, xPos, LCD_Ypos(lenStr,GetPos,0), ptr, fullHight, 0, BK_SCREEN_color,POS_LOG_color,254,NoConstWidth);
	#else
	lenStr=LCD_StrChangeColorIndirect(STR_ID_PosLog, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_SCREEN_color,POS_LOG_color,254,NoConstWidth);
	#endif

	WaitForTouchState(press);

	BSP_TS_GetState(&TS_State);
	phys->x = TS_State.x;
	phys->y = TS_State.y;

	WaitForTouchState(release);

	width = ShowCircleIndirect(log.x, log.y, width, 0, CIRCLE_FRAME_color, CIRCLE_FILL_pressColor, BK_SCREEN_color);

	#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none));
	xPos = xPos - LCD_GetWholeStrPxlWidth(STR_ID_PosPhys,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(STR_ID_PosPhys, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_SCREEN_color,POS_PHYS_color,254,NoConstWidth);
	#else
	ptr = StrAll(5,"(",Int2Str(CenterOfCircle(phys->x,width),None,3,Sign_none),",",Int2Str(CenterOfCircle(phys->y,width),None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(STR_ID_PosPhys, LCD_Xmiddle(GetPos,STR_ID_PosPhys,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, BK_SCREEN_color,POS_PHYS_color,254,NoConstWidth);
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
	FILE_NAME(var).NAME_color = 25643;

	//int *pp = SCREEN_Calibration_function();
	//DbgVar(1,100,"%d  %d  %d  %d  %d",SCREEN_Calibration_var.NAME_color, FILE_NAME(var).NAME_color, *(pp+14), *(pp+15), *(pp+16));

	Delete_TouchLcd_Task();

	LCD_AllRefreshScreenClear();
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();

	LCD_LoadFont_DarkgrayGreen(FONT_SIZE_Title, FONT_STYLE_Title, STR_ID_Title);
	LCD_LoadFont_DarkgrayGreen(FONT_SIZE_Name, FONT_STYLE_Title, STR_ID_NameCircle);

	LCD_SetCircleAA(RATIO_AA_VALUE_MAX, RATIO_AA_VALUE_MAX);
	LCD_Clear(BK_SCREEN_color);  LCD_Show();

	lenStr=LCD_StrChangeColorIndirect(STR_ID_Title, LCD_Xpos(lenStr,SetPos,200), LCD_Ypos(lenStr,SetPos,0), TITLE_1, fullHight,0,BK_SCREEN_color,TITLE_color,255,NoConstWidth);
	lenStr=LCD_StrChangeColorIndirect(STR_ID_Title, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,0), TITLE_2, fullHight,0,BK_SCREEN_color,TITLE_color,255,NoConstWidth);

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

	   //HAL_Delay(1000);
	   Create_TouchLcd_Task();
	}
}
