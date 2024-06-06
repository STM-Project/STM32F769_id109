
#include "SCREEN_FontsLCD.h"
#include "LCD_BasicGaphics.h"
#include "LCD_Common.h"
#include "timer.h"
#include "lang.h"
#include "SCREEN_ReadPanel.h"
#include "debug.h"
#include "string_oper.h"
#include "cpu_utils.h"
#include "tim.h"
#include "touch.h"
#include "common.h"
#include "mini_printf.h"
#include "TouchLcdTask.h"
#include "stmpe811.h"

/*----------------- Main Settings ------------------*/
#define FILE_NAME(extend) SCREEN_Fonts_##extend

static const char FILE_NAME(Lang)[]="\
Czcionki LCD,Fonts LCD,\
";\

#define SCREEN_FONTS_SET_PARAMETERS \
/* id   name							default value */ \
	X(0, FONT_SIZE_Title, 	 		FONT_14_bold) \
	X(1, FONT_SIZE_FontColor, 	 	FONT_14_bold) \
	X(2, FONT_SIZE_BkColor,			FONT_14_bold) \
	X(3, FONT_SIZE_FontType,		FONT_14) \
	X(4, FONT_SIZE_FontSize,		FONT_14) \
	X(5, FONT_SIZE_FontStyle,		FONT_14_italics) \
	X(6, FONT_SIZE_Coeff,			FONT_10) \
	X(7, FONT_SIZE_LenWin,			FONT_10) \
	X(8, FONT_SIZE_OffsWin,			FONT_10) \
	X(9, FONT_SIZE_LoadFontTime,	FONT_10) \
	X(10, FONT_SIZE_PosCursor,		FONT_10) \
	X(11, FONT_SIZE_CPUusage,		FONT_10) \
	X(12, FONT_SIZE_Speed,			FONT_10) \
	X(13, FONT_SIZE_Fonts,			FONT_36_bold) \
	\
	X(14, FONT_STYLE_Title, 	 	Arial) \
	X(15, FONT_STYLE_FontColor, 	Arial) \
	X(16, FONT_STYLE_BkColor, 		Arial) \
	X(17, FONT_STYLE_FontType, 	Arial) \
	X(18, FONT_STYLE_FontSize, 	Arial) \
	X(19, FONT_STYLE_FontStyle, 	Arial) \
	X(20, FONT_STYLE_Coeff, 		Arial) \
	X(21, FONT_STYLE_LenWin, 		Arial) \
	X(22, FONT_STYLE_OffsWin, 		Arial) \
	X(23, FONT_STYLE_LoadFontTime,Arial) \
	X(24, FONT_STYLE_PosCursor,	Arial) \
	X(25, FONT_STYLE_CPUusage,		Arial) \
	X(26, FONT_STYLE_Speed,			Arial) \
	X(27, FONT_STYLE_Fonts, 		Arial) \
	\
	X(28, FONT_COLOR_Title,  	 	WHITE) \
	X(29, FONT_COLOR_FontColor, 	WHITE) \
	X(30, FONT_COLOR_BkColor, 	 	WHITE) \
	X(31, FONT_COLOR_FontType,  	WHITE) \
	X(32, FONT_COLOR_FontSize,  	WHITE) \
	X(33, FONT_COLOR_FontStyle,  	WHITE) \
	X(34, FONT_COLOR_Coeff,  		WHITE) \
	X(35, FONT_COLOR_LenWin,  		WHITE) \
	X(36, FONT_COLOR_OffsWin,  	WHITE) \
	X(37, FONT_COLOR_LoadFontTime,WHITE) \
	X(38, FONT_COLOR_PosCursor,	WHITE) \
	X(39, FONT_COLOR_CPUusage,		WHITE) \
	X(40, FONT_COLOR_Speed,			WHITE) \
	X(41, FONT_COLOR_Fonts,  		0xFFE1A000) \
	\
	X(42, FONT_BKCOLOR_Title,  	 	MYGRAY) \
	X(43, FONT_BKCOLOR_FontColor, 	MYGRAY) \
	X(44, FONT_BKCOLOR_BkColor, 	 	MYGRAY) \
	X(45, FONT_BKCOLOR_FontType,  	MYGRAY) \
	X(46, FONT_BKCOLOR_FontSize,  	MYGRAY) \
	X(47, FONT_BKCOLOR_FontStyle,  	MYGRAY) \
	X(48, FONT_BKCOLOR_Coeff,  		MYGRAY) \
	X(49, FONT_BKCOLOR_LenWin,  		MYGRAY) \
	X(50, FONT_BKCOLOR_OffsWin,  		MYGRAY) \
	X(51, FONT_BKCOLOR_LoadFontTime,	MYGRAY) \
	X(52, FONT_BKCOLOR_PosCursor,		MYGRAY) \
	X(53, FONT_BKCOLOR_CPUusage,		MYGRAY) \
	X(54, FONT_BKCOLOR_Speed,			MYGRAY) \
	X(55, FONT_BKCOLOR_Fonts,  		0xFF090440) \
	\
	X(56, FONT_SIZE_Press, 	 	FONT_14_bold) \
	X(57, FONT_STYLE_Press, 	Arial) \
	X(58, FONT_COLOR_Press, 	DARKRED) \
	X(59, FONT_BKCOLOR_Press, 	WHITE) \
	\
	X(60, COLOR_BkScreen,  			COLOR_GRAY(0x38)) \
	X(61, COLOR_MainFrame,  		COLOR_GRAY(0xB0)) \
	X(62, COLOR_FillMainFrame, 	MYGRAY) \
	X(63, COLOR_Frame,  				COLOR_GRAY(0xB0)) \
	X(64, COLOR_FillFrame, 			COLOR_GRAY(0x38)) \
	X(65, COLOR_FramePress, 		DARKRED) \
	X(66, COLOR_FillFramePress,	COLOR_GRAY(0xFF)) \
	X(67, DEBUG_ON,  	1) \
	\
	X(68, FONT_ID_Title,			fontID_1) \
	X(69, FONT_ID_FontColor,	fontID_2) \
	X(70, FONT_ID_BkColor, 		fontID_3) \
	X(71, FONT_ID_FontType, 	fontID_4) \
	X(72, FONT_ID_FontSize, 	fontID_5) \
	X(73, FONT_ID_FontStyle,  	fontID_6) \
	X(74, FONT_ID_Coeff,  		fontID_7) \
	X(75, FONT_ID_LenWin,  		fontID_8) \
	X(76, FONT_ID_OffsWin,  	fontID_9) \
	X(77, FONT_ID_LoadFontTime,fontID_10) \
	X(78, FONT_ID_PosCursor,	fontID_11) \
	X(79, FONT_ID_CPUusage,		fontID_12) \
	X(80, FONT_ID_Speed,			fontID_13) \
	X(81, FONT_ID_Fonts,  		fontID_14) \
	X(82, FONT_ID_Press,  		fontID_15) \
	\
	X(83, FONT_VAR_Title,			fontVar_1) \
	X(84, FONT_VAR_FontColor,		fontVar_2) \
	X(85, FONT_VAR_BkColor, 		fontVar_3) \
	X(86, FONT_VAR_FontType, 		fontVar_4) \
	X(87, FONT_VAR_FontSize, 		fontVar_5) \
	X(88, FONT_VAR_FontStyle,  	fontVar_6) \
	X(89, FONT_VAR_Coeff,  			fontVar_7) \
	X(90, FONT_VAR_LenWin,  		fontVar_8) \
	X(91, FONT_VAR_OffsWin,  		fontVar_9) \
	X(92, FONT_VAR_LoadFontTime,	fontVar_10) \
	X(93, FONT_VAR_PosCursor,		fontVar_11) \
	X(94, FONT_VAR_CPUusage,		fontVar_12) \
	X(95, FONT_VAR_Speed,			fontVar_13) \
	X(96, FONT_VAR_Fonts,  			fontVar_14) \
	X(97, FONT_VAR_Press,  			fontVar_15) \
/*------------ End Main Settings -----------------*/

/*------------ Main Screen MACRO -----------------*/
typedef enum{
	#define X(a,b,c) b,
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
}FILE_NAME(enum);


typedef struct{
	#define X(a,b,c) int b;
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
}FILE_NAME(struct);

static FILE_NAME(struct) v ={
	#define X(a,b,c) c,
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
};

static uint64_t FILE_NAME(SelBits) = 0;
static uint64_t FILE_NAME(SelTouch) = 0;
/*
static int FILE_NAME(SetDefaultParam)(int param){
	int temp;
	#define X(a,b,c) \
		if(b==param){ v.b=c; temp=c; }
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
		return temp;
}
*/
static int FILE_NAME(GetDefaultParam)(int param){
	int temp;
	#define X(a,b,c) \
		if(b==param) temp=c;
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
		return temp;
}

void FILE_NAME(printInfo)(void){
	if(v.DEBUG_ON){
		Dbg(1,Clr_ CoG2_"\r\ntypedef struct{\r\n"_X);
		DbgVar2(1,200,CoGr_"%*s %*s %*s %s\r\n"_X, -8,"id", -18,"name", -15,"default value", "value");
		#define X(a,b,c) DbgVar2(1,200,CoGr_"%*d"_X	"%*s" 	CoGr_"= "_X	 	"%*s" 	"(%s0x%x)\r\n",-4,a,		-23,getName(b),	-15,getName(c), 	CHECK_bit(FILE_NAME(SelBits),a)?CoR_"change to: "_X:"", v.b);
			SCREEN_FONTS_SET_PARAMETERS
		#undef X
		DbgVar(1,200,CoG2_"}%s;\r\n"_X,getName(FILE_NAME(struct)));
	}
}

int FILE_NAME(funcGet)(int offs){
	return *( (int*)((int*)(&v) + offs) );
}

void FILE_NAME(funcSet)(int offs, int val){
	*( (int*)((int*)(&v) + offs) ) = val;
	SET_bit(FILE_NAME(SelBits),offs);
}

void FILE_NAME(setDefaultAllParam)(int rst){
	#define X(a,b,c) FILE_NAME(funcSet)(b,c);
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
	if(rst)
		FILE_NAME(SelBits)=0;
}

void FILE_NAME(debugRcvStr)(void);
void FILE_NAME(setTouch)(void);

void 	FILE_NAME(main)(int argNmb, char **argVal);
/*------------ End Main Screen MACRO -----------------*/

#define TEXT_TO_SHOW		"Rafa"ł" Markielowski"

#define FLOAT2STR(val)	Float2Str(val,' ',4,Sign_plusMinus,1)
#define INT2STR(val)		  Int2Str(val,'0',3,Sign_none)
#define INT2STR_TIME(val) Int2Str(val,' ',6,Sign_none)
#define ONEBIT(val)	     Int2Str(val,None,1,Sign_none)

#define TXT_FONT_COLOR 	StrAll(7," ",INT2STR(Test.font[0])," ",INT2STR(Test.font[1])," ",INT2STR(Test.font[2])," ")
#define TXT_BK_COLOR 	StrAll(7," ",INT2STR(Test.bk[0]),  " ",INT2STR(Test.bk[1]),  " ",INT2STR(Test.bk[2])," ")
#define TXT_FONT_TYPE	StrAll(5," ",ONEBIT(Test.type+1)," ",LCD_FontType2Str(bufTemp,0,Test.type+1)+1," ")
#define TXT_FONT_SIZE	StrAll(3," ",LCD_FontSize2Str(bufTemp+25,Test.size)," ")
#define TXT_FONT_STYLE	StrAll(3," ",LCD_FontStyle2Str(bufTemp,Test.style)," ")
#define TXT_COEFF			Int2Str(Test.coeff  ,' ',3,Sign_plusMinus)
#define TXT_LEN_WIN		Int2Str(Test.lenWin ,' ',3,Sign_none)
#define TXT_OFFS_WIN		Int2Str(Test.offsWin,' ',3,Sign_none)
#define TXT_LOAD_FONT_TIME		StrAll(2,INT2STR_TIME(Test.loadFontTime)," ms")
#define TXT_SPEED					StrAll(2,INT2STR_TIME(Test.speed),       " us")
#define TXT_CPU_USAGE		   StrAll(2,INT2STR(osGetCPUUsage()),"c")

#define RGB_FONT 	RGB2INT(Test.font[0],Test.font[1],Test.font[2])
#define RGB_BK    RGB2INT(Test.bk[0],  Test.bk[1],  Test.bk[2])

typedef enum{
	NoTouch,
	Point_1,
	Point_2,
	Point_3,
	Point_4,
	Point_5,
	Point_6,
	Point_7,
	Point_8,
	Point_9,
	Point_10,
	Point_11,
	Point_12,
	Move_1,
	Move_2,
	Move_3,
	Move_4,
	AnyPress,
	AnyPressWithWait
}NEW_TOUCH;

typedef enum{
	PARAM_TYPE,
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

typedef enum{
	All_Block,
	Block_1,
	Block_2,
	Block_3,
	Block_4,
	Block_5,
	Block_6,
	All_Block_Indirect
}SELECT_BLOCK;

static char bufTemp[50];
static int lenTxt_prev;
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
	char txt[200];  //!!!!!!!!!!!!!!!!! ZMIANA TEXTU NA INNY DOWOLNY
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

static char* TXT_PosCursor(void){
	return Test.posCursor>0 ? Int2Str(Test.posCursor-1,' ',3,Sign_none) : StrAll(1,"off");
}

static void ClearCursorField(void){         //MYGRAY,MYGRAY,MYGRAY  popraw to !!!!!!!!!!!!!!!!
	LCD_ShapeIndirect(LCD_GetStrVar_x(v.FONT_VAR_Fonts),LCD_GetStrVar_y(v.FONT_VAR_Fonts)+LCD_GetFontHeight(v.FONT_ID_Fonts)+Test.spaceCoursorY,LCD_Rectangle, lenStr.inPixel,Test.heightCursor, v.COLOR_BkScreen,v.COLOR_BkScreen,v.COLOR_BkScreen);
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
		LCD_ShapeIndirect(LCD_GetStrVar_x(v.FONT_VAR_Fonts)+LCD_GetStrPxlWidth(v.FONT_ID_Fonts,Test.txt,Test.posCursor-1,Test.spaceBetweenFonts,Test.constWidth),LCD_GetStrVar_y(v.FONT_VAR_Fonts)+LCD_GetFontHeight(v.FONT_ID_Fonts)+Test.spaceCoursorY,LCD_Rectangle, LCD_GetFontWidth(v.FONT_ID_Fonts,Test.txt[Test.posCursor-1]),Test.heightCursor, color,color,color);
	}
}




static void LCD_DrawMainFrame(figureShape shape, int directDisplay, uint8_t bold, uint16_t x,uint16_t y, uint16_t w,uint16_t h, int frameColor,int fillColor,int bkColor)// zastanowic czy nie dac to do BasicGraphic.c
{
	figureShape pShape[4] = {LCD_Rectangle, LCD_BoldRectangle, LCD_RoundRectangle, LCD_BoldRoundRectangle};

	if(shape==pShape[1] || shape==pShape[3])
		frameColor = SetColorBoldFrame(frameColor,bold);

	if(shape==pShape[2] || shape==pShape[3])
		Set_AACoeff_RoundFrameRectangle(0.55, 0.73);

	if(IndDisp==directDisplay)
		LCD_ShapeIndirect(x,y,shape,w,h,frameColor,fillColor,bkColor);
	else
		LCD_Shape(x,y,shape,w,h,frameColor,fillColor,bkColor);
}

static void LCD_FrameForStr(int idVar, uint32_t bkScreenColor, char *str)
{
	int spac= LCD_GetFontWidth(LCD_GetStrVar_fontID(v.FONT_VAR_FontStyle),' ');

	LCD_DrawMainFrame(LCD_RoundRectangle,IndDisp,0, \
		LCD_GetStrVar_x(idVar)-spac, LCD_GetStrVar_y(idVar)-2, \
		LCD_GetWholeStrPxlWidth(LCD_GetStrVar_fontID(idVar),str,0,NoConstWidth)+2*spac, LCD_GetFontHeight(LCD_GetStrVar_fontID(idVar))+4, \
		LCD_GetStrVar_bkColor(idVar), LCD_GetStrVar_bkColor(idVar), bkScreenColor);
}

static void Data2Refresh(int nr)
{
	switch(nr)
	{
	case PARAM_TYPE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontType,TXT_FONT_TYPE);
		SetTouchForNewEndPos(v.FONT_VAR_FontType, lenStr);
		break;
	case PARAM_SIZE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontSize,TXT_FONT_SIZE);
		SetTouchForNewEndPos(v.FONT_VAR_FontSize, lenStr);
		break;
	case FONTS:

		switch(Test.type)
		{
		case 0:
			if(Test.dispChangeColorOrNot==0){
				LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
				LCD_SetStrVar_fontColor(v.FONT_VAR_Fonts,RGB_FONT);
				LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,RGB_BK);
				LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,Test.coeff);
				StartMeasureTime_us();



				LCD_FrameForStr(v.FONT_VAR_Fonts, v.COLOR_BkScreen, Test.txt );

				 lenStr=LCD_StrChangeColorVarIndirect(v.FONT_VAR_Fonts,Test.txt);
				Test.speed=StopMeasureTime_us("");
			}
			else{
				LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
				LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,MYGRAY);
				LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,0);
				StartMeasureTime_us();
				lenStr=LCD_StrVarIndirect(v.FONT_VAR_Fonts,Test.txt);
				Test.speed=StopMeasureTime_us("");
			}
			break;
		case 1:
			LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
			LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,RGB_BK);
			LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,Test.coeff);
			StartMeasureTime_us();
			lenStr=LCD_StrVarIndirect(v.FONT_VAR_Fonts,Test.txt);
		   Test.speed=StopMeasureTime_us("");
		   break;
		case 2:
			LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
			LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,WHITE);
			Test.coeff=0; LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,0);
			StartMeasureTime_us();
			lenStr=LCD_StrVarIndirect(v.FONT_VAR_Fonts,Test.txt);
		   Test.speed=StopMeasureTime_us("");
			break;
		}
		break;
	case PARAM_COLOR_BK:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_BkColor,TXT_BK_COLOR);
		break;
	case PARAM_COLOR_FONT:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontColor,TXT_FONT_COLOR);
		break;
	case PARAM_LEN_WINDOW:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_LenWin, TXT_LEN_WIN);
		break;
	case PARAM_OFFS_WINDOW:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_OffsWin, TXT_OFFS_WIN);
		break;
	case PARAM_STYLE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontStyle, TXT_FONT_STYLE);
		SetTouchForNewEndPos(v.FONT_VAR_FontStyle, lenStr);
		break;
	case PARAM_COEFF:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_Coeff,TXT_COEFF);
		break;
	case PARAM_SPEED:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_Speed,TXT_SPEED);
		break;
	case PARAM_LOAD_FONT_TIME:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_LoadFontTime, TXT_LOAD_FONT_TIME);
		break;
	case PARAM_POS_CURSOR:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_PosCursor,TXT_PosCursor());
		break;
	case PARAM_CPU_USAGE:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_CPUusage,TXT_CPU_USAGE);
		break;
	}
}

static void RefreshAllParam(void)
{
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COLOR_FONT);
	Data2Refresh(PARAM_COLOR_BK);
	Data2Refresh(PARAM_TYPE);
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
		else
			color[idx]=255;
		break;
	case -1:
		if(color[idx] >= Test.step)
			color[idx]-=Test.step;
		else
			color[idx]=0;
		break;
	}
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COLOR_FONT);
	Data2Refresh(PARAM_COLOR_BK);
	Data2Refresh(PARAM_SPEED);
}
/*
static void IncStepRGB(void){
	Test.step>=255 ? 255 : Test.step++;
}
static void DecStepRGB(void){
	Test.step<=0 ? 0 : Test.step--;
}
*/
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

static int ChangeTxt(void){ //wprowadzanie z klawiatury textu !!!!!!
	//return CopyCharsTab(Test.txt,Test.lenWin,Test.offsWin,Test.size);

	const char *pChar;
	int i, j, lenChars;

	pChar= TEXT_TO_SHOW;

	lenChars=mini_strlen(pChar);
	for(i=0;i < Test.lenWin;++i)
	{
		j=Test.offsWin + i;
		if(j < lenChars)
			Test.txt[i]=pChar[j];
		else
			break;
	}
	Test.txt[i]=0;

	if(i == Test.lenWin)
		return 0;
	else
		return 1;
}

static void ResetRGB(void)
{
	Test.xFontsField=0;

	Test.bk[0]=R_PART(v.FONT_BKCOLOR_Fonts);
	Test.bk[1]=G_PART(v.FONT_BKCOLOR_Fonts);
	Test.bk[2]=B_PART(v.FONT_BKCOLOR_Fonts);

	Test.font[0]=R_PART(v.FONT_COLOR_Fonts);
	Test.font[1]=G_PART(v.FONT_COLOR_Fonts);
	Test.font[2]=B_PART(v.FONT_COLOR_Fonts);

	Test.step=1;
	Test.coeff=255;
	Test.coeff_prev=Test.coeff;

	Test.type=0;
	Test.speed=0;

	Test.size=v.FONT_SIZE_Fonts;
	Test.style=v.FONT_STYLE_Fonts;

	//strcpy(Test.txt,"Rafa"ł" Markielowski");

	Test.lenWin=mini_strlen(TEXT_TO_SHOW);
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

static void LCD_LoadFontVar(void)
{
	if(v.FONT_ID_Fonts == FILE_NAME(GetDefaultParam)(FONT_ID_Fonts))
		LCD_DeleteFont(FILE_NAME(GetDefaultParam)(FONT_ID_Fonts));

	StartMeasureTime(0);
	switch(Test.type)
	{
	case 0:
		v.FONT_ID_Fonts = LCD_LoadFont_DarkgrayGreen(Test.size,Test.style,FILE_NAME(GetDefaultParam)(FONT_ID_Fonts));
		break;
	case 1:
		v.FONT_ID_Fonts = LCD_LoadFont_DarkgrayWhite(Test.size,Test.style,FILE_NAME(GetDefaultParam)(FONT_ID_Fonts));
		break;
	case 2:
		v.FONT_ID_Fonts = LCD_LoadFont_WhiteBlack(Test.size,Test.style,FILE_NAME(GetDefaultParam)(FONT_ID_Fonts));
		break;
	}
	Test.loadFontTime=StopMeasureTime(0,"");

	if(v.FONT_ID_Fonts<0){
		Dbg(1,"\r\nERROR_LoadFontVar ");
		v.FONT_ID_Fonts=0;
	}
	DisplayFontsStructState();
}

static void AdjustMiddle_X(void){
	LCD_SetStrVar_x(v.FONT_VAR_Fonts,LCD_Xmiddle(GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth));
}
static void AdjustMiddle_Y(void){
	LCD_SetStrVar_y(v.FONT_VAR_Fonts,LCD_Ymiddle(GetPos,v.FONT_ID_Fonts));
}

static void ChangeFontStyle(void)
{
	switch(Test.style)
	{
	case Arial:   			 Test.style=Times_New_Roman; break;
	case Times_New_Roman: Test.style=Comic_Saens_MS;  break;
	case Comic_Saens_MS:  Test.style=Arial; 			  break;
	default:              Test.style=Arial;           break;
	}
	ClearCursorField();
	LCD_LoadFontVar();
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LOAD_FONT_TIME);
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
	LCD_LoadFontVar();
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
	Data2Refresh(PARAM_LOAD_FONT_TIME);
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_TYPE);
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
	LCD_LoadFontVar();
	ChangeTxt();
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LOAD_FONT_TIME);
	Data2Refresh(PARAM_TYPE);
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
	LCD_LoadFontVar();
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LOAD_FONT_TIME);
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_TYPE);
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
	LCD_LoadFontVar();
	AdjustMiddle_X();
	AdjustMiddle_Y();
	Data2Refresh(FONTS);
	Test.lenWin=lenStr.inChar;
	SetCursor();
	Data2Refresh(PARAM_LOAD_FONT_TIME);
	Data2Refresh(PARAM_LEN_WINDOW);
	Data2Refresh(PARAM_TYPE);
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
	if(((LCD_GetStrVar_x(v.FONT_VAR_Fonts)+lenStr.inPixel>=LCD_GetXSize()-1)&&(1==incDec))||
		((0==LCD_GetStrPxlWidth(v.FONT_ID_Fonts,Test.txt,Test.posCursor-1,Test.spaceBetweenFonts,Test.constWidth))&&(0==incDec)))
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

#define KEYBOARD_RGB(shape,bold,FrameColor,fillColor,bkColor,blockNr)	LCD_Keyboard_RGB(shape,bold, 550,160, 60,40, 10, SHAPE_PARAM(FrameColor,fillColor,bkColor),NoTouch,blockNr)

static void LCD_Keyboard_RGB(figureShape shape, uint8_t bold, uint16_t x,uint16_t y, uint16_t width,uint16_t height, uint8_t interSpace, int frameColor,int fillColor,int bkColor, uint16_t touchIdx, uint8_t selBlockPress)
{
	#define	_F(xPos,yPos)	LCD_DrawMainFrame(shape,NoIndDisp,bold, xPos,yPos, width,height, frameColor,fillColor,bkColor)
	#define	_P(xPos,yPos)	LCD_DrawMainFrame(shape,IndDisp,  bold, xPos,yPos, width,height, frameColor,fillColor,bkColor)
	#define	dx	(width + interSpace)
	#define	dy	(height + interSpace)

	switch(selBlockPress){
	case Block_1:	_P(x, y);			break;
	case Block_2:	_P(x+dx, y);		break;
	case Block_3:	_P(x+2*dx, y);		break;
	case Block_4:	_P(x, y+dy);		break;
	case Block_5:	_P(x+dx, y+dy);	break;
	case Block_6:	_P(x+2*dx, y+dy);	break;
	case All_Block_Indirect:
		_P(x, y);		_P(x+dx, y); 		_P(x+2*dx, y);
		_P(x, y+dy);	_P(x+dx, y+dy); 	_P(x+2*dx, y+dy);
		break;
	case All_Block:
		_F(x, y);		_F(x+dx, y); 		_F(x+2*dx, y);
		_F(x, y+dy);	_F(x+dx, y+dy); 	_F(x+2*dx, y+dy);
		break;
	}

	if(touchIdx){
		touchTemp[0].x= x;
		touchTemp[1].x= x+width;
		touchTemp[0].y= y;
		touchTemp[1].y= y+height;
		LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT_WITH_WAIT,touchIdx,TOUCH_GET_PER_X_PROBE);

		touchTemp[0].x= x;
		touchTemp[1].x= x+width;
		touchTemp[0].y= y+dy;
		touchTemp[1].y= y+dy + height;
		LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT_WITH_WAIT,touchIdx+1,TOUCH_GET_PER_X_PROBE);
	}

	#undef _F
	#undef _P
	#undef dx
	#undef dy
}

void FILE_NAME(setTouch)(void)
{
	#define SELECT_CURRENT_FONT(src,dst,txt,coeff) \
		LCD_SetStrVar_fontID		(v.FONT_VAR_##src, v.FONT_ID_##dst);\
		LCD_SetStrVar_fontColor	(v.FONT_VAR_##src, v.FONT_COLOR_##dst);\
		LCD_SetStrVar_bkColor  	(v.FONT_VAR_##src, v.FONT_BKCOLOR_##dst);\
		LCD_SetStrVar_coeff		(v.FONT_VAR_##src, coeff);\
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_##src, txt)

	#define DESELECT_CURRENT_FONT(src,txt) \
		LCD_SetStrVar_fontID		(v.FONT_VAR_##src, v.FONT_ID_##src);\
		LCD_SetStrVar_fontColor	(v.FONT_VAR_##src, v.FONT_COLOR_##src);\
		LCD_SetStrVar_bkColor	(v.FONT_VAR_##src, v.FONT_BKCOLOR_##src);\
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_##src, txt)

	#define DESELECT_ALL_FONTS \
		DESELECT_CURRENT_FONT(FontColor,	TXT_FONT_COLOR);\
		DESELECT_CURRENT_FONT(BkColor,	TXT_BK_COLOR);\
		DESELECT_CURRENT_FONT(FontType,	TXT_FONT_TYPE);\
		DESELECT_CURRENT_FONT(FontSize,	TXT_FONT_SIZE);\
		DESELECT_CURRENT_FONT(FontStyle,	TXT_FONT_STYLE)

	#define CHECK_TOUCH(state)	CHECK_bit(FILE_NAME(SelTouch),state-1)
	#define SET_TOUCH(state) 	SET_bit	(FILE_NAME(SelTouch),state-1)
	#define CLR_TOUCH(state) 	RST_bit	(FILE_NAME(SelTouch),state-1)
	#define CLR_ALL_TOUCH 		FILE_NAME(SelTouch) = 0

	#define CASE_TOUCH_STATE(state,touchPoint, src,dst, txt,coeff) \
		case touchPoint:\
			if(0==CHECK_TOUCH(state)){\
				DESELECT_ALL_FONTS;		CLR_ALL_TOUCH;\
				SELECT_CURRENT_FONT(src, dst, txt, coeff);\
				SET_TOUCH(state);\
			}\
			else{\
				DESELECT_CURRENT_FONT(src, txt);\
				CLR_ALL_TOUCH;\
			}

	static uint16_t statePrev=0;
	uint16_t state;
	XY_Touch_Struct pos;

	void _SaveState(void){
		statePrev = state;
	}
	int _WasState(int point){
		if(release==LCD_TOUCH_isPress() && point==statePrev){
			statePrev = state;
			return 1;
		}
		else return 0;
	}

	state = LCD_TOUCH_GetTypeAndPosition(&pos);
	switch(state)
	{
		CASE_TOUCH_STATE(state,Point_1, FontColor,Press, TXT_FONT_COLOR,252);
			DisplayTouchPosXY(Point_1,pos);  Dbg(1,": TouchPoint_1 ");
			break;

		CASE_TOUCH_STATE(state,Point_2, BkColor,Press, TXT_BK_COLOR,252);
			DisplayTouchPosXY(Point_2,pos);  Dbg(1,": TouchPoint_2 ");
			break;

		CASE_TOUCH_STATE(state,Point_3, FontType,Press, TXT_FONT_TYPE,252);
			DisplayTouchPosXY(Point_3,pos);  Dbg(1,": TouchPoint_3 ");
			break;

		CASE_TOUCH_STATE(state,Point_4, FontSize,Press, TXT_FONT_SIZE,252);
			DisplayTouchPosXY(Point_4,pos);  Dbg(1,": TouchPoint_4 ");
			break;

		CASE_TOUCH_STATE(state,Point_5, FontStyle,Press, TXT_FONT_STYLE,252);
			DisplayTouchPosXY(Point_5,pos);  Dbg(1,": TouchPoint_5 ");
			break;

	case Point_6:
			ChangeValRGB('f', 'R', 1);
			KEYBOARD_RGB(LCD_BoldRoundRectangle,4,FramePress,FillFramePress,BkScreen,Block_1);	Test.step=5;	_SaveState();
			break;
	case Point_7:
			ChangeValRGB('f', 'R', -1);
			KEYBOARD_RGB(LCD_BoldRoundRectangle,4,FramePress,FillFramePress,BkScreen,Block_4);	Test.step=5;	_SaveState();
			break;




		case Point_12:
			Dbg(1,"\r\nTouchPoint_12");
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
		default:

			if(_WasState(Point_6) || _WasState(Point_7))
			{
				KEYBOARD_RGB(LCD_RoundRectangle,0,Frame,FillFrame,BkScreen,All_Block_Indirect);
				Test.step=1;
			}
			break;
	}
}

void FILE_NAME(debugRcvStr)(void)
{
	if(DEBUG_RcvStr("1"))
		ChangeValRGB('f', 'R', 1);
	else if(DEBUG_RcvStr("q"))
		ChangeValRGB('f', 'R', -1);
	else if(DEBUG_RcvStr("2"))
		ChangeValRGB('f', 'G', 1);
	else if(DEBUG_RcvStr("w"))
		ChangeValRGB('f', 'G', -1);
	else if(DEBUG_RcvStr("3"))
		ChangeValRGB('f', 'B', 1);
	else if(DEBUG_RcvStr("e"))
		ChangeValRGB('f', 'B', -1);

	else if(DEBUG_RcvStr("a"))
		ChangeValRGB('b', 'R', 1);
	else if(DEBUG_RcvStr("z"))
		ChangeValRGB('b', 'R', -1);
	else if(DEBUG_RcvStr("s"))
		ChangeValRGB('b', 'G', 1);
	else if(DEBUG_RcvStr("x"))
		ChangeValRGB('b', 'G', -1);
	else if(DEBUG_RcvStr("d"))
		ChangeValRGB('b', 'B', 1);
	else if(DEBUG_RcvStr("c"))
		ChangeValRGB('b', 'B', -1);

	else if(DEBUG_RcvStr("f"))
		IncCoeffRGB();
	else if(DEBUG_RcvStr("v"))
		DecCoeefRGB();

	else if(DEBUG_RcvStr("g"))
		IncFontSize();
	else if(DEBUG_RcvStr("b"))
		DecFontSize();

	else if(DEBUG_RcvStr(" "))
		ChangeFontStyle();

	else if(DEBUG_RcvStr("`"))
		ReplaceLcdStrType();

	else if(DEBUG_RcvStr("r"))
		ChangeFontBoldItalNorm();

	else if(DEBUG_RcvStr("t"))
		Dec_offsWin();
	else if(DEBUG_RcvStr("y"))
		Inc_offsWin();

	else if(DEBUG_RcvStr("u"))
		Dec_lenWin();
	else if(DEBUG_RcvStr("i"))
		Inc_lenWin();
	else if(DEBUG_RcvStr("0"))
		DisplayFontsWithChangeColorOrNot();

	else if(DEBUG_RcvStr("]"))
		Inc_PosCursor();
	else if(DEBUG_RcvStr("["))
		Dec_PosCursor();
	else if(DEBUG_RcvStr("'"))
		IncDec_SpaceBetweenFont(0);
	else if(DEBUG_RcvStr("\\"))
		IncDec_SpaceBetweenFont(1);
	else if(DEBUG_RcvStr("/"))
		LCD_DisplayRemeberedSpacesBetweenFonts();
	else if(DEBUG_RcvStr("o"))
		LCD_WriteSpacesBetweenFontsOnSDcard();
	else if(DEBUG_RcvStr("m"))
		LCD_ResetSpacesBetweenFonts();
	else if(DEBUG_RcvStr("p"))
	{
		DbgVar(DEBUG_ON,100,Clr_ Mag_"\r\nStart: %s\r\n"_X,GET_CODE_FUNCTION);
		DisplayCoeffCalibration();

		Delete_TouchLcd_Task();
		if(BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize()))
			Dbg(1,"\r\nERROR: BSP_TS_Init ");
		Create_TouchLcd_Task();

	}
	else if(DEBUG_RcvStr("-"))
	{
		FILE_NAME(printInfo)();
	}

}




void FILE_NAME(main)(int argNmb, char **argVal)  //tu W **arcv PRZEKAZ TEXT !!!!!! dla fonts !!!
{
	char *ptr=NULL;   //zaokraglic selected text w biale roundRectange !!!!!

	SCREEN_ResetAllParameters();		//ROBIMY TU JUZ KLAWIATUTE i wprowadzanie textu dowolnego !!!!!
	LCD_TOUCH_DeleteAllSetTouch();  //Przyciski do zmian paranetru !!!!
	ResetRGB();
	LCD_Clear(v.COLOR_BkScreen);

//	 	touchTemp[0].y= LCD_GetYSize()/5;
//	 	touchTemp[1].y= LCD_GetYSize()-LCD_GetYSize()/5;
//	 	touchTemp[0].x= 500;
//	 	touchTemp[1].x= 750;
//	 	LCD_TOUCH_Set(ID_TOUCH_MOVE_DOWN,Move_4,press);
//
//	 	touchTemp[0].x= 0;
//	 	touchTemp[0].y= 300;
//	 	touchTemp[1].x= touchTemp[0].x+200;
//	 	touchTemp[1].y= touchTemp[0].y+180;
//	 	LCD_TOUCH_Set(ID_TOUCH_POINT,Point_12,pressRelease);


	v.FONT_ID_Title 	 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Title),	  	FILE_NAME(GetDefaultParam)(FONT_ID_Title));
	v.FONT_ID_FontColor		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontColor),	FILE_NAME(GetDefaultParam)(FONT_ID_FontColor));
	v.FONT_ID_BkColor 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(BkColor),  	FILE_NAME(GetDefaultParam)(FONT_ID_BkColor));
	v.FONT_ID_FontType 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontType), 	FILE_NAME(GetDefaultParam)(FONT_ID_FontType));
	v.FONT_ID_FontSize 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontSize), 	FILE_NAME(GetDefaultParam)(FONT_ID_FontSize));
	v.FONT_ID_FontStyle  	= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontStyle),	FILE_NAME(GetDefaultParam)(FONT_ID_FontStyle));

	v.FONT_ID_Coeff 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Coeff), 		 FILE_NAME(GetDefaultParam)(FONT_ID_Coeff));
	v.FONT_ID_LenWin 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(LenWin), 		 FILE_NAME(GetDefaultParam)(FONT_ID_LenWin));
	v.FONT_ID_OffsWin 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(OffsWin), 	 FILE_NAME(GetDefaultParam)(FONT_ID_OffsWin));
	v.FONT_ID_LoadFontTime 	= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(LoadFontTime),FILE_NAME(GetDefaultParam)(FONT_ID_LoadFontTime));
	v.FONT_ID_PosCursor 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(PosCursor), 	 FILE_NAME(GetDefaultParam)(FONT_ID_PosCursor));
	v.FONT_ID_CPUusage 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(CPUusage), 	 FILE_NAME(GetDefaultParam)(FONT_ID_CPUusage));
	v.FONT_ID_Speed 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Speed), 		 FILE_NAME(GetDefaultParam)(FONT_ID_Speed));
	v.FONT_ID_Press 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Press), 		 FILE_NAME(GetDefaultParam)(FONT_ID_Press));

	LCD_LoadFontVar();
	//FILE_NAME(printInfo)();

	LCD_DrawMainFrame(LCD_RoundRectangle,NoIndDisp,0, 0,0, LCD_X,140,SHAPE_PARAM(MainFrame,FillMainFrame,BkScreen)); // dlatego daj bk color MYGRAY aby zachowac kolory przy cieniowaniu !!!!!

	LCD_Keyboard_RGB(LCD_RoundRectangle,0, 550,160, 60,40, 10, SHAPE_PARAM(Frame,FillFrame,BkScreen),Point_6,All_Block);  //dac wyrownanie ADJUTMENT to LEFT RIGHT TOP .....




	ptr = GetSelTxt(0,FILE_NAME(Lang),0);
	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(Title, FillMainFrame),LCD_Xpos(lenStr,SetPos,600),LCD_Ypos(lenStr,SetPos,8), ptr,fullHight,0,255,NoConstWidth);

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontColor, FillMainFrame), LCD_Xpos(lenStr,SetPos,23), LCD_Ypos(lenStr,SetPos,8), TXT_FONT_COLOR, fullHight,0, 240,ConstWidth);  //zrobic mniejsza czcionka przeliczenie na hex !!!
	ConfigTouchForStrVar(ID_TOUCH_POINT, Point_1, press, v.FONT_VAR_FontColor, lenStr);

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(BkColor, FillMainFrame),  LCD_Xpos(lenStr,SetPos,23), LCD_Ypos(lenStr,IncPos,10), TXT_BK_COLOR,fullHight,0,	255,ConstWidth); //zrobic mniejsza czcionka przeliczenie na hex !!!
	ConfigTouchForStrVar(ID_TOUCH_POINT, Point_2, press, v.FONT_VAR_BkColor, lenStr);

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontType, FillMainFrame),  LCD_Xpos(lenStr,SetPos,23), LCD_Ypos(lenStr,IncPos,10), TXT_FONT_TYPE, 	fullHight,0,255,NoConstWidth);
	ConfigTouchForStrVar(ID_TOUCH_POINT, Point_3, press, v.FONT_VAR_FontType, lenStr);

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontSize, FillMainFrame),  LCD_Xpos(lenStr,IncPos,20), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_SIZE, 	fullHight,0,255,NoConstWidth);
	ConfigTouchForStrVar(ID_TOUCH_POINT, Point_4, press, v.FONT_VAR_FontSize, lenStr);

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontStyle, FillMainFrame), LCD_Xpos(lenStr,IncPos,80), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_STYLE, 	fullHight,0,255,NoConstWidth);
	ConfigTouchForStrVar(ID_TOUCH_POINT, Point_5, press, v.FONT_VAR_FontStyle, lenStr);




	LCD_StrDependOnColorsVar(STR_FONT_PARAM(Coeff,FillMainFrame),200, 20, TXT_COEFF,  		 	fullHight,0,255,ConstWidth);  //DESCRiption malymi szarymi literkami !!!! oddzoelone kreseczkami !!!
	LCD_StrDependOnColorsVar(STR_FONT_PARAM(LenWin,FillMainFrame),400,  0, TXT_LEN_WIN,		 	 halfHight,0,255,ConstWidth);
	LCD_StrDependOnColorsVar(STR_FONT_PARAM(OffsWin,FillMainFrame),400, 20, TXT_OFFS_WIN,	    	 halfHight,0,255,ConstWidth);
	LCD_StrDependOnColorsVar(STR_FONT_PARAM(LoadFontTime,FillMainFrame),320, 20, TXT_LOAD_FONT_TIME,	 halfHight,0,255,ConstWidth);
	LCD_StrDependOnColorsDescrVar(STR_FONT_PARAM(PosCursor,FillMainFrame),440, 40, "T: ",TXT_PosCursor(),halfHight,0,255,ConstWidth);
	LCD_StrDependOnColorsVar(STR_FONT_PARAM(CPUusage,FillMainFrame),450, 0, 	TXT_CPU_USAGE,	 		 halfHight,0,255,ConstWidth);

	 Test.yFontsField=LCD_Ypos(lenStr,IncPos,5);
	 LCD_Ymiddle(SetPos, Test.yFontsField|(LCD_GetYSize()-2)<<16 );
	 LCD_Xmiddle(SetPos, Test.xFontsField|LCD_GetXSize()<<16,"",0,NoConstWidth);

	StartMeasureTime_us();    //daj mozliwosc wpisywania dowolnego textu aby korygowac odstepy miedzy kazdymi fontami jakimi sie chce !!!!!!!
	if(Test.type)
		lenStr=LCD_StrVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,v.COLOR_BkScreen,0,Test.constWidth,v.COLOR_BkScreen);
	else
		lenStr=LCD_StrChangeColorVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,RGB_BK,RGB_FONT,Test.coeff,Test.constWidth,v.COLOR_BkScreen);
	Test.speed=StopMeasureTime_us("");  //dla FONTS trzeba odstepy spacji po lewekj i prawej stronie aby lepiej wygladaly bk

	LCD_StrDependOnColorsVar(STR_FONT_PARAM(Speed, FillMainFrame),450,0,TXT_SPEED,fullHight,0,255,ConstWidth);

	LCD_Show();

	DbgVar(DEBUG_ON,100,Clr_ Mag_"\r\nStart: %s\r\n"_X,GET_CODE_FUNCTION);






	LCD_SetStrVar_fontID		(v.FONT_VAR_FontStyle, v.FONT_ID_Press);
	LCD_SetStrVar_fontColor	(v.FONT_VAR_FontStyle, v.FONT_COLOR_Press);
	LCD_SetStrVar_bkColor	(v.FONT_VAR_FontStyle, v.FONT_BKCOLOR_Press);
	LCD_SetStrVar_bkScreenColor(v.FONT_VAR_FontStyle, v.FONT_BKCOLOR_Press);

	LCD_FrameForStr(v.FONT_VAR_FontStyle, v.FONT_BKCOLOR_FontStyle, "123456789");
	LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontStyle, "123456789");

	LCD_SetStrVar_fontID		(v.FONT_VAR_FontStyle, v.FONT_ID_FontStyle);
	LCD_SetStrVar_fontColor	(v.FONT_VAR_FontStyle, v.FONT_COLOR_FontStyle);
	LCD_SetStrVar_bkColor	(v.FONT_VAR_FontStyle, v.FONT_BKCOLOR_FontStyle);
	LCD_SetStrVar_bkScreenColor(v.FONT_VAR_FontStyle, v.COLOR_FillMainFrame);

}
