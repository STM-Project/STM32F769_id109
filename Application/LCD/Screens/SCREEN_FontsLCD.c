
#include "SCREEN_FontsLCD.h"
#include "LCD_BasicGaphics.h"
#include "LCD_fonts_images.h"
#include "LCD_Common.h"
#include "timer.h"
#include "lang.h"

/*----------------- Main Settings ------------------*/

#define FILE_NAME(extend) SCREEN_Fonts_##extend

static const char FILE_NAME(Lang)[]="\
Czcionki LCD,Fonts LCD,\
";\

#define SCREEN_FONTS_SET_PARAMETERS \
/*  id 	 name					default value */ \
	X(0, FONT_SIZE_Title, 	 		FONT_16) \
	X(1, FONT_SIZE_FontColor, 	 	FONT_12_bold) \
	X(2, FONT_SIZE_BkColor,			FONT_12_bold) \
	X(3, FONT_SIZE_FontSize,		FONT_12_bold) \
	\
	\
	X(20, STR_ID_Title,			fontID_1) \
	X(20, STR_ID_FontColor,		fontID_2) \
	X(21, STR_ID_BkColor, 		fontID_2) \
	X(22, STR_ID_FontSize, 	 	fontID_2)

/*------------ End Main Settings -----------------*/

//typedef enum{
//	#define X(a,b,c) b,
//	SCREEN_FONTS_SET_PARAMETERS
//	#undef X
//}FILE_NAME(enum);

static char bufTemp[50];

#define FLOAT2STR(val)	Float2Str(val,' ',4,Sign_plusMinus,1)
#define INT2STR(val)		  Int2Str(val,'0',3,Sign_none)
#define INT2STR_TIME(val) Int2Str(val,' ',6,Sign_none)
#define ONEBIT(val)	     Int2Str(val,' ',0,Sign_none)

#define TXT_FONT_COLOR 	StrAll(5,INT2STR(Test.font[0])," ",INT2STR(Test.font[1])," ",INT2STR(Test.font[2]))
#define TXT_BK_COLOR 	StrAll(5,INT2STR(Test.bk[0]),  " ",INT2STR(Test.bk[1]),  " ",INT2STR(Test.bk[2]))
#define TXT_FONT_SIZE	StrAll(3,GetSelTxt(0,FILE_NAME(Lang),Test.type),":",LCD_FontSize2Str(bufTemp,Test.size))
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
static StructTxtPxlLen lenStr;

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

static void FILE_NAME(setTouch)(void)
{

}

static void SCREEN_ResetAllParameters(void)
{
	LCD_AllRefreshScreenClear();
	LCD_ResetStrMovBuffPos();
	LCD_DeleteAllFontAndImages();
}

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

static void Data2Refresh(int nr)
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


static void ChangeValRGB(char font_bk, char rgb, int32_t sign)
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

static void IncStepRGB(void){
	Test.step>=255 ? 255 : Test.step++;
}
static void DecStepRGB(void){
	Test.step<=0 ? 0 : Test.step--;
}

static void IncCoeffRGB(void){
	if(Test.type)
		Test.coeff>=127 ? 127 : Test.coeff++;
	else
		Test.coeff>=255 ? 255 : Test.coeff++;
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COEFF);
	Data2Refresh(PARAM_SPEED);
}
static void DecCoeefRGB(void){
	if(Test.type)
		Test.coeff<=-127 ? -127 : Test.coeff--;
	else
		Test.coeff<=0 ? 0 : Test.coeff--;
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COEFF);
	Data2Refresh(PARAM_SPEED);
}

static int ChangeTxt(void){
	return CopyCharsTab(Test.txt,Test.lenWin,Test.offsWin,Test.size);
}
static void ResetRGB(void)
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
	DisplayFontsStructState();  //tu dac kolory w dbg!!!!
	if(startScreen) Data2Refresh(PARAM_LOAD_FONT_TIME);
}

static void AdjustMiddle_X(void){
	LCD_SetStrVar_x(STR_ID_Fonts,LCD_Xmiddle(GetPos,fontIDVar,Test.txt,Test.spaceBetweenFonts,Test.constWidth));
}
static void AdjustMiddle_Y(void){
	LCD_SetStrVar_y(STR_ID_Fonts,LCD_Ymiddle(GetPos,fontIDVar));
}

static void ChangeFontStyle(void)
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

static void Inc_lenWin(void){
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
static void Dec_lenWin(void){
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

static void Inc_offsWin(void){
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
static void Dec_offsWin(void){
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

static void IncFontSize(void)
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

static void DecFontSize(void)
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

static void ChangeFontBoldItalNorm(void)
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

static void ReplaceLcdStrType(void)
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

static void Inc_PosCursor(void){
	if(Test.posCursor<Test.lenWin){
		Test.posCursor++;
		Data2Refresh(PARAM_POS_CURSOR);
		SetCursor();
	}
}
static void Dec_PosCursor(void){
	if(Test.posCursor>0){
		Test.posCursor--;
		Data2Refresh(PARAM_POS_CURSOR);
		SetCursor();
	}
}

static void IncDec_SpaceBetweenFont(int incDec){
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

static void DisplayFontsWithChangeColorOrNot(void){
	TOOGLE(Test.dispChangeColorOrNot);
	RefreshAllParam();
}



void FILE_NAME(main)(void)
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
}
