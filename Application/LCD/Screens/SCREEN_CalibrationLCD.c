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

#include "mini_printf.h"  //do usuniecia

/*#################### -- Main Setting -- ###############################*/

#define FILE_NAME(extend) SCREEN_Calibration_##extend

const char FILE_NAME(Lang)[]="\
Kalibracja LCD,Calibration LCD,\
Ko\xB3o,Circle,\
";\

#define SCREEN_CALIBRATION_SET_PARAMETERS \
/*  id 	 name				default value */ \
	X(0, FONT_SIZE_Title, 	 FONT_16) \
	X(1, FONT_SIZE_Name, 	 FONT_12_bold) \
	X(2, FONT_SIZE_PosLog,		 FONT_12_bold) \
	X(3, FONT_SIZE_PosPhys,		 FONT_12_bold) \
	\
	X(4, FONT_STYLE_Title, 	 Arial) \
	X(5, FONT_STYLE_Name, 	 Arial) \
	X(6, FONT_STYLE_PosLog, 	 Comic_Saens_MS) \
	X(7, FONT_STYLE_PosPhys, 	 Comic_Saens_MS) \
	\
	X(8, BK_SCREEN_color, 			 	BLACK) \
	X(9, CIRCLE_FRAME_color, 		 	WHITE) \
	X(10, CIRCLE_FILL_color, 		 	ORANGE) \
	X(11, CIRCLE_FILL_pressColor, 	DARKCYAN) \
	X(12, TITLE_color,  	 YELLOW) \
	\
	X(13, NAME_color, 		 WHITE) \
	X(14, POS_LOG_color, 	 ORANGE) \
	X(15, POS_PHYS_color,  	 DARKYELLOW) \
	X(16, MAXVAL_NAME,  	255)	\
	X(17, MAXVAL_LOG,  	255)	\
	X(18, MAXVAL_PHYS,  	255) \
	X(19, DEBUG_ON,  	1) \
	\
	X(20, STR_ID_Title,		 fontID_1) \
	X(21, STR_ID_NameCircle, fontID_2) \
	X(22, STR_ID_PosLog, 	 fontID_3) \
	X(23, STR_ID_PosPhys, 	 fontID_4)

/*#################### -- End Main Setting -- ###############################*/

#define CIRCLE_MACRO \
/*	 Name  width  x	 y */ \
	X("Circle 1 AAAAAA",  48,  50,  50) \
	X("C 2",  48,  LCD_GetXSize()-150, LCD_GetYSize()-150) \
	X("Circle 3", 148, 300, 140) \
	X("d 4",  76,   0, 300) \
	X("Circle Markielowski 5",  84, 150, 250) \
	X("Circle 6", 108, 603,   1) \
	X("Circle 7", 77, 650,   199) \
	X("1",  50,   0,   0)


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

int FILE_NAME(funcGet)(int offs){
	return *( (int*)((int*)(&var) + offs) );
}

void FILE_NAME(funcSet)(int offs, int val){
	*( (int*)((int*)(&var) + offs) ) = val;
	SET_bit(SelectBits,offs);
}

void FILE_NAME(debug)(void)
{
	Dbg(1,"\r\ntypedef struct{\r\n");
	#define X(a,b,c) DbgVar(1,200,"%d %s \t\t= %s (%s0x%x)\r\n",a,getName(b),getName(c), CHECK_bit(SelectBits,a)?"change to: ":"", var.b);
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

//#define XXXX //// o 2 kole rozmiar press !!!

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
	width = _ShowCircleIndirect(log.x, log.y, width, 0, var.CIRCLE_FRAME_color, var.CIRCLE_FILL_color, var.BK_SCREEN_color);
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

	width = _ShowCircleIndirect(log.x, log.y, width, 0, var.CIRCLE_FRAME_color, var.CIRCLE_FILL_pressColor, var.BK_SCREEN_color);

#ifdef DISPLAY_COMMA_UNDER_COMMA_
	ptr = StrAll(2,"(",Int2Str(phys->x,None,3,Sign_none));
	xPos = xPos - LCD_GetWholeStrPxlWidth(var.STR_ID_PosPhys,ptr,0,NoConstWidth);
	CorrectPosIfOutRange(&xPos);

	ptr = StrAll(5,"(",Int2Str(phys->x,None,3,Sign_none),",",Int2Str(phys->y,None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(var.STR_ID_PosPhys, xPos, LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_PHYS_color,var.MAXVAL_PHYS,NoConstWidth);
#else
	ptr = StrAll(5,"(",Int2Str(phys->x,None,3,Sign_none),",",Int2Str(phys->y,None,3,Sign_none),")");
	LCD_StrChangeColorIndirect(var.STR_ID_PosPhys, LCD_Xmiddle(GetPos,var.STR_ID_PosPhys,ptr,0,NoConstWidth), LCD_Ypos(lenStr,IncPos,1), ptr, fullHight, 0, var.BK_SCREEN_color,var.POS_PHYS_color,var.MAXVAL_PHYS,NoConstWidth);
#endif
}

void Touchscreen_Calibration(void)
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
	#undef X  // zamien wszystko np HAL_Delay na vTaskDelant itd..




		char test_1[50]="KOCHA";
		char test_2[50]="nie wiem";


		char gg[20]="12345";
		int se = 456789;


  char bnnn[200];

  mini_snprintf(bnnn,200,Clr_   \
				Yel_"\r\nRafal"_X \
				Col_" Markielowski"_X \
				Col_"  AAAAA  "_X  \
				Col_"%s"_X \
				" i zreszta %s" \
				Cya_"---"_X  \
				Red_"%d"_X \
				"  %s",  \
				_Col(font,0,140,250), _Col(bkg,160,70,45),  _Col(bkg,0,140,250),test_1,  test_2,  se,  gg);

 Dbg(1,bnnn);


		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",30,"Markielowski", "1");
		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",30,"Markski", "1");
		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",30,"M", "1");
		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",20,"Markielowskigfhg", "1");
		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",20,"Markielowskk,ki", "1");
		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",20,"Markieki");
		DbgVar2(1,50,"\r\nAAAAAA: %*s : %s  gg",20,"Markielki");


	
	

	Delete_TouchLcd_Task();

	LCD_AllRefreshScreenClear();
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();

	i=LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_Title, 	 var.FONT_STYLE_Title, 	 var.STR_ID_Title);			if(0<=i) var.STR_ID_Title = i;
	i=LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_Name, 	 var.FONT_STYLE_Name, 	 var.STR_ID_NameCircle);	if(0<=i) var.STR_ID_NameCircle = i;
	i=LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_PosLog,  var.FONT_STYLE_PosLog,  var.STR_ID_PosLog);			if(0<=i) var.STR_ID_PosLog = i;
	i=LCD_LoadFont_DarkgrayGreen(var.FONT_SIZE_PosPhys, var.FONT_STYLE_PosPhys, var.STR_ID_PosPhys);		if(0<=i) var.STR_ID_PosPhys = i;

	FILE_NAME(debug)();

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
