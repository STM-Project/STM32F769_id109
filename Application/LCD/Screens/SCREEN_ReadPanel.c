/*
 * SCREEN_ReadPanel.c
 *
 *  Created on: 14.04.2021
 *      Author: RafalMar
 */

#include "SCREEN_ReadPanel.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LCD_BasicGaphics.h"
#include "LCD_fonts_images.h"
#include "string_oper.h"
#include "LCD_Common.h"
#include "tim.h"
#include "timer.h"
#include "mini_printf.h"
#include <string.h>
#include "debug.h"
#include "FreeRTOS.h"
#include "common.h"
#include "sd_card.h"
#include "ff.h"
#include "lang.h"
#include "cpu_utils.h"
#include "cpu_utils.h"
#include "SCREEN_CalibrationLCD.h"
#include "touch.h"

#define TOUCH_GET_PER_X_PROBE		3

enum new_touch{
	Point_1 = 1,
	Point_2,
	Point_3,
	Move_1,
	Move_2,
	Move_3,
	Move_4,
	AnyPress,
	AnyPressWithWait
};

xTaskHandle vtask_ScreensSelectLCD_Handle;

extern const char LANG_ReadPanel_StringType[];

static char bufTemp[50];

#define FLOAT2STR(val)	Float2Str(val,' ',4,Sign_plusMinus,1)
#define INT2STR(val)		  Int2Str(val,'0',3,Sign_none)
#define INT2STR_TIME(val) Int2Str(val,' ',6,Sign_none)
#define ONEBIT(val)	     Int2Str(val,' ',0,Sign_none)

#define TXT_FONT_COLOR 	StrAll(5,INT2STR(Test.font[0])," ",INT2STR(Test.font[1])," ",INT2STR(Test.font[2]))
#define TXT_BK_COLOR 	StrAll(5,INT2STR(Test.bk[0]),  " ",INT2STR(Test.bk[1]),  " ",INT2STR(Test.bk[2]))
#define TXT_FONT_SIZE	StrAll(3,GetSelTxt(0,LANG_ReadPanel_StringType,Test.type),":",LCD_FontSize2Str(bufTemp,Test.size))
#define TXT_FONT_STYLE	LCD_FontStyle2Str(bufTemp,Test.style)
#define TXT_COEFF			Int2Str(Test.coeff  ,' ',3,Sign_plusMinus)
#define TXT_LEN_WIN		Int2Str(Test.lenWin ,' ',3,Sign_none)
#define TXT_OFFS_WIN		Int2Str(Test.offsWin,' ',3,Sign_none)
#define TXT_LOAD_FONT_TIME		StrAll(2,INT2STR_TIME(Test.loadFontTime)," ms")
#define TXT_SPEED					StrAll(2,INT2STR_TIME(Test.speed),       " us")
#define TXT_CPU_USAGE		   StrAll(2,INT2STR(osGetCPUUsage()),"c")

#define RGB_FONT 	RGB2INT(Test.font[0],Test.font[1],Test.font[2])
#define RGB_BK    RGB2INT(Test.bk[0],  Test.bk[1],  Test.bk[2])

typedef enum{
	STR_ID_LenWin = fontVar_1,
	STR_ID_OffsWin,
	STR_ID_LoadFontTime,
	STR_ID_BkColor,
	STR_ID_FontColor,
	STR_ID_FontSize,
	STR_ID_FontStyle,
	STR_ID_Coeff,
	STR_ID_Speed,
	STR_ID_PosCursor,
	STR_ID_Fonts,
	STR_ID_MovFonts_1,
	STR_ID_MovFonts_2,
	STR_ID_MovFonts_3,
	STR_ID_MovFonts_4,
	STR_ID_MovFonts_5,
	STR_ID_MovFonts_6,
	STR_ID_MovFonts_7,
	STR_ID_MovFonts_8,
	STR_ID_test,
	STR_ID_CPU_usage

}LCD_STRID;

typedef enum{
	PARAM_SIZE,
	PARAM_COLOR_BK,
	PARAM_COLOR_FONT,
	PARAM_LEN_WINDOW,
	PARAM_OFFS_WINDOW,
	PARAM_STYLE,
	PARAM_COEFF,
	PARAM_SPEED,
	PARAM_LOAD_FONT_TIME,
	PARAM_POS_CURSOR,
	PARAM_CPU_USAGE,
	PARAM_MOV_TXT,
	FONTS
}REFRESH_DATA;


static int fontIDVar,lenTxt_prev;
StructTxtPxlLen lenStr;

typedef struct{
	int32_t bk[3];
	int32_t font[3];
	int8_t xFontsField;
	int8_t yFontsField;
	int8_t step;
	int16_t coeff;
	int16_t coeff_prev;
	int8_t size;
	uint8_t style;
	uint32_t time;
	uint8_t type;
	char txt[200];
	int16_t lenWin;
	int16_t offsWin;
	int16_t lenWin_prev;
	int16_t offsWin_prev;
	uint8_t normBoldItal;
	uint32_t speed;
	uint32_t loadFontTime;
	uint8_t posCursor;
	uint8_t spaceCoursorY;
	uint8_t heightCursor;
	uint8_t spaceBetweenFonts;
	uint8_t dispChangeColorOrNot;
	uint8_t constWidth;
} RGB_BK_FONT;
static RGB_BK_FONT Test;



static int startScreen=0;

static char* TXT_PosCursor(void){
	return Test.posCursor>0 ? Int2Str(Test.posCursor-1,' ',3,Sign_none) : StrAll(1,"off");
}

static void ClearCursorField(void){
	LCD_ShapeIndirect(LCD_GetStrVar_x(STR_ID_Fonts),LCD_GetStrVar_y(STR_ID_Fonts)+LCD_GetFontHeight(fontIDVar)+Test.spaceCoursorY,LCD_Rectangle, lenStr.inPixel,Test.heightCursor, MYGRAY,MYGRAY,MYGRAY);
}
static void SetCursor(void)  //KURSOR DLA BIG FONT DAC PODWOJNY !!!!!
{
	ClearCursorField();
	if(Test.posCursor)
	{
		uint32_t color;
		switch(Test.type)
		{
		case 0:
			if(Test.dispChangeColorOrNot)
				color=MYGREEN;
			else
				color=RGB_FONT;
			break;
		case 1:
			color=WHITE;
			break;
		case 2:
		default:
			color=BLACK;
			break;
		}
		if(Test.posCursor>Test.lenWin)
			Test.posCursor=Test.lenWin;
		LCD_ShapeIndirect(LCD_GetStrVar_x(STR_ID_Fonts)+LCD_GetStrPxlWidth(fontIDVar,Test.txt,Test.posCursor-1,Test.spaceBetweenFonts,Test.constWidth),LCD_GetStrVar_y(STR_ID_Fonts)+LCD_GetFontHeight(fontIDVar)+Test.spaceCoursorY,LCD_Rectangle, LCD_GetFontWidth(fontIDVar,Test.txt[Test.posCursor-1]),Test.heightCursor, color,color,color);
	}
}

void Data2Refresh(int nr)
{
	switch(nr)
	{
	case PARAM_SIZE:
		LCD_StrVarIndirect(STR_ID_FontSize,TXT_FONT_SIZE);
		break;
	case FONTS:

		switch(Test.type)
		{
		case 0:
			if(Test.dispChangeColorOrNot==0){
				LCD_SetStrVar_fontID(STR_ID_Fonts,fontIDVar);
				LCD_SetStrVar_fontColor(STR_ID_Fonts,RGB_FONT);
				LCD_SetStrVar_bkColor(STR_ID_Fonts,RGB_BK);
				LCD_SetStrVar_coeff(STR_ID_Fonts,Test.coeff);
				StartMeasureTime_us();
				 lenStr=LCD_StrChangeColorVarIndirect(STR_ID_Fonts,Test.txt);
				Test.speed=StopMeasureTime_us("");
			}
			else{
				LCD_SetStrVar_fontID(STR_ID_Fonts,fontIDVar);
				LCD_SetStrVar_bkColor(STR_ID_Fonts,MYGRAY);
				LCD_SetStrVar_coeff(STR_ID_Fonts,0);
				StartMeasureTime_us();
				lenStr=LCD_StrVarIndirect(STR_ID_Fonts,Test.txt);
				Test.speed=StopMeasureTime_us("");
			}
			break;
		case 1:
			LCD_SetStrVar_fontID(STR_ID_Fonts,fontIDVar);
			LCD_SetStrVar_bkColor(STR_ID_Fonts,RGB_BK);
			LCD_SetStrVar_coeff(STR_ID_Fonts,Test.coeff);
			StartMeasureTime_us();
			lenStr=LCD_StrVarIndirect(STR_ID_Fonts,Test.txt);
		   Test.speed=StopMeasureTime_us("");
		   break;
		case 2:
			LCD_SetStrVar_fontID(STR_ID_Fonts,fontIDVar);
			LCD_SetStrVar_bkColor(STR_ID_Fonts,WHITE);
			Test.coeff=0; LCD_SetStrVar_coeff(STR_ID_Fonts,0);
			StartMeasureTime_us();
			lenStr=LCD_StrVarIndirect(STR_ID_Fonts,Test.txt);
		   Test.speed=StopMeasureTime_us("");
			break;
		}
		break;
	case PARAM_COLOR_BK:
		LCD_StrVarIndirect(STR_ID_BkColor,TXT_BK_COLOR);
		break;
	case PARAM_COLOR_FONT:
		LCD_StrVarIndirect(STR_ID_FontColor,TXT_FONT_COLOR);
		break;
	case PARAM_LEN_WINDOW:
		LCD_StrVarIndirect(STR_ID_LenWin, TXT_LEN_WIN);
		break;
	case PARAM_OFFS_WINDOW:
		LCD_StrVarIndirect(STR_ID_OffsWin, TXT_OFFS_WIN);
		break;
	case PARAM_STYLE:
		LCD_StrVarIndirect(STR_ID_FontStyle,TXT_FONT_STYLE);
		break;
	case PARAM_COEFF:
		LCD_StrVarIndirect(STR_ID_Coeff,TXT_COEFF);
		break;
	case PARAM_SPEED:
		LCD_StrVarIndirect(STR_ID_Speed,TXT_SPEED);
		break;
	case PARAM_LOAD_FONT_TIME:
		LCD_StrVarIndirect(STR_ID_LoadFontTime, TXT_LOAD_FONT_TIME);
		break;
	case PARAM_POS_CURSOR:
		LCD_StrVarIndirect(STR_ID_PosCursor,TXT_PosCursor());
		break;
	case PARAM_CPU_USAGE:
		LCD_StrVarIndirect(STR_ID_CPU_usage,TXT_CPU_USAGE);
		break;
	case PARAM_MOV_TXT:
//		if(IsRefreshScreenTimeout(refresh_1,20)){ LCD_StrMovHIndirect(STR_ID_MovFonts_1,1); Data2Refresh(PARAM_CPU_USAGE);  }
//		if(IsRefreshScreenTimeout(refresh_2,20)) LCD_StrMovHIndirect(STR_ID_MovFonts_2,1);
	//	if(IsRefreshScreenTimeout(refresh_3,20)) LCD_StrMovHIndirect(STR_ID_MovFonts_3,1);
		//if(IsRefreshScreenTimeout(refresh_4,20)) LCD_StrMovHIndirect(STR_ID_MovFonts_4,1);
		break;
	}
}

static void RefreshAllParam(void)
{
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COLOR_FONT);
	Data2Refresh(PARAM_COLOR_BK);
	Data2Refresh(PARAM_SIZE);
	Data2Refresh(PARAM_STYLE);
	Data2Refresh(PARAM_COEFF);
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_OFFS_WINDOW);
	Data2Refresh(PARAM_LOAD_FONT_TIME);
	Data2Refresh(PARAM_SPEED);
	Data2Refresh(PARAM_POS_CURSOR);
}


void ChangeValRGB(char font_bk, char rgb, int32_t sign)
{
	int32_t *color;
	int idx;

	switch(font_bk)
	{
	case 'b': color=&Test.bk[0];	break;
	case 'f': color=&Test.font[0];	break;
	}

	switch (rgb)
	{
	case 'R': idx=0; break;
	case 'G': idx=1; break;
	case 'B': idx=2; break;
	}

	switch (sign)
	{
	case 1:
		if(color[idx] <= 255-Test.step)
			color[idx]+=Test.step;
		break;
	case -1:
		if(color[idx] >= Test.step)
			color[idx]-=Test.step;
		break;
	}
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COLOR_FONT);
	Data2Refresh(PARAM_COLOR_BK);
	Data2Refresh(PARAM_SPEED);
}

void IncStepRGB(void){
	Test.step>=255 ? 255 : Test.step++;
}
void DecStepRGB(void){
	Test.step<=0 ? 0 : Test.step--;
}

void IncCoeffRGB(void){
	if(Test.type)
		Test.coeff>=127 ? 127 : Test.coeff++;
	else
		Test.coeff>=255 ? 255 : Test.coeff++;
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COEFF);
	Data2Refresh(PARAM_SPEED);
}
void DecCoeefRGB(void){
	if(Test.type)
		Test.coeff<=-127 ? -127 : Test.coeff--;
	else
		Test.coeff<=0 ? 0 : Test.coeff--;
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COEFF);
	Data2Refresh(PARAM_SPEED);
}

int ChangeTxt(void){
	return CopyCharsTab(Test.txt,Test.lenWin,Test.offsWin,Test.size);
}

void ResetRGB(void)
{
	Test.xFontsField=0;

	Test.bk[0]=0;
	Test.bk[1]=0;
	Test.bk[2]=0;

	Test.font[0]=255;
	Test.font[1]=255;
	Test.font[2]=255;

	Test.step=5;
	Test.coeff=255;
	Test.coeff_prev=Test.coeff;

	Test.type=0;
	Test.speed=0;

	Test.size=FONT_8;
	Test.style=Arial;

	//strcpy(Test.txt,"abcd");

	Test.lenWin=4;
	Test.offsWin=0;

	Test.lenWin_prev=Test.lenWin;
	Test.offsWin_prev=Test.offsWin;

   Test.posCursor=0;

	Test.normBoldItal=0;

	Test.spaceCoursorY=0;
	Test.heightCursor=1;
	Test.spaceBetweenFonts=0;
	Test.constWidth=0;
	Test.dispChangeColorOrNot=0;


	ChangeTxt();
}

static void LCD_LoadFontVar(uint32_t id)
{
	LCD_DeleteFont(fontID_3);
	StartMeasureTime(0);
	switch(Test.type)
	{
	case 0:
		fontIDVar=LCD_LoadFont_DarkgrayGreen(Test.size,Test.style,id);
		break;
	case 1:
		fontIDVar=LCD_LoadFont_DarkgrayWhite(Test.size,Test.style,id);
		break;
	case 2:
		fontIDVar=LCD_LoadFont_WhiteBlack(Test.size,Test.style,id);
		break;
	}
	Test.loadFontTime=StopMeasureTime(0,"");

	if(fontIDVar<0){
		Dbg(1,"\r\nERROR_LoadFontVar ");
		fontIDVar=0;
	}
	DisplayFontsStructState();
	if(startScreen) Data2Refresh(PARAM_LOAD_FONT_TIME);
}

static void AdjustMiddle_X(void){
	LCD_SetStrVar_x(STR_ID_Fonts,LCD_Xmiddle(GetPos,fontIDVar,Test.txt,Test.spaceBetweenFonts,Test.constWidth));
}
static void AdjustMiddle_Y(void){
	LCD_SetStrVar_y(STR_ID_Fonts,LCD_Ymiddle(GetPos,fontIDVar));
}

void ChangeFontStyle(void)
{
	switch(Test.style)
	{
	case Arial:   			 Test.style=Times_New_Roman; break;
	case Times_New_Roman: Test.style=Comic_Saens_MS;  break;
	case Comic_Saens_MS:  Test.style=Arial; 			 break;
	default:              Test.style=Arial;           break;
	}
	ClearCursorField();
	LCD_LoadFontVar(fontID_3);
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_STYLE);
	Data2Refresh(PARAM_SPEED);
}

void Inc_lenWin(void){
	Test.lenWin++;
	if(ChangeTxt()){
		Test.lenWin--;
		ChangeTxt();
	}
	else{
		lenTxt_prev=lenStr.inChar;
		AdjustMiddle_X();
		Data2Refresh(FONTS);
		if(lenTxt_prev==lenStr.inChar)
			Test.lenWin--;
		else
			Data2Refresh(PARAM_LEN_WINDOW);
	}
	Data2Refresh(PARAM_SPEED);
}
void Dec_lenWin(void){
	Test.lenWin<=1 ? 1 : Test.lenWin--;
	ChangeTxt();
	ClearCursorField();
	AdjustMiddle_X();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_SPEED);
}

void Inc_offsWin(void){
	Test.offsWin++;
	if(ChangeTxt()){
		Test.offsWin--;
		ChangeTxt();
	}
	ClearCursorField();
	AdjustMiddle_X();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_OFFS_WINDOW);
	Data2Refresh(PARAM_SPEED);
}
void Dec_offsWin(void){
	Test.offsWin<=0 ? 0 : Test.offsWin--;
	ChangeTxt();
	ClearCursorField();
	AdjustMiddle_X();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_OFFS_WINDOW);
	Data2Refresh(PARAM_SPEED);
}

void IncFontSize(void)
{
	int sizeLimit;
	Test.size+=3;
	switch(Test.normBoldItal)
	{
	default:
	case 0:  sizeLimit=FONT_130; 			break;
	case 1:  sizeLimit=FONT_130_bold; 	break;
	case 2:  sizeLimit=FONT_130_italics; break;
	}
	if(Test.size>sizeLimit){
		Test.size=sizeLimit;
		return;
	}
	ClearCursorField();
	LCD_LoadFontVar(fontID_3);
	if((Test.size==FONT_72)||(Test.size==FONT_72_bold)||(Test.size==FONT_72_italics)||
	   (Test.size==FONT_130)||(Test.size==FONT_130_bold)||(Test.size==FONT_130_italics)){
		Test.lenWin_prev=Test.lenWin;
		Test.offsWin_prev=Test.offsWin;
		Test.lenWin=8;
		Test.offsWin=0;
		ChangeTxt();
	}
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_SIZE);
	Data2Refresh(PARAM_SPEED);
}

void DecFontSize(void)
{
	int sizeLimit;
	if((Test.size==FONT_72)||(Test.size==FONT_72_bold)||(Test.size==FONT_72_italics)||
	   (Test.size==FONT_130)||(Test.size==FONT_130_bold)||(Test.size==FONT_130_italics)){
		Test.lenWin=Test.lenWin_prev;
		Test.offsWin=Test.offsWin_prev;
	}
	Test.size-=3;
	switch(Test.normBoldItal)
	{
	default:
	case 0:  sizeLimit=FONT_8; 		  break;
	case 1:  sizeLimit=FONT_8_bold; 	  break;
	case 2:  sizeLimit=FONT_8_italics; break;
	}
	if(Test.size<sizeLimit) Test.size=sizeLimit;

	ClearCursorField();
	LCD_LoadFontVar(fontID_3);
	ChangeTxt();
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_SIZE);
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_SPEED);
}

void ChangeFontBoldItalNorm(void)
{
	if(Test.normBoldItal>1){
		Test.normBoldItal=0;
		Test.size-=2;
	}
	else{
		Test.normBoldItal++;
		Test.size++;
	}
	ClearCursorField();
	LCD_LoadFontVar(fontID_3);
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_SIZE);
	Data2Refresh(PARAM_SPEED);
}

void ReplaceLcdStrType(void)
{
	INCR_WRAP(Test.type,1,0,2);
	switch(Test.type)
	{
	case 1:
		Test.coeff_prev=Test.coeff;
		Test.coeff=1;
		break;
	case 0:
		Test.coeff=Test.coeff_prev;
		break;
	}
	ClearCursorField();
	LCD_LoadFontVar(fontID_3);
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_SIZE);
	Data2Refresh(PARAM_SPEED);
	Data2Refresh(PARAM_COEFF);
}

void Inc_PosCursor(void){
	if(Test.posCursor<Test.lenWin){
		Test.posCursor++;
		Data2Refresh(PARAM_POS_CURSOR);
		SetCursor();
	}
}
void Dec_PosCursor(void){
	if(Test.posCursor>0){
		Test.posCursor--;
		Data2Refresh(PARAM_POS_CURSOR);
		SetCursor();
	}
}

void IncDec_SpaceBetweenFont(int incDec){
	if(((LCD_GetStrVar_x(STR_ID_Fonts)+lenStr.inPixel>=LCD_GetXSize()-1)&&(1==incDec))||
		((0==LCD_GetStrPxlWidth(fontIDVar,Test.txt,Test.posCursor-1,Test.spaceBetweenFonts,Test.constWidth))&&(0==incDec)))
		return;
	if(Test.posCursor>1){
		if(0xFFFF!=LCD_SelectedSpaceBetweenFontsIncrDecr(incDec, Test.style, Test.size, Test.txt[Test.posCursor-2], Test.txt[Test.posCursor-1])){
			ClearCursorField();
			Data2Refresh(FONTS);
			Test.lenWin=lenStr.inChar;
			SetCursor();
			RefreshAllParam();
		}
	}
}

void DisplayFontsWithChangeColorOrNot(void){
	TOOGLE(Test.dispChangeColorOrNot);
	RefreshAllParam();
}


//static xTaskHandle vtaskWifiHandle;


void vtaskWifi(void *argument)
{
	while (1)
	{

	 	vTaskDelay(1);
	}

}

static void SCREEN_ResetAllParameters(void)
{
	LCD_AllRefreshScreenClear();
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();
}

void NOWY_0(void)
{
	SCREEN_ResetAllParameters();
	ResetRGB();
	LCD_Clear(MYGRAY);
	LCD_LoadFont_DarkgrayWhite(FONT_26, Arial, fontID_1);
	LCD_LoadFont_DarkgrayWhite(FONT_10, Arial, fontID_2);
	LCD_LoadFont_DarkgrayWhite(FONT_12, Arial, fontID_6);
	LCD_LoadFont_DarkgrayGreen(FONT_10, Arial, fontID_7);
	LCD_LoadFont_DarkgrayGreen(FONT_26, Arial, fontID_15);
//	LCD_LoadFont_WhiteBlack(FONT_16, Arial, fontID_11);
//	LCD_LoadFont_WhiteBlack(FONT_11, Arial, fontID_12);

	LCD_LoadFontVar(fontID_3);


	lenStr=LCD_StrVar(STR_ID_FontColor,	 fontID_2, 23, LCD_Ypos(lenStr,SetPos,0), TXT_FONT_COLOR, 	 fullHight,0,MYGRAY,0,1,MYGRAY);
	lenStr=LCD_StrVar(STR_ID_BkColor,	 fontID_2, 23, LCD_Ypos(lenStr,IncPos,5), TXT_BK_COLOR,   	 fullHight,0,MYGRAY,0,1,MYGRAY);

	lenStr=LCD_StrVar(STR_ID_FontSize,	 fontID_2, LCD_Xpos(lenStr,SetPos,23), LCD_Ypos(lenStr,IncPos,5), TXT_FONT_SIZE,      fullHight,0,MYGRAY,0,1,MYGRAY);
	lenStr=LCD_StrVar(STR_ID_FontStyle,	 fontID_2, LCD_Xpos(lenStr,IncPos,70), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_STYLE,     fullHight,0,MYGRAY,0,1,MYGRAY);

	LCD_StrVar(	STR_ID_Coeff,		   fontID_2,150,20, TXT_COEFF, 	       fullHight,0,MYGRAY,0,1,MYGRAY);
	LCD_StrVar(	STR_ID_LenWin,		   fontID_2,400, 0, TXT_LEN_WIN,	       halfHight,0,MYGRAY,0,1,MYGRAY);
	LCD_StrVar(	STR_ID_OffsWin,	   fontID_2,400,20, TXT_OFFS_WIN,  	    halfHight,0,MYGRAY,0,1,MYGRAY);
	LCD_StrVar(	STR_ID_LoadFontTime, fontID_2,320,20, TXT_LOAD_FONT_TIME, halfHight,0,MYGRAY,0,1,MYGRAY);

	LCD_StrDescrVar(STR_ID_PosCursor,fontID_2,440,40,"T: ",TXT_PosCursor(),halfHight,0,MYGRAY,0,1,MYGRAY);

	LCD_StrVar(	STR_ID_CPU_usage, fontID_2,450,0, TXT_CPU_USAGE, halfHight,0,MYGRAY,0,1,MYGRAY);


	 Test.yFontsField=LCD_Ypos(lenStr,IncPos,5);
	 LCD_Ymiddle(SetPos, Test.yFontsField|(LCD_GetYSize()-2)<<16 );
	 LCD_Xmiddle(SetPos, Test.xFontsField|LCD_GetXSize()<<16,"",0,NoConstWidth);

	StartMeasureTime_us();    //daj mozliwosc wpisywania dowolnego textu aby korygowac odstepy miedzy kazdymi fontami jakimi sie chce !!!!!!!
	if(Test.type)
		lenStr=LCD_StrVar(STR_ID_Fonts,fontIDVar, LCD_Xmiddle(GetPos,fontIDVar,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(GetPos,fontIDVar), Test.txt, fullHight, Test.spaceBetweenFonts,MYGRAY,0,Test.constWidth,MYGRAY);
	else
		lenStr=LCD_StrChangeColorVar(STR_ID_Fonts,fontIDVar, LCD_Xmiddle(GetPos,fontIDVar,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(GetPos,fontIDVar), Test.txt, fullHight, Test.spaceBetweenFonts,RGB_BK,RGB_FONT,Test.coeff,Test.constWidth,MYGRAY);
	Test.speed=StopMeasureTime_us("");


	LCD_StrVar(STR_ID_Speed,fontID_2, 150, 0, TXT_SPEED, fullHight, 0,MYGRAY,0,1,MYGRAY);


	LCD_Show();


	//xTaskCreate(vtaskWifi, "vtaskWifi", 1500, NULL, (unsigned portBASE_TYPE ) 0, &vtaskWifiHandle);


}

void NOWY_1(void)  //dac mozliwosc zablokowania Dbg definem!!!
{
	int bkColor=MYGRAY;

	SCREEN_ResetAllParameters();
	LCD_Clear(bkColor);

	LCD_LoadFont_DarkgrayWhite(FONT_10, Arial, fontID_6);
	LCD_LoadFont_DarkgrayGreen(FONT_10, Arial, fontID_7);


	lenStr=LCD_StrRot(Rotate_0,fontID_6,        LCD_Xpos(lenStr,SetPos,5),LCD_Ypos(lenStr,SetPos,5), "123ABCabc", fullHight, 0,DARKRED,1,1);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRot(Rotate_0,fontID_7,       LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "123ABCabc", fullHight, 0,GRAY,MYBLUE,254,1);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrRotWin(Rotate_0,120,fontID_7,       LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "Rafa� Markielowski Agnieszaka Markielowska", fullHight, 0,bkColor,1,ConstWidth,0);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRotWin(Rotate_0,120,fontID_7,       LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "Rafa� Markielowski Agnieszaka Markielowska", fullHight, 0,DARKRED,MAGENTA,244,ConstWidth,0);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);



	lenStr=LCD_StrRot(Rotate_90,fontID_6,     LCD_Xpos(lenStr,SetPos,5),LCD_Ypos(lenStr,IncPos,10), "123ABCabc", fullHight, 0,DARKRED,1,1);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRot(Rotate_90,fontID_7, LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "123ABCabc", fullHight, 0,GRAY,MYBLUE,254,1);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),		 LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrRotWin(Rotate_90,70,fontID_7,       LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "Rafa� Markielowski Agnieszaka Markielowska", fullHight, 0,bkColor,1,1,0);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),		  LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRotWin(Rotate_90,70,fontID_7,LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "Rafa� Markielowski Agnieszaka Markielowska", fullHight, 0,DARKRED,MAGENTA,244,1,0);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),				LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);



	lenStr=LCD_StrRot(Rotate_180,fontID_6,     LCD_Xpos(lenStr,SetPos,5),LCD_Ypos(lenStr,IncPos,10), "123ABCabc", fullHight, 0,DARKRED,1,1);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRot(Rotate_180,fontID_7, LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "123ABCabc", fullHight, 0,GRAY,MYBLUE,254,1);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),		 LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrRotWin(Rotate_180,80,fontID_7,       LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "Rafa� Markielowski Agnieszaka Markielowska", fullHight, 0,bkColor,1,1,0);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),		  LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRotWin(Rotate_180,80,fontID_7,LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), "Raf", fullHight, 0,DARKRED,MAGENTA,244,1,0);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),				LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);


	LCD_Show();
	DisplayFontsStructState();

}

int test[6]={0,0,0,0,0,0};
void NOWY_2(void)  //dac mozliwosc zablokowania Dbg definem!!!
{
	int bkColor=MYGRAY;

	SCREEN_ResetAllParameters();
	LCD_Clear(bkColor);

	LCD_LoadFont_DarkgrayWhite(FONT_14, Arial, fontID_6);
	LCD_LoadFont_DarkgrayGreen(FONT_14, Arial, fontID_7);


	lenStr=LCD_StrRotVar(fontVar_1,Rotate_0,fontID_6,  LCD_Xpos(lenStr,SetPos,5),LCD_Ypos(lenStr,SetPos,5), INT2STR(test[0]), halfHight, 0,DARKRED,1,ConstWidth,bkColor);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),			LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRotVar(fontVar_2,Rotate_0,fontID_7, LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), INT2STR(test[1]), halfHight, 0,GRAY,MYBLUE,249,ConstWidth,bkColor);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),						LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);


	lenStr=LCD_StrRotVar(fontVar_3,Rotate_90,fontID_6, LCD_Xpos(lenStr,SetPos,5),LCD_Ypos(lenStr,IncPos,10), INT2STR(test[2]), halfHight, 0,DARKRED,1,ConstWidth,bkColor);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),			LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRotVar(fontVar_4,Rotate_90,fontID_7, LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), INT2STR(test[3]), halfHight, 0,GRAY,MYBLUE,249,ConstWidth,bkColor);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),						 LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);


	lenStr=LCD_StrRotVar(fontVar_5,Rotate_180,fontID_6,  LCD_Xpos(lenStr,SetPos,5),LCD_Ypos(lenStr,IncPos,10), INT2STR(test[4]), halfHight, 0,DARKRED,1,ConstWidth,bkColor);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),			LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);

	lenStr=LCD_StrChangeColorRotVar(fontVar_6,Rotate_180,fontID_7, LCD_Xpos(lenStr,IncPos,5),LCD_Ypos(lenStr,GetPos,0), INT2STR(test[5]), halfHight, 0,GRAY,MYBLUE,249,ConstWidth,bkColor);
	LCD_Frame(0,LCD_GetXSize(),LCD_GetYSize(),						 LCD_Xpos(lenStr,GetPos,-1),LCD_Ypos(lenStr,GetPos,-1), lenStr.inPixel+2,lenStr.height+2,WHITE,0,0);


	lenStr=LCD_StrRotVar				 (fontVar_7,Rotate_180, fontID_6, LCD_Xpos(lenStr,SetPos,100),LCD_Ypos(lenStr,SetPos,50), "1234567890", halfHight, 0,DARKRED,5,NoConstWidth,bkColor);
	lenStr=LCD_StrChangeColorRotVar(fontVar_8,Rotate_90, fontID_7, LCD_Xpos(lenStr,IncPos,50),  LCD_Ypos(lenStr,GetPos,0),   "1234567890", halfHight, 0,GRAY,MYBLUE,249,ConstWidth,bkColor);

//	StartMeasureTime_us();
//	LCD_RoundRectangle(0,LCD_GetXSize(),LCD_GetYSize(),230,0, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//	StopMeasureTime_us("\r\nLCD_DrawRoundRectangle:");
//
//	StartMeasureTime_us();
//	LCD_RoundFrame(0,LCD_GetXSize(),LCD_GetYSize(),230,50, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//	StopMeasureTime_us("\r\nLCD_DrawRoundFrame:");
//
//	LCD_BoldRoundRectangle(0,LCD_GetXSize(),LCD_GetYSize(),230,100, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//	LCD_BoldRoundFrame(0,LCD_GetXSize(),LCD_GetYSize(),230,150, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//
//	StartMeasureTime_us();
//	LCD_BoldRoundRectangle_AA(0,LCD_GetXSize(),LCD_GetYSize(),350,0, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//	StopMeasureTime_us("\r\nLCD_DrawBoldRoundRectangle_AA:");
//
//	StartMeasureTime_us();
//	LCD_BoldRoundFrame_AA(0,LCD_GetXSize(),LCD_GetYSize(),350,50, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//	StopMeasureTime_us("\r\nLCD_DrawBoldRoundFrame_AA:");
//
//	StartMeasureTime_us();
//	LCD_RoundRectangle_AA(0,LCD_GetXSize(),LCD_GetYSize(),350,100, 100,45,LIGHTGRAY,MYGRAY,MYGRAY);
//	StopMeasureTime_us("\r\nLCD_DrawRoundRectangle_AA:");


	int frameColor=LIGHTGRAY;
	//int fillColor=GRAY;

	Set_AACoeff_RoundFrameRectangle(0.55, 0.73);
	//Set_AACoeff_RoundFrameRectangle(0, 0);
	//Set_AACoeff_RoundFrameRectangle(1, 1);


	StartMeasureTime_us();
	LCD_Shape(240,0,LCD_Frame,100,45,frameColor,bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(240,50,LCD_BoldFrame,100,45,frameColor,bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(240,100,LCD_BoldFrame,100,45,SetColorBoldFrame(frameColor,4),bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(240,150,LCD_RoundFrame,100,45,frameColor,bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(240,200,LCD_BoldRoundFrame,100,45,SetColorBoldFrame(frameColor,3),bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA3:");


	StartMeasureTime_us();
	LCD_Shape(350,0,LCD_Rectangle,100,45,frameColor,bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(350,50,LCD_BoldRectangle,100,45,frameColor,bkColor,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(350,100,LCD_BoldRectangle,100,45,SetColorBoldFrame(GRAY,4),DARKYELLOW,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(350,150,LCD_RoundRectangle,100,45,GRAY,DARKRED,bkColor);  //zrobic jezsli Round to _AA !!!!
	StopMeasureTime_us("\r\nAAAAAAAAAAAA1:");
	StartMeasureTime_us();
	LCD_Shape(350,200,LCD_BoldRoundRectangle,100,45,SetColorBoldFrame(frameColor,3),0x111111,bkColor);
	StopMeasureTime_us("\r\nAAAAAAAAAAAA3:");






//RotWin jako select rozwijane zanaczyc text w select !!!!!!!!!!!!!!!!!!!!!!!!



	LCD_Show();
	DisplayFontsStructState();


	LCD_ShapeIndirect(100,200,LCD_BoldRoundRectangle,100,45, SetColorBoldFrame(frameColor,5),DARKGREEN,bkColor);

}

void NOWY_3(void)  //dac mozliwosc zablokowania Dbg definem!!!
{
	SCREEN_ResetAllParameters();
	ResetRGB();
	LCD_Clear(MYGRAY);
	LCD_LoadFont_DarkgrayWhite(FONT_10, Arial, fontID_6);
	LCD_LoadFont_DarkgrayGreen(FONT_10, Arial, fontID_7);



	// w winHeight dac automatic zeby dopasowac wysokosc do textu
	//przed LCD_StrMovV dac obrys ramki np


// ZMIENIC lOGIKE width i heght gdy tot80 lub 90 !!!

	lenStr=LCD_StrMovH(fontVar_1,Rotate_0,0,109,   fontID_6,  100,0,"AAA11111111111222222222222222 33333333333333333333A",fullHight,0,DARKBLUE,1,1);
	lenStr=LCD_StrMovH(fontVar_2,Rotate_90,0,109,   fontID_6, 0,0,"BBB1111111111222222222222222 33333333333333333333B",fullHight,0,DARKBLUE,1,1);
	lenStr=LCD_StrMovH(fontVar_3,Rotate_180,0,109,   fontID_6, 50,0,"CCCC11111111111222222222222222 33333333333333333333C",fullHight,0,DARKBLUE,1,1);

	lenStr=LCD_StrChangeColorMovH(fontVar_4,Rotate_0,0,80,fontID_7, 100,50,"Zeszyty i ksi��ki to podr�czniki do szko�y dla ��o�A_XY",fullHight,0,DARKYELLOW,DARKRED,254,0);
	lenStr=LCD_StrChangeColorMovH(fontVar_5,Rotate_90,0,80,fontID_7, 400,0,"Zeszyty i ksi��ki to podr�czniki do szko�y dla ��o�A_XY",fullHight,0,DARKYELLOW,DARKRED,254,0);
	lenStr=LCD_StrChangeColorMovH(fontVar_6,Rotate_180,0,80,fontID_7, 435,0,"Zeszyty i ksi��ki to podr�czniki do szko�y dla ��o�A_XY",fullHight,0,DARKYELLOW,DARKRED,254,0);


	lenStr=LCD_StrChangeColorMovV(fontVar_7,Rotate_0,0,100,50,fontID_7,100,100,"Rafa� Markielowski jest ww p omieszczeniu gospodarczym lubi krasnale www doku na drzewie takie jego bojowe zadanie  SEX _XY",fullHight,0,LIGHTGRAY,DARKBLUE,249,0);
	lenStr=LCD_StrMovV           (fontVar_8,Rotate_0,0,100,50,fontID_6,210,100,"1234567890 abcdefghijklnn opqrstuvw ABCDEFGHIJKLMNOPQRTSUVWXYZ iiiiiiiiiiiijjjjjjjjjjjjSSSSSSSSSSEEEEEEEEEERRRRRRRRRA _XY",fullHight,0,MYGRAY,1,0);

	lenStr=LCD_StrChangeColorMovV(fontVar_9,Rotate_90,0,100,50,fontID_7,0,160,"Rafa� Markielowski jest ww p omieszczeniu gospodarczym lubi krasnale www doku na drzewie takie jego bojowe zadanie  SEX _XY",fullHight,0,LIGHTGRAY,DARKBLUE,249,0);
	lenStr=LCD_StrMovV           (fontVar_10,Rotate_90,0,100,50,fontID_6,110,160,"1234567890 abcdefghijklnn opqrstuvw ABCDEFGHIJKLMNOPQRTSUVWXYZ iiiiiiiiiiiijjjjjjjjjjjjSSSSSSSSSSEEEEEEEEEERRRRRRRRRA _XY",fullHight,0,MYGRAY,1,0);

	lenStr=LCD_StrChangeColorMovV(fontVar_11,Rotate_180,0,100,50,fontID_7,220,160,"Rafa� Markielowski jest ww p omieszczeniu gospodarczym lubi krasnale www doku na drzewie takie jego bojowe zadanie  SEX _XY",fullHight,0,LIGHTGRAY,DARKBLUE,249,0);
	lenStr=LCD_StrMovV           (fontVar_12,Rotate_180,0,100,50,fontID_6,330,160,"1234567890 abcdefghijklnn opqrstuvw ABCDEFGHIJKLMNOPQRTSUVWXYZ iiiiiiiiiiiijjjjjjjjjjjjSSSSSSSSSSEEEEEEEEEERRRRRRRRRA _XY",fullHight,0,MYGRAY,1,0);




	// wyzej funkcje pobieraja bufor w ktorym sa przewijane texty zastanowic sie ??




	LCD_Show();
	DisplayFontsStructState();
	LCD_DisplayStrMovBuffState();

}

typedef struct{
	uint8_t startFlag;
	uint16_t width;
	uint8_t bk[3];
	uint8_t frame[3];
	uint8_t fill[3];
	float ratioBk;
	float ratioFill;
	uint32_t speed;
	uint8_t bold;
	uint8_t halfCircle;
	uint16_t deg[10];
	uint32_t degColor[10];
} CIRCLE_PARAM;
static CIRCLE_PARAM Circle;

int test_len=100;

void SCREEN_Test_Circle(void)  //skopiowac pliki do innego projektu bo mam blad z jakims #udef ?????
{
   uint32_t _BkColor		(void){ return RGB2INT( Circle.bk[0],	  Circle.bk[1],    Circle.bk[2]    ); }
   uint32_t _FillColor	(void){ return RGB2INT( Circle.fill[0],  Circle.fill[1],  Circle.fill[2]  ); }
   uint32_t _FrameColor	(void){ return RGB2INT( Circle.frame[0], Circle.frame[1], Circle.frame[2] ); }

	void __Show_FrameAndCircle_Indirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold)
	{
		int widthCalculated=LCD_CalculateCircleWidth(width);
		LCD_ClearPartScreen(3333,widthCalculated,widthCalculated,RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));
		LCD_SetCircleAA(Circle.ratioBk,Circle.ratioFill);
		LCD_ShapeWindow	         (LCD_Circle,3333,widthCalculated,widthCalculated, 0,0, width,          width,          SetColorBoldFrame(_FrameColor(),bold), _FillColor(),_BkColor());
		LCD_ShapeWindowIndirect(x,y,LCD_Frame, 3333,widthCalculated,widthCalculated, 0,0, widthCalculated,widthCalculated,                  _FrameColor(),       _BkColor(),  _BkColor());
	}

	void __Show_Circle_Indirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold)
	{
		LCD_ClearPartScreen(0,width,width,RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));
		LCD_SetCircleAA(Circle.ratioBk,Circle.ratioFill);
		LCD_ShapeIndirect(x,y,LCD_Circle, width,width, SetColorBoldFrame(_FrameColor(),bold), _FillColor(), _BkColor());
	}

	if(Circle.startFlag!=0x52)
	{
		Circle.startFlag=0x52;
		Circle.width=270;

		Circle.bk[0]=R_PART(MYGRAY);
		Circle.bk[1]=G_PART(MYGRAY);
		Circle.bk[2]=B_PART(MYGRAY);

		Circle.frame[0]=R_PART(WHITE);
		Circle.frame[1]=G_PART(WHITE);
		Circle.frame[2]=B_PART(WHITE);

		Circle.fill[0]=R_PART(MYGRAY);
		Circle.fill[1]=G_PART(MYGRAY);
		Circle.fill[2]=B_PART(MYGRAY);

		Circle.ratioBk=0.0;
		Circle.ratioFill=0.0;
		Circle.bold=0;
		Circle.halfCircle=0;

		Circle.deg[0]=45;//45;		Circle.degColor[0]=MYGRAY; //nie wykorzystane !!!!!
		Circle.deg[1]=170;	   Circle.degColor[1]=MYRED;
		Circle.deg[2]=360; /*185 !!!*/    Circle.degColor[2]=DARKYELLOW;
		Circle.deg[3]=215;   Circle.degColor[3]=DARKGREEN;
		Circle.deg[4]=240;   Circle.degColor[4]=DARKBLUE;
		Circle.deg[5]=249;   Circle.degColor[5]=DARKCYAN;
		Circle.deg[6]=310;   Circle.degColor[6]=BROWN;

		SCREEN_ResetAllParameters();
		LCD_LoadFont_DarkgrayWhite(FONT_10, Arial, fontID_1);
		LCD_LoadFont_DarkgrayWhite(FONT_10_bold, Arial, fontID_2);
	}

	CorrectLineAA_off();

	//LCD_SetCircleDegrees(4,Circle.deg[0],Circle.deg[1],Circle.deg[2],Circle.deg[3],Circle.deg[4],Circle.deg[5],Circle.deg[6]);
	//LCD_SetCircleDegreesBuff(4,Circle.deg);
	//LCD_SetCircleDegColors(4,Circle.degColor[0],Circle.degColor[1],Circle.degColor[2],Circle.degColor[3],Circle.degColor[4],Circle.degColor[5],Circle.degColor[6]);
	//LCD_SetCircleDegColorsBuff(4,Circle.degColor);
	LCD_SetCirclePercentParam(2,Circle.deg,Circle.degColor);
	LCD_Clear(RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));  //dac opisy b: to bold !!!!!! na lcd ekranie


	lenStr=LCD_Str(fontID_1, LCD_Xpos(lenStr,SetPos,0), LCD_Ypos(lenStr,SetPos,0), StrAll(2,"Radius } | ",Int2Str(Circle.width,' ',3,Sign_none)), 	 fullHight,0,_BkColor(),1,1);  //'znak' w opisie !!!!!
	lenStr=LCD_Str(fontID_2, LCD_Xpos(lenStr,IncPos,10), LCD_Ypos(lenStr,GetPos,0), StrAll(2,"angle:",INT2STR_TIME(Circle.deg[0])),fullHight,0,_BkColor(),1,0);

	lenStr=LCD_Str(fontID_1, LCD_Xpos(lenStr,SetPos,0), LCD_Ypos(lenStr,IncPos,8), StrAll(6,"Frame: ",INT2STR(Circle.frame[0])," ",INT2STR(Circle.frame[1])," ",INT2STR(Circle.frame[2])),  halfHight,0,_BkColor(),1,1);
	lenStr=LCD_Str(fontID_1, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,5), StrAll(6,"Fill:  ",INT2STR(Circle.fill[0]), " ",INT2STR(Circle.fill[1]), " ",INT2STR(Circle.fill[2])),   halfHight,0,_BkColor(),1,1);
	lenStr=LCD_Str(fontID_1, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,5), StrAll(6,"Backup:",INT2STR(Circle.bk[0]),   " ",INT2STR(Circle.bk[1]),   " ",INT2STR(Circle.bk[2])),   halfHight,0,_BkColor(),1,1);

	lenStr=LCD_Str(fontID_2, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,5), StrAll(4,"AA out:",Float2Str(Circle.ratioBk,' ',1,Sign_none,2),"  AA in:",Float2Str(Circle.ratioFill,' ',1,Sign_none,2)), 	 halfHight,0,_BkColor(),1,1);


//	LCD_SetCircleParam(0.01,0.01,14, 5,4,4,3,3,2,2,2,2,2, 2,2,1,1); // dla tej konfig nie ma Bold  x to srodek kola
//	LCD_Shape(60,160, LCD_Circle, 0,0, _FrameColor(), _FillColor(), _BkColor());


	if(Circle.bold > Circle.width/INCR_WIDTH_CIRCLE_STEP-1)
		Circle.bold=Circle.width/INCR_WIDTH_CIRCLE_STEP-1;

	LCD_SetCircleAA(Circle.ratioBk,Circle.ratioFill);
	StartMeasureTime_us();
	//CorrectLineAA_on();
	//LCD_Shape(480-LCD_GetYSize(),0,LCD_Circle, SetParamWidthCircle(Percent_Circle,Circle.width),Circle.width, SetColorBoldFrame(_FrameColor(),Circle.bold), _FillColor(), _BkColor());

	//LCD_Shape(480-LCD_GetYSize()+10,10,LCD_Circle, Circle.width-20,Circle.width-20, _FrameColor(),  _FillColor(), MYGRAY);


	LCD_Shape(480-LCD_GetYSize(),0,LCD_Circle, Circle.width,Circle.width, SetColorBoldFrame(WHITE/*_FrameColor()*/,Circle.bold), /*TRANSPARENT*/ _FillColor(), _BkColor());  //dla tRANSPARENT nie dziala bold na >niz 0 !!!!
	//LCD_Shape(200,200,LCD_Circle, 50,50, SetColorBoldFrame(WHITE/*_FrameColor()*/,Circle.bold), TRANSPARENT/* _FillColor()*/, _BkColor());
	//LCD_Shape(480-LCD_GetYSize()+6,6,LCD_Circle, Circle.width-5,Circle.width-5, _FrameColor(),  TRANSPARENT/*_FillColor()*/, MYRED);

	//LCD_Shape(480-LCD_GetYSize(),0,LCD_Circle, SetParamWidthCircle(Degree_Circle,Circle.width),Circle.width, SetColorBoldFrame(RED,Circle.bold), _FillColor(), _BkColor());

	//LCD_Shape(480-LCD_GetYSize(),0, LCD_HalfCircle, SetParamWidthCircle(Circle.halfCircle,Circle.width),Circle.width, SetColorBoldFrame(_FrameColor(),Circle.bold), _FillColor(), _BkColor());
	//LCD_Shape(480-LCD_GetYSize(),0,LCD_Frame, LCD_GetCircleWidth(),LCD_GetCircleWidth(), _FrameColor(), _BkColor(), _BkColor()); //RAMKA KWADRATOWA
	Circle.speed=StopMeasureTime_us("");


	lenStr=LCD_Str(fontID_2, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,8), StrAll(3,"Speed: ",INT2STR_TIME(Circle.speed)," us"), 	 halfHight,0,_BkColor(),1,1);
	lenStr=LCD_Str(fontID_2, LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,8), StrAll(2,"Bold: ",Int2Str(Circle.bold,' ',2,Sign_none)), halfHight,0,_BkColor(),1,1);






//--------------------- LINE --------
//	DrawLine(0,130,135,test_len,Circle.deg[0],WHITE,LCD_GetXSize(), Circle.ratioBk,Circle.ratioFill, RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]),RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));  //okreslic jako wewn funkcja !!!! dla bkColor
//	DrawLine(0,230,135,test_len,Circle.deg[0],WHITE,LCD_GetXSize(), 1,1/*Circle.ratioBk,Circle.ratioFill*/, RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]),RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));  //okreslic jako wewn funkcja !!!! dla bkColor
//	CorrectLineAA_on();
//	DrawLine(0,330,135,test_len,Circle.deg[0],WHITE,LCD_GetXSize(), Circle.ratioBk,Circle.ratioFill, RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]),RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));  //okreslic jako wewn funkcja !!!! dla bkColor
//--------------------------------------







	//ZMIENIC na float deg aby dac np 92.4


	//	DrawLine(0,130,190,150,200,MYBLUE,LCD_GetXSize(), Circle.ratioBk,Circle.ratioFill, RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]),RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));
//	DrawLine(0,130,190,150,210,MYBLUE,LCD_GetXSize(), Circle.ratioBk,Circle.ratioFill, RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]),RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));
//	DrawLine(0,130,190,150,250,MYBLUE,LCD_GetXSize(), Circle.ratioBk,Circle.ratioFill, RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]),RGB2INT(Circle.bk[0],Circle.bk[1],Circle.bk[2]));
	LCD_Show();

	// __Show_FrameAndCircle_Indirect(120,180, 54, 1);

	// __Show_FrameAndCircle_Indirect(480-LCD_GetYSize(),0, Circle.width, Circle.bold);
	 __Show_Circle_Indirect(LCD_GetXSize()-50,200, 50, 0);

	 LCD_ShapeIndirect(400,350,LCD_Circle, 50,50, SetColorBoldFrame(WHITE,0), RED, MYGRAY);




//Zrobic kr�z�cy cykl znaczku na okregu w zaleznosci od keyboarda !!!!!!!!!!!!





}


int SCREEN_number=5;  //LOAD IMAGE !!!!!

void SCREEN_ReadPanel(void)
{
	if(startScreen==0)
	{
		switch(SCREEN_number)
		{
		case 0:
			SCREEN_Calibration_funcSet(_FONT_SIZE_PosPhys, SCREEN_Calibration_funcGet(_FONT_SIZE_Title));
			SCREEN_Calibration_funcSet(_BK_SCREEN_color, GRAY);
			SCREEN_Calibration_funcSet(_NAME_color, RED);
			SCREEN_Calibration_funcSet(_CIRCLE_FILL_color, BLUE);
			//NOWY_0();
			SCREEN_Fonts_main();
			startScreen=1;
			break;
		case 1:
			NOWY_1();
			startScreen=1;
			break;
		case 2:
			NOWY_2();
			startScreen=1;
			break;
		case 3:
			NOWY_3();
			startScreen=1;
			break;
		case 4:
			SCREEN_Test_Circle();
			startScreen=1;
			break;
		case 5:
			Touchscreen_Calibration();
			startScreen=1;
			break;
		}
	}
	else
	{
		switch(SCREEN_number)
		{
		case 0:
				 if(DEBUG_RcvStr("1")) ChangeValRGB('f','R',1);
			else if(DEBUG_RcvStr("q")) ChangeValRGB('f','R',-1);
			else if(DEBUG_RcvStr("2")) ChangeValRGB('f','G',1);
			else if(DEBUG_RcvStr("w")) ChangeValRGB('f','G',-1);
			else if(DEBUG_RcvStr("3")) ChangeValRGB('f','B',1);
			else if(DEBUG_RcvStr("e")) ChangeValRGB('f','B',-1);

			else if(DEBUG_RcvStr("a")) ChangeValRGB('b','R',1);
			else if(DEBUG_RcvStr("z")) ChangeValRGB('b','R',-1);
			else if(DEBUG_RcvStr("s")) ChangeValRGB('b','G',1);
			else if(DEBUG_RcvStr("x")) ChangeValRGB('b','G',-1);
			else if(DEBUG_RcvStr("d")) ChangeValRGB('b','B',1);
			else if(DEBUG_RcvStr("c")) ChangeValRGB('b','B',-1);

			else if(DEBUG_RcvStr("f")) IncCoeffRGB();
			else if(DEBUG_RcvStr("v")) DecCoeefRGB();

			else if(DEBUG_RcvStr("g")) IncFontSize();
			else if(DEBUG_RcvStr("b")) DecFontSize();

			else if(DEBUG_RcvStr(" ")) ChangeFontStyle();

			else if(DEBUG_RcvStr("`")) ReplaceLcdStrType();

			else if(DEBUG_RcvStr("r")) ChangeFontBoldItalNorm();

			else if(DEBUG_RcvStr("t")) Dec_offsWin();
			else if(DEBUG_RcvStr("y")) Inc_offsWin();

			else if(DEBUG_RcvStr("u")) Dec_lenWin();
			else if(DEBUG_RcvStr("i")) Inc_lenWin();
			else if(DEBUG_RcvStr("0")) DisplayFontsWithChangeColorOrNot();

			else if(DEBUG_RcvStr("]")) Inc_PosCursor();
			else if(DEBUG_RcvStr("[")) Dec_PosCursor();
			else if(DEBUG_RcvStr("'")) IncDec_SpaceBetweenFont(0);
			else if(DEBUG_RcvStr("\\")) IncDec_SpaceBetweenFont(1);
			else if(DEBUG_RcvStr("/")) LCD_DisplayRemeberedSpacesBetweenFonts();
			else if(DEBUG_RcvStr("o")) LCD_WriteSpacesBetweenFontsOnSDcard();
			else if(DEBUG_RcvStr("m")) LCD_ResetSpacesBetweenFonts();



			else if(DEBUG_RcvStr("h")) LCD_StrChangeColorRotVarIndirect(STR_ID_test,"10");
			else if(DEBUG_RcvStr("j")) LCD_StrChangeColorRotVarIndirect(STR_ID_test,"90");
			break;

		case 1:
			break;

		case 2:
			if(LCD_IsRefreshScreenTimeout(refresh_1,500)) LCD_StrRotVarIndirect				(fontVar_1,INT2STR(++test[0]));
			if(LCD_IsRefreshScreenTimeout(refresh_2,500)) LCD_StrChangeColorRotVarIndirect(fontVar_2,INT2STR(++test[1]));

			if(LCD_IsRefreshScreenTimeout(refresh_3,500)) LCD_StrRotVarIndirect				(fontVar_3,INT2STR(++test[2]));
		   if(LCD_IsRefreshScreenTimeout(refresh_4,500)) LCD_StrChangeColorRotVarIndirect(fontVar_4,INT2STR(++test[3]));

			if(LCD_IsRefreshScreenTimeout(refresh_5,500)) LCD_StrRotVarIndirect				(fontVar_5,INT2STR(++test[4]));
			if(LCD_IsRefreshScreenTimeout(refresh_6,500)) LCD_StrChangeColorRotVarIndirect(fontVar_6,INT2STR(++test[5]));

			     if(DEBUG_RcvStr("1")) LCD_StrRotVarIndirect(fontVar_7,"abAb");
			else if(DEBUG_RcvStr("2")) LCD_StrRotVarIndirect(fontVar_7,"Rafa� Markiel�wski hhhhX");
			else if(DEBUG_RcvStr("3")){ LCD_OffsStrVar_x(fontVar_7,10);  LCD_StrRotVarIndirect(fontVar_7,"XY");  }
			else if(DEBUG_RcvStr("4")){ LCD_OffsStrVar_x(fontVar_7,-10); LCD_StrRotVarIndirect(fontVar_7,"XY");  }
			else if(DEBUG_RcvStr("5")){ LCD_OffsStrVar_y(fontVar_7,10);  LCD_StrRotVarIndirect(fontVar_7,"SD");  }
			else if(DEBUG_RcvStr("6")){ LCD_OffsStrVar_y(fontVar_7,-10); LCD_StrRotVarIndirect(fontVar_7,"SD");  }

			else if(DEBUG_RcvStr("a")) LCD_StrChangeColorRotVarIndirect(fontVar_8,"abAb");
			else if(DEBUG_RcvStr("b")) LCD_StrChangeColorRotVarIndirect(fontVar_8,"Rafa� Markiel�wski hhhhX");
			else if(DEBUG_RcvStr("c")){ LCD_OffsStrVar_x(fontVar_8,10);  LCD_StrChangeColorRotVarIndirect(fontVar_8,"XY");  }
			else if(DEBUG_RcvStr("d")){ LCD_OffsStrVar_x(fontVar_8,-10); LCD_StrChangeColorRotVarIndirect(fontVar_8,"XY");  }
			else if(DEBUG_RcvStr("e")){ LCD_OffsStrVar_y(fontVar_8,10);  LCD_StrChangeColorRotVarIndirect(fontVar_8,"SD");  }
			else if(DEBUG_RcvStr("f")){ LCD_OffsStrVar_y(fontVar_8,-10); LCD_StrChangeColorRotVarIndirect(fontVar_8,"SD");  }
			break;

		case 3:
			if(LCD_IsRefreshScreenTimeout(refresh_1,60)) LCD_StrMovHIndirect(fontVar_1,1);
			if(LCD_IsRefreshScreenTimeout(refresh_2,60)) LCD_StrMovHIndirect(fontVar_2,1);
			if(LCD_IsRefreshScreenTimeout(refresh_3,20)) LCD_StrMovHIndirect(fontVar_3,1);

			if(LCD_IsRefreshScreenTimeout(refresh_4,60)) LCD_StrMovHIndirect(fontVar_4,1);
			if(LCD_IsRefreshScreenTimeout(refresh_5,60)) LCD_StrMovHIndirect(fontVar_5,1);
			if(LCD_IsRefreshScreenTimeout(refresh_6,20)) LCD_StrMovHIndirect(fontVar_6,1);

			if(LCD_IsRefreshScreenTimeout(refresh_7,60)) LCD_StrMovVIndirect(fontVar_7,1);
			if(LCD_IsRefreshScreenTimeout(refresh_8,20)) LCD_StrMovVIndirect(fontVar_8,1);

			if(LCD_IsRefreshScreenTimeout(refresh_9,60))  LCD_StrMovVIndirect(fontVar_9,1);
			if(LCD_IsRefreshScreenTimeout(refresh_10,20)) LCD_StrMovVIndirect(fontVar_10,1);

			if(LCD_IsRefreshScreenTimeout(refresh_11,60))  LCD_StrMovVIndirect(fontVar_11,1);
			if(LCD_IsRefreshScreenTimeout(refresh_12,20)) LCD_StrMovVIndirect(fontVar_12,1);
			break;

		case 4:
				  if(DEBUG_RcvStr("]")){ INCR_WRAP(Circle.width,INCR_WIDTH_CIRCLE_STEP,12,MAX_WIDTH_CIRCLE);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("\\")){ DECR_WRAP(Circle.width,INCR_WIDTH_CIRCLE_STEP,12,MAX_WIDTH_CIRCLE);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("1")){ INCR_WRAP(Circle.frame[0],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("2")){ INCR_WRAP(Circle.frame[1],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("3")){ INCR_WRAP(Circle.frame[2],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("q")){ DECR_WRAP(Circle.frame[0],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("w")){ DECR_WRAP(Circle.frame[1],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("e")){ DECR_WRAP(Circle.frame[2],1,0,255);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("a")){ INCR_WRAP(Circle.bk[0],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("s")){ INCR_WRAP(Circle.bk[1],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("d")){ INCR_WRAP(Circle.bk[2],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("z")){ DECR_WRAP(Circle.bk[0],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("x")){ DECR_WRAP(Circle.bk[1],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("c")){ DECR_WRAP(Circle.bk[2],1,0,255);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("4")){ INCR_WRAP(Circle.fill[0],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("5")){ INCR_WRAP(Circle.fill[1],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("6")){ INCR_WRAP(Circle.fill[2],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("r")){ DECR_WRAP(Circle.fill[0],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("t")){ DECR_WRAP(Circle.fill[1],1,0,255);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("y")){ DECR_WRAP(Circle.fill[2],1,0,255);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("h")){ INCR_FLOAT_WRAP(Circle.ratioBk,  0.10, 0.00, 1.00);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("j")){ INCR_FLOAT_WRAP(Circle.ratioFill,0.10, 0.00, 1.00);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("n")){ DECR_FLOAT_WRAP(Circle.ratioBk  ,0.10, 0.00, 1.00);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("m")){ DECR_FLOAT_WRAP(Circle.ratioFill,0.10, 0.00, 1.00);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("g")){ INCR_WRAP(Circle.bold,1,0,Circle.width/6-1);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("b")){ DECR_WRAP(Circle.bold,1,0,Circle.width/6-1);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("o")){ INCR_WRAP(Circle.halfCircle,1,0,3);  SCREEN_Test_Circle(); }

			else if(DEBUG_RcvStr("k")){ DECR(Circle.deg[0],1,0);   SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("l")){ INCR(Circle.deg[0],1,Circle.deg[1]-1);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("k")){ DECR(Circle.deg[0],1,0);   SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("l")){ INCR(Circle.deg[0],1,360);  SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr(",")){ DECR(Circle.deg[1],1,Circle.deg[0]+1);    SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr(".")){ INCR(Circle.deg[1],1,Circle.deg[2]-1);   SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("9")){ DECR(Circle.deg[2],1,Circle.deg[1]+1);    SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("0")){ INCR(Circle.deg[2],1,Circle.deg[3]-1);   SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("7")){ DECR(Circle.deg[3],1,Circle.deg[2]+1);    SCREEN_Test_Circle(); }
			else if(DEBUG_RcvStr("8")){ INCR(Circle.deg[3],1,360);   SCREEN_Test_Circle(); }  //TU ograniczenie do ostatniego degree a nie do 360 !!!!

			//else if(DEBUG_RcvStr("-")){ INCR(test_len,1,360);   SCREEN_Test_Circle(); }

			break;
//rozpisac wyswietlanie wszystkich degree i uatwic circleLinesLenCorrect  !!!!!!!!!!!!!!!!
		}
	}



	//if(IsRefreshScreenTimeout(refresh_10,2000)){ if(u==0) IncFontSize(); else DecFontSize();  startScreen++; if(startScreen>17){ startScreen=1; u=1-u; } }
	//Data2Refresh(PARAM_MOV_TXT);
//


	//if(IsRefreshScreenTimeout(refresh_7,40)) LCD_StrMovHIndirect_TEST(STR_ID_MovFonts_7,1);
	//if(IsRefreshScreenTimeout(refresh_8,50)) LCD_StrMovVIndirect_TEST(STR_ID_MovFonts_8,1);




	if(DEBUG_RcvStr("=")){ startScreen=0; INCR_WRAP(SCREEN_number,1,0,5); }
	if(DEBUG_RcvStr("n")) LCD_DisplayStrMovBuffState();


}

//	StartMeasureTime_us();
//	LCD_Znak_XX(0,LCD_X,LCD_Y, 240,136, 30,15, LIGHTGRAY,WHITE,WHITE);
//	StopMeasureTime_us("\r\nLCD_Znak_XX:");

void vtask_ScreensSelectLCD(void *pvParameters)
{


	uint16_t state;
	XY_Touch_Struct pos;

	//	 	touchTemp[0].x= 0;
	//	 	touchTemp[0].y= 0;
	//	 	touchTemp[1].x= touchTemp[0].x+200;
	//	 	touchTemp[1].y= touchTemp[0].y+150;
	//	 	SetTouch(ID_TOUCH_POINT,Point_1,press);
	//
	//	 	touchTemp[0].x= 0;
	//	 	touchTemp[0].y= 300;
	//	 	touchTemp[1].x= touchTemp[0].x+200;
	//	 	touchTemp[1].y= touchTemp[0].y+180;
	//	 	SetTouch(ID_TOUCH_POINT,Point_2,release);
	//
	//	 	touchTemp[0].x= 600;
	//	 	touchTemp[0].y= 0;
	//	 	touchTemp[1].x= touchTemp[0].x+200;
	//	 	touchTemp[1].y= touchTemp[0].y+150;
	//	 	SetTouch(ID_TOUCH_POINT,Point_3,release);
	//
	//	 	touchTemp[0].x= LCD_GetXSize()-LCD_GetXSize()/5;
	//	 	touchTemp[1].x= LCD_GetXSize()/5;
	//	 	touchTemp[0].y= 150;
	//	 	touchTemp[1].y= 300;
	//	 	SetTouch(ID_TOUCH_MOVE_LEFT,Move_1,press);
	//
	//	 	touchTemp[0].x= LCD_GetXSize()/5;
	//	 	touchTemp[1].x= LCD_GetXSize()-LCD_GetXSize()/5;
	//	 	touchTemp[0].y= 150;
	//	 	touchTemp[1].y= 300;
	//	 	SetTouch(ID_TOUCH_MOVE_RIGHT,Move_2,release);
	//
	//	 	touchTemp[0].y= LCD_GetYSize()-LCD_GetYSize()/5;
	//	 	touchTemp[1].y= LCD_GetYSize()/5;
	//	 	touchTemp[0].x= 300;
	//	 	touchTemp[1].x= 450;
	//	 	SetTouch(ID_TOUCH_MOVE_UP,Move_3,press);
	//
	//	 	touchTemp[0].y= LCD_GetYSize()/5;
	//	 	touchTemp[1].y= LCD_GetYSize()-LCD_GetYSize()/5;
	//	 	touchTemp[0].x= 500;
	//	 	touchTemp[1].x= 650;
	//	 	SetTouch(ID_TOUCH_MOVE_DOWN,Move_4,release);

		 	touchTemp[0].x= 0;
		 	touchTemp[1].x= 800;
		 	touchTemp[0].y= 0;
		 	touchTemp[1].y= 480;
		 	//SetTouch(ID_TOUCH_GET_ANY_POINT,AnyPress,TOUCH_GET_PER_X_PROBE);
		 	SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT,AnyPressWithWait,TOUCH_GET_PER_X_PROBE);

	while(1)
	{

			SCREEN_ReadPanel();

					state = LCD_Touch_Get(&pos);
					switch(state)
					{
						case Point_1:
							Dbg(1,"\r\nTouchPoint_1");
							break;
						case Point_2:
							Dbg(1,"\r\nTouchPoint_2");
							break;
						case Point_3:
							Dbg(1,"\r\nTouchPoint_3");
							break;
						case Move_1:
							Dbg(1,"\r\nTouchMove_1");
							break;
						case Move_2:
							Dbg(1,"\r\nTouchMove_2");
							break;
						case Move_3:
							Dbg(1,"\r\nTouchMove_3");
							break;
						case Move_4:
							Dbg(1,"\r\nTouchMove_4");
							break;
						case AnyPress:
							DbgVar(1,40,"\r\nAny Press: x= %03d y= %03d", pos.x, pos.y);
							break;
						case AnyPressWithWait:
							DbgVar(1,40,"\r\nAny Press With Wait: x= %03d y= %03d", pos.x, pos.y);
							break;
					}

					vTaskDelay(20);

	}
}

void Create_ScreensSelectLCD_Task(void)
{
	xTaskCreate(vtask_ScreensSelectLCD, (char* )"vtask_ScreensSelectLCD", 2048, NULL, (unsigned portBASE_TYPE ) 1, &vtask_ScreensSelectLCD_Handle);
}

