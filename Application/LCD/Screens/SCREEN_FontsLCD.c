
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
Zmiana kolorow czcionki,Press to change color fonts,\
Pierw:,First:,\
Czer,Red,\
Ziel,Green,\
Nieb,Blue,\
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
	X(13, FONT_SIZE_Descr, 	 		FONT_11_bold) \
	X(14, FONT_SIZE_Press, 	 		FONT_14_bold) \
	X(15, FONT_SIZE_Fonts,			FONT_20) \
	\
	X(16, FONT_STYLE_Title, 	 	Arial) \
	X(17, FONT_STYLE_FontColor, 	Arial) \
	X(18, FONT_STYLE_BkColor, 		Arial) \
	X(19, FONT_STYLE_FontType, 	Arial) \
	X(20, FONT_STYLE_FontSize, 	Arial) \
	X(21, FONT_STYLE_FontStyle, 	Arial) \
	X(22, FONT_STYLE_Coeff, 		Arial) \
	X(23, FONT_STYLE_LenWin, 		Arial) \
	X(24, FONT_STYLE_OffsWin, 		Arial) \
	X(25, FONT_STYLE_LoadFontTime,Arial) \
	X(26, FONT_STYLE_PosCursor,	Arial) \
	X(27, FONT_STYLE_CPUusage,		Arial) \
	X(28, FONT_STYLE_Speed,			Arial) \
	X(29, FONT_STYLE_Descr, 		Times_New_Roman) \
	X(30, FONT_STYLE_Press, 		Arial) \
	X(31, FONT_STYLE_Fonts, 		Arial) \
	\
	X(32, FONT_COLOR_Title,  	 	WHITE) \
	X(33, FONT_COLOR_FontColor, 	WHITE) \
	X(34, FONT_COLOR_BkColor, 	 	WHITE) \
	X(35, FONT_COLOR_FontType,  	WHITE) \
	X(36, FONT_COLOR_FontSize,  	WHITE) \
	X(37, FONT_COLOR_FontStyle,  	WHITE) \
	X(38, FONT_COLOR_Coeff,  		WHITE) \
	X(39, FONT_COLOR_LenWin,  		WHITE) \
	X(40, FONT_COLOR_OffsWin,  	WHITE) \
	X(41, FONT_COLOR_LoadFontTime,WHITE) \
	X(42, FONT_COLOR_PosCursor,	WHITE) \
	X(43, FONT_COLOR_CPUusage,		WHITE) \
	X(44, FONT_COLOR_Speed,			WHITE) \
	X(45, FONT_COLOR_Descr, 		COLOR_GRAY(0xB0)) \
	X(46, FONT_COLOR_Press, 		DARKRED) \
	X(47, FONT_COLOR_Fonts,  		0xFFE1A000) \
	\
	X(48, FONT_BKCOLOR_Title,  	 	MYGRAY) \
	X(49, FONT_BKCOLOR_FontColor, 	MYGRAY) \
	X(50, FONT_BKCOLOR_BkColor, 	 	MYGRAY) \
	X(51, FONT_BKCOLOR_FontType,  	MYGRAY) \
	X(52, FONT_BKCOLOR_FontSize,  	MYGRAY) \
	X(53, FONT_BKCOLOR_FontStyle,  	MYGRAY) \
	X(54, FONT_BKCOLOR_Coeff,  		MYGRAY) \
	X(55, FONT_BKCOLOR_LenWin,  		MYGRAY) \
	X(56, FONT_BKCOLOR_OffsWin,  		MYGRAY) \
	X(57, FONT_BKCOLOR_LoadFontTime,	MYGRAY) \
	X(58, FONT_BKCOLOR_PosCursor,		MYGRAY) \
	X(59, FONT_BKCOLOR_CPUusage,		MYGRAY) \
	X(60, FONT_BKCOLOR_Speed,			MYGRAY) \
	X(61, FONT_BKCOLOR_Descr, 			MYGRAY) \
	X(62, FONT_BKCOLOR_Press, 			WHITE) \
	X(63, FONT_BKCOLOR_Fonts,  		0x090440) \
	\
	X(64, COLOR_BkScreen,  			COLOR_GRAY(0x38)) \
	X(65, COLOR_MainFrame,  		COLOR_GRAY(0xDA)) \
	X(66, COLOR_FillMainFrame, 	MYGRAY) \
	X(67, COLOR_Frame,  				COLOR_GRAY(0x60)) \
	X(68, COLOR_FillFrame, 			COLOR_GRAY(0x2C)) \
	X(69, COLOR_FramePress, 		COLOR_GRAY(0xBA)) \
	X(70, COLOR_FillFramePress,	COLOR_GRAY(0x60)) \
	X(71, DEBUG_ON,  	1) \
	X(72, BK_FONT_ROUND,  	1) \
	\
	X(73, FONT_ID_Title,			fontID_1) \
	X(74, FONT_ID_FontColor,	fontID_2) \
	X(75, FONT_ID_BkColor, 		fontID_3) \
	X(76, FONT_ID_FontType, 	fontID_4) \
	X(77, FONT_ID_FontSize, 	fontID_5) \
	X(78, FONT_ID_FontStyle,  	fontID_6) \
	X(79, FONT_ID_Coeff,  		fontID_7) \
	X(80, FONT_ID_LenWin,  		fontID_8) \
	X(81, FONT_ID_OffsWin,  	fontID_9) \
	X(82, FONT_ID_LoadFontTime,fontID_10) \
	X(83, FONT_ID_PosCursor,	fontID_11) \
	X(84, FONT_ID_CPUusage,		fontID_12) \
	X(85, FONT_ID_Speed,			fontID_13) \
	X(86, FONT_ID_Descr,  		fontID_14) \
	X(87, FONT_ID_Press,  		fontID_15) \
	X(88, FONT_ID_Fonts,  		fontID_16) \
	\
	X(89, FONT_VAR_Title,			fontVar_1) \
	X(90, FONT_VAR_FontColor,		fontVar_2) \
	X(91, FONT_VAR_BkColor, 		fontVar_3) \
	X(92, FONT_VAR_FontType, 		fontVar_4) \
	X(93, FONT_VAR_FontSize, 		fontVar_5) \
	X(94, FONT_VAR_FontStyle,  	fontVar_6) \
	X(95, FONT_VAR_Coeff,  			fontVar_7) \
	X(96, FONT_VAR_LenWin,  		fontVar_8) \
	X(97, FONT_VAR_OffsWin,  		fontVar_9) \
	X(98, FONT_VAR_LoadFontTime,	fontVar_10) \
	X(99, FONT_VAR_PosCursor,		fontVar_11) \
	X(100, FONT_VAR_CPUusage,		fontVar_12) \
	X(101, FONT_VAR_Speed,			fontVar_13) \
	X(102, FONT_VAR_Fonts,  			fontVar_14) \
	X(103, FONT_VAR_Press,  			fontVar_15) \
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

#define SEL_BITS_SIZE	5
static uint64_t FILE_NAME(SelBits)[5] = {0};
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
		#define X(a,b,c) DbgVar2(1,200,CoGr_"%*d"_X	"%*s" 	CoGr_"= "_X	 	"%*s" 	"(%s0x%x)\r\n",-4,a,		-23,getName(b),	-15,getName(c), 	CHECK_bit( FILE_NAME(SelBits)[a/64], (a-64*(a/64)) )?CoR_"change to: "_X:"", v.b);
			SCREEN_FONTS_SET_PARAMETERS
		#undef X
		DbgVar(1,200,CoG2_"}%s;\r\n"_X,getName(FILE_NAME(struct)));
	}
}
//ZROBIC szablon z macro by dla kazdego pliku szybko skopioowac !!!!!!!!!!!!!!!
int FILE_NAME(funcGet)(int offs){
	return *( (int*)((int*)(&v) + offs) );
}

void FILE_NAME(funcSet)(int offs, int val){
	*( (int*)((int*)(&v) + offs) ) = val;
	SET_bit( FILE_NAME(SelBits)[offs/64], (offs-64*(offs/64)) );
}

void FILE_NAME(setDefaultAllParam)(int rst){
	#define X(a,b,c) FILE_NAME(funcSet)(b,c);
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
	if(rst){
		for(int i=0;i<SEL_BITS_SIZE;++i)
			FILE_NAME(SelBits)[i]=0;
	}
}

void FILE_NAME(debugRcvStr)(void);
void FILE_NAME(setTouch)(void);

void 	FILE_NAME(main)(int argNmb, char **argVal);
/*------------ End Main Screen MACRO -----------------*/

#define TEXT_TO_SHOW		" Rafa"ł" Markielowski "

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

#define CHECK_TOUCH(state)			CHECK_bit(FILE_NAME(SelTouch),state-1)
#define SET_TOUCH(state) 			SET_bit	(FILE_NAME(SelTouch),state-1)
#define CLR_TOUCH(state) 			RST_bit	(FILE_NAME(SelTouch),state-1)
#define CLR_ALL_TOUCH 				FILE_NAME(SelTouch) = 0
#define GET_TOUCH 					FILE_NAME(SelTouch)

#define NONE_TYPE_REQ	-1

typedef enum{
	NoTouch,//UWAZJA na MAX_OPEN_TOUCH_SIMULTANEOUSLY!!!! zeby nie przkroczyc !!!!  trzeba te same dla roznych punktow !!!!
	Touch_FontColor,
	Touch_BkColor,
	Touch_FontType,
	Touch_FontType2,
	Touch_FontSize,
	Touch_FontSize2,
	Touch_FontStyle,
	Touch_FontStyle2,
	Touch_FontCoeff,
	Touch_FontCoeff2,
	Touch_fontRp,
	Touch_fontGp,
	Touch_fontBp,
	Touch_fontRm,
	Touch_fontGm,
	Touch_fontBm,
	Touch_bkRp,
	Touch_bkGp,
	Touch_bkBp,
	Touch_bkRm,
	Touch_bkGm,
	Touch_bkBm,
	Touch_style1,
	Touch_style2,
	Touch_style3,
	Touch_type1,
	Touch_type2,
	Touch_type3,
	Touch_size_plus,
	Touch_size_minus,
	Touch_size_norm,
	Touch_size_bold,
	Touch_size_italic,
	Touch_coeff1,
	Touch_coeff2,
	Touch_coeff3,
	Touch_coeff4,
	Move_1,
	Move_2,
	Move_3,
	Move_4,
	Point_1,
	AnyPress,
	AnyPressWithWait
}TOUCH_POINTS;

typedef enum{
	KEYBOARD_none,
	KEYBOARD_fontRGB,
	KEYBOARD_bkRGB,
	KEYBOARD_fontSize,
	KEYBOARD_fontType,
	KEYBOARD_fontStyle,
	KEYBOARD_fontCoeff,
}KEYBOARD_TYPES;

typedef enum{
	KEY_All_release,
	KEY_Select_one,
	KEY_All_release_and_select_one,

	KEY_Red_plus,
	KEY_Green_plus,
	KEY_Blue_plus,
	KEY_Red_minus,
	KEY_Green_minus,
	KEY_Blue_minus,

	KEY_Style_1,
	KEY_Style_2,
	KEY_Style_3,

	KEY_Size_plus,
	KEY_Size_minus,
	KEY_Size_norm,
	KEY_Size_bold,
	KEY_Size_italic
}SELECT_PRESS_BLOCK;

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

static char bufTemp[50];
static int lenTxt_prev;
static StructTxtPxlLen lenStr;

typedef struct{
	int32_t bk[3];
	int32_t font[3];
	uint16_t xFontsField;
	uint16_t yFontsField;
	int8_t step;
	int16_t coeff;
	int16_t coeff_prev;
	int8_t size;
	uint8_t style;
	uint32_t time;
	int8_t type;
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
	int usun; //!!!!!!!!!!! USUN
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

static void Data2Refresh(int nr)
{
	switch(nr)
	{
	case PARAM_TYPE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontType,TXT_FONT_TYPE);
		SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontType,0, lenStr);
		break;
	case PARAM_SIZE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontSize,TXT_FONT_SIZE);
		SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontSize,0, lenStr);
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
		SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontStyle,0, lenStr);
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

static void FONTS_LCD_ResetParam(void)
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
	LCD_SetStrVar_x(v.FONT_VAR_Fonts,LCD_Xmiddle(0,GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth));
}
static void AdjustMiddle_Y(void){
	LCD_SetStrVar_y(v.FONT_VAR_Fonts,LCD_Ymiddle(0,GetPos,v.FONT_ID_Fonts));
}

static void ChangeFontStyle(int8_t typeReq)
{
	if(typeReq > NONE_TYPE_REQ)
	{
		if(Test.style == typeReq)
			return;
		else
			Test.style = typeReq;
	}
	else
	{
		switch(Test.style)
		{
		case Arial:   			 Test.style=Times_New_Roman; break;
		case Times_New_Roman: Test.style=Comic_Saens_MS;  break;
		case Comic_Saens_MS:  Test.style=Arial; 			  break;
		default:              Test.style=Arial;           break;
		}
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

static void ChangeFontBoldItalNorm(int8_t typeReq)
{
	if(typeReq > NONE_TYPE_REQ)
	{
		if(Test.normBoldItal == typeReq)
			return;

		if(typeReq > Test.normBoldItal)
			Test.size += (typeReq-Test.normBoldItal);
		else if(typeReq < Test.normBoldItal)
			Test.size -= (Test.normBoldItal-typeReq);

		Test.normBoldItal = typeReq;
	}
	else
	{
		if(Test.normBoldItal>1){
			Test.normBoldItal=0;
			Test.size-=2;
		}
		else{
			Test.normBoldItal++;
			Test.size++;
		}
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

static void ReplaceLcdStrType(int8_t typeReq)
{
	if(Test.type == typeReq)
		return;

	GOTO_ReplaceLcdStrType:
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

	if(typeReq > NONE_TYPE_REQ){
		if(typeReq!=Test.type)
			goto GOTO_ReplaceLcdStrType;
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
//dla DEBUG ON wlaczyc i wylaczyc caly debug dla danego pliku !!!! uregulowac to !!!
static void DisplayFontsWithChangeColorOrNot(void){
	TOOGLE(Test.dispChangeColorOrNot);
	RefreshAllParam();
}

static void LCD_DrawMainFrame(figureShape shape, int directDisplay, uint8_t bold, uint16_t x,uint16_t y, uint16_t w,uint16_t h, int frameColor,int fillColor,int bkColor)// zastanowic czy nie dac to do BasicGraphic.c
{
	figureShape pShape[5] = {LCD_Rectangle, LCD_BoldRectangle, LCD_RoundRectangle, LCD_BoldRoundRectangle, LCD_LittleRoundRectangle};

	if(shape==pShape[1] || shape==pShape[3])
		frameColor = SetColorBoldFrame(frameColor,bold);

	if(shape==pShape[2] || shape==pShape[3])
		Set_AACoeff_RoundFrameRectangle(0.55, 0.73);

	if(IndDisp==directDisplay)
		LCD_ShapeIndirect(x,y,shape,w,h,frameColor,fillColor,bkColor);
	else
		LCD_Shape(x,y,shape,w,h,frameColor,fillColor,bkColor);
}

#define KEYBOARD_TYPE(type,key)	KeyboardTypeDisplay(type,key,0,0,0,0,0,0,0,NoTouch,NoTouch)

int KeyboardTypeDisplay(KEYBOARD_TYPES type, SELECT_PRESS_BLOCK selBlockPress, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx)
{//Rozmiar klawiatury dostosowany do wielkosci fontu !!!!!!!!!!!!
	int fontID 			= v.FONT_ID_Press;
	int frameColor 	= v.COLOR_Frame;
	int fillColor 		= v.COLOR_FillFrame;
	int framePressColor 	= v.COLOR_FramePress;
	int fillPressColor 	= v.COLOR_FillFramePress;
	int bkColor 		= v.COLOR_BkScreen;

	#define MIDDLE_NR		1
	#define GET_X(txt)	LCD_Xmiddle(MIDDLE_NR,GetPos,fontID,txt,0,NoConstWidth)
	#define GET_Y			LCD_Ymiddle(MIDDLE_NR,GetPos,fontID)

	uint16_t widthAll = 0;
	uint16_t heightAll = 0;

	static struct SETTINGS{
		figureShape shape;
		uint8_t bold;
		uint16_t x;
		uint16_t y;
		uint16_t widthKey;
		uint16_t heightKey;
		uint8_t interSpace;
		uint8_t forTouchIdx;
		uint8_t startTouchIdx;
		uint8_t nmbTouch;
	}s={0}, c={0};

	void _Str(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrPress(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _StrDisp(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindowIndirect(0, s.x, s.y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrPressDisp(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindowIndirect(0, s.x, s.y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _TxtPos(XY_Touch_Struct pos){
		LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s.widthKey),NULL,0,NoConstWidth);
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s.heightKey));
	}
	void _Key(XY_Touch_Struct pos){
		LCD_ShapeWindow( s.shape, 0, widthAll,heightAll, pos.x,pos.y, s.widthKey,s.heightKey, SetColorBoldFrame(frameColor,s.bold),fillColor,bkColor);
	}
	void _KeyStr(XY_Touch_Struct pos,const char *txt, uint32_t color){
		_Key(pos);
		_TxtPos(pos);
		_Str(txt,color);
	}
	void _KeyStrDisp(XY_Touch_Struct pos,const char *txt, uint32_t color){
		_Key(pos);
		_TxtPos(pos);
		_StrDisp(txt,color);
	}
	void _KeyPress(XY_Touch_Struct pos){
		LCD_ShapeWindow( s.shape, 0, widthAll,heightAll, pos.x,pos.y, s.widthKey,s.heightKey, SetColorBoldFrame(framePressColor,s.bold),fillPressColor,bkColor);
	}
	void _KeyStrPress(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_TxtPos(pos);
		_StrPress(txt,colorTxt);
	}
	void _KeyStrPressDisp(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_TxtPos(pos);
		_StrPressDisp(txt,colorTxt);
	}
	void _KeyStrPressDisp_oneBlock(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		LCD_ShapeWindow( s.shape, 0, s.widthKey,s.heightKey, 0,0, s.widthKey,s.heightKey, SetColorBoldFrame(framePressColor,s.bold),fillPressColor,bkColor);
		_TxtPos((XY_Touch_Struct){0});
		LCD_StrDependOnColorsWindowIndirect(0, s.x+pos.x, s.y+pos.y, s.widthKey, s.heightKey,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, colorTxt,255, NoConstWidth);
	}
	void _SetTouch(uint16_t ID, uint16_t idx, uint8_t paramTouch, XY_Touch_Struct pos){
		touchTemp[0].x= s.x + pos.x;
		touchTemp[1].x= touchTemp[0].x + s.widthKey;
		touchTemp[0].y= s.y + pos.y;
		touchTemp[1].y= touchTemp[0].y + s.heightKey;
		LCD_TOUCH_Set(ID,idx,paramTouch);
		s.nmbTouch++;
	}
	void _deleteTouchs(void){
		for(int i=0; i<s.nmbTouch; ++i)
			LCD_TOUCH_DeleteSelectTouch(s.startTouchIdx+i);
	}
	void _exit(void){
		s.forTouchIdx = NoTouch;
		s.nmbTouch = 0;  // to samo jak s=0; !!!!
	}
	int _startUp(void){
		if(KEYBOARD_none == type){
			_deleteTouchs();
			_exit();
			return 1;
		}
		if(shape!=0){
			_deleteTouchs();
			if(s.forTouchIdx == forTouchIdx){
				_exit();
				return 1;
			}
			s.shape = shape;
			s.bold = bold;
			s.x = x;
			s.y = y;
			s.widthKey = widthKey;
			s.heightKey = heightKey;
			s.interSpace = interSpace;
			s.forTouchIdx = forTouchIdx;
			s.startTouchIdx = startTouchIdx;
			s.nmbTouch = 0;
		}
		return 0;
	}

	if(_startUp()) return 1;

	/* ----- User Function Definitions ----- */
	void _ServiceRGB(void)
	{
		const char *txtKey[]								= {"R+",	  "G+", 	  "B+",		 "R-", 	    "G-",	  "B-"};
		const COLORS_DEFINITION colorTxtKey[]		= {RED,	  GREEN, 	BLUE,		 RED, 	    GREEN,	 BLUE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,DARKRED, LIGHTGREEN,LIGHTGREEN, DARKBLUE,DARKBLUE};
		const uint16_t dimKeys[] = {3,2};
		XY_Touch_Struct posKey[]=
		  {{1*s.interSpace + 0*s.widthKey,	1*s.interSpace + 0*s.heightKey},
			{2*s.interSpace + 1*s.widthKey, 	1*s.interSpace + 0*s.heightKey},
			{3*s.interSpace + 2*s.widthKey, 	1*s.interSpace + 0*s.heightKey},
			\
			{1*s.interSpace + 0*s.widthKey,	2*s.interSpace + 1*s.heightKey},
			{2*s.interSpace + 1*s.widthKey, 	2*s.interSpace + 1*s.heightKey},
			{3*s.interSpace + 2*s.widthKey, 	2*s.interSpace + 1*s.heightKey}};

		int countKey = dimKeys[0]*dimKeys[1]; 		/* = STRUCT_TAB_SIZE(txtKey); */
		widthAll =  dimKeys[0]*s.widthKey +  (dimKeys[0]+1)*s.interSpace;
		heightAll = dimKeys[1]*s.heightKey + (dimKeys[1]+1)*s.interSpace;

		switch((int)selBlockPress)
		{
			case KEY_All_release:
				LCD_ShapeWindow( s.shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s.bold), bkColor,bkColor );

				for(int i=0; i<countKey; ++i){
					i<countKey-1 ? _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(posKey[i],txtKey[i],colorTxtKey[i]);
				/*	_Key(posKey[i]);
					_TxtPos(posKey[i]);		i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
				}
				break;

			case KEY_Red_plus:	 _KeyStrPressDisp_oneBlock(posKey[0], txtKey[0], colorTxtPressKey[0]);	break;	/* Wykorzystac to dla przesuwu dotyku i pozostawienie sladu :) */
			case KEY_Green_plus:	 _KeyStrPressDisp_oneBlock(posKey[1], txtKey[1], colorTxtPressKey[1]);	break;
			case KEY_Blue_plus:	 _KeyStrPressDisp_oneBlock(posKey[2], txtKey[2], colorTxtPressKey[2]);	break;
			case KEY_Red_minus: 	 _KeyStrPressDisp_oneBlock(posKey[3], txtKey[3], colorTxtPressKey[3]);	break;
			case KEY_Green_minus: _KeyStrPressDisp_oneBlock(posKey[4], txtKey[4], colorTxtPressKey[4]);	break;
			case KEY_Blue_minus:  _KeyStrPressDisp_oneBlock(posKey[5], txtKey[5], colorTxtPressKey[5]);	break;
		}

		if(startTouchIdx){
			for(int i=0; i<countKey; ++i)
				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s.startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
		}
	}

	void _ServiceStyle(void)  //zrobic te tyhpy Keybordow jako general schematy !!!!
	{
		const char *txtKey[]								= {"Arial", "Times_New_Roman", "Comic_Saens_MS"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,	  WHITE, 	  			WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,	  DARKRED, 				DARKBLUE};
		const uint16_t dimKeys[] = {1,3};

		XY_Touch_Struct posKey[]=
		  {{s.interSpace, 1*s.interSpace + 0*s.heightKey - 0},
			{s.interSpace, 2*s.interSpace + 1*s.heightKey - 1},
			{s.interSpace, 3*s.interSpace + 2*s.heightKey - 2}};

		int countKey = STRUCT_TAB_SIZE(txtKey);
		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s.widthKey  + (dimKeys[0]+1)*s.interSpace;
		heightAll = dimKeys[1]*s.heightKey + (dimKeys[1]+1)*s.interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				LCD_ShapeWindow( s.shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s.bold), bkColor,bkColor );

				for(int i=0; i<countKey; ++i)
				{
					if((i==0 && Test.style==Arial) ||
						(i==1 && Test.style==Times_New_Roman) ||
						(i==2 && Test.style==Comic_Saens_MS))
					{
							i<countKey-1 ? _KeyStrPress(posKey[i],txtKey[i],colorTxtPressKey[i]) : _KeyStrPressDisp(posKey[i],txtKey[i],colorTxtPressKey[i]);
						/* _KeyPress(posKey[i]); _TxtPos(posKey[i]);    i<countKey-1 ? _StrPress(txtKey[i],colorTxtPressKey[i]) : _StrPressDisp(txtKey[i],colorTxtPressKey[i]); */		/* This is the same as up */
					}
					else{	i<countKey-1 ? _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(posKey[i],txtKey[i],colorTxtKey[i]);
						/* _Key(posKey[i]); _TxtPos(posKey[i]);   i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
					}
				}
				break;
		}

		if(startTouchIdx){
			for(int i=0; i<countKey; ++i)
				_SetTouch(ID_TOUCH_POINT, s.startTouchIdx + i, press, posKey[i]);
		}
	}

	void _ServiceType(void)
	{
		const char *txtKey[]								= {"(Gray-Green)", "(Gray-White)", "(White-Black)"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,	  		 WHITE, 	  			WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {BLACK,	  		 BROWN, 				ORANGE};
		const uint16_t dimKeys[] = {1,3};

		XY_Touch_Struct posKey[]=
		  {{s.interSpace, 1*s.interSpace + 0*s.heightKey - 0},
			{s.interSpace, 2*s.interSpace + 1*s.heightKey - 1},
			{s.interSpace, 3*s.interSpace + 2*s.heightKey - 2}};

		int countKey = STRUCT_TAB_SIZE(txtKey);

		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s.widthKey  + (dimKeys[0]+1)*s.interSpace;
		heightAll = dimKeys[1]*s.heightKey + (dimKeys[1]+1)*s.interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				LCD_ShapeWindow( s.shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s.bold), bkColor,bkColor );

				for(int i=0; i<countKey; ++i)
				{
					if((i==0 && Test.type==0) ||
						(i==1 && Test.type==1) ||
						(i==2 && Test.type==2))
					{
							i<countKey-1 ? _KeyStrPress(posKey[i],txtKey[i],colorTxtPressKey[i]) : _KeyStrPressDisp(posKey[i],txtKey[i],colorTxtPressKey[i]);
					}
					else{	i<countKey-1 ? _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(posKey[i],txtKey[i],colorTxtKey[i]);
					}
				}
				break;
		}

		if(startTouchIdx){
			for(int i=0; i<countKey; ++i)
				_SetTouch(ID_TOUCH_POINT, s.startTouchIdx + i, press, posKey[i]);
		}
	}

	void _ServiceSize(void)
	{
		const char *txtKey[]								= {"Size +",	"Size -"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,	  	WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,		BLACK};
		const uint16_t dimKeys[] = {1,2};
		XY_Touch_Struct posKey[]=
		  {{1*s.interSpace + 0*s.widthKey,	1*s.interSpace + 0*s.heightKey},
			{1*s.interSpace + 0*s.widthKey,	2*s.interSpace + 1*s.heightKey}};

		const char *txtKey2[]								= {"Normal", "Bold", "Italic"};
		const COLORS_DEFINITION colorTxtKey2[]			= {WHITE,	  WHITE,  WHITE};
		const COLORS_DEFINITION colorTxtPressKey2[]	= {BLACK,	  BROWN,  ORANGE};
		const uint16_t dimKeys2[] = {1,3};
		int ofs=s.interSpace + s.widthKey;
		XY_Touch_Struct posKey2[]=
		  {{s.interSpace+ofs, 1*s.interSpace + 0*s.heightKey - 0},
			{s.interSpace+ofs, 1*s.interSpace + 1*s.heightKey - 1},
			{s.interSpace+ofs, 1*s.interSpace + 2*s.heightKey - 2}};

		int countKey = dimKeys[0]*dimKeys[1];
		int countKey2 = dimKeys2[0]*dimKeys2[1];

		widthAll =  (s.interSpace + s.widthKey + s.interSpace) + (s.widthKey + s.interSpace);
		heightAll = s.interSpace + dimKeys2[1]*s.heightKey + s.interSpace - (countKey2-1);

		switch((int)selBlockPress)
		{
			case KEY_All_release_and_select_one:
				LCD_ShapeWindow( s.shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s.bold), bkColor,bkColor );

				for(int i=0; i<countKey; ++i)
					_KeyStr(posKey[i],txtKey[i],colorTxtKey[i]);

				c.shape = s.shape;	s.shape = LCD_Rectangle;
				framePressColor = frameColor;

				for(int i=0; i<countKey2; ++i)
				{
					if((i==0 && Test.normBoldItal==0) ||
						(i==1 && Test.normBoldItal==1) ||
						(i==2 && Test.normBoldItal==2))
					{
							i<countKey2-1 ? _KeyStrPress(posKey2[i],txtKey2[i],colorTxtPressKey2[i]) : _KeyStrPressDisp(posKey2[i],txtKey2[i],colorTxtPressKey2[i]);
					}
					else{	i<countKey2-1 ? _KeyStr(posKey2[i],txtKey2[i],colorTxtKey2[i]) : _KeyStrDisp(posKey2[i],txtKey2[i],colorTxtKey2[i]);
					}
				}
				s.shape = c.shape;
				break;

			case KEY_Size_plus:	 _KeyStrPressDisp_oneBlock(posKey[0], txtKey[0], colorTxtPressKey[0]);	break;
			case KEY_Size_minus:	 _KeyStrPressDisp_oneBlock(posKey[1], txtKey[1], colorTxtPressKey[1]);	break;
		}

		if(startTouchIdx){
			int touch_it=0;
			for(int i=0; i<countKey; ++i)
				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s.startTouchIdx + touch_it++, TOUCH_GET_PER_X_PROBE, posKey[i]);

			for(int i=0; i<countKey2; ++i)
				_SetTouch(ID_TOUCH_POINT, s.startTouchIdx + touch_it++, press, posKey2[i]);
		}
	}
/*
	void _ServiceScrollSel_Example(void)
	{
		const char *txtKey[]								= {"11","22","33","44","55","66","77","88","99"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKBLUE,DARKGREEN,DARKCYAN,DARKMAGENTA,DARKYELLOW,LIGHTGRAY,LIGHTCYAN,MAGENTA,CYAN};
		const uint16_t dimKeys[] = {1,9};
		int frameNmbVis = 4;
		uint16_t roll = 0;
		uint16_t selFrame = 6;

		int countKey = STRUCT_TAB_SIZE(txtKey);
		int win = frameNmbVis * s.heightKey - (frameNmbVis-1);

		if(shape!=0)
			LCD_TOUCH_ScrollSel_SetCalculate(0, &roll, &selFrame, 0,0,0,0);

		XY_Touch_Struct posKey[]=
		  {{s.interSpace, 1*s.interSpace + 0*s.heightKey - 0},
			{s.interSpace, 2*s.interSpace + 1*s.heightKey - 1},
			{s.interSpace, 3*s.interSpace + 2*s.heightKey - 2},
			{s.interSpace, 4*s.interSpace + 3*s.heightKey - 3},
			{s.interSpace, 5*s.interSpace + 4*s.heightKey - 4},
			{s.interSpace, 6*s.interSpace + 5*s.heightKey - 5},
			{s.interSpace, 7*s.interSpace + 6*s.heightKey - 6},
			{s.interSpace, 8*s.interSpace + 7*s.heightKey - 7},
			{s.interSpace, 9*s.interSpace + 8*s.heightKey - 8}};

		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s.widthKey  + (dimKeys[0]+1)*s.interSpace;
		heightAll = dimKeys[1]*s.heightKey + (dimKeys[1]+1)*s.interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				LCD_ShapeWindow( s.shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s.bold), bkColor,bkColor );

				if(shape==0)
					LCD_TOUCH_ScrollSel_SetCalculate(0, &roll, &selFrame, s.y, heightAll, s.heightKey, win);

				for(int i=0; i<countKey; ++i)
				{
					if(i == selFrame)
						_KeyStrPress(posKey[i],txtKey[i],colorTxtPressKey[i]);
					else
						_KeyStr(posKey[i],txtKey[i],colorTxtKey[i]);

				}
				LCD_Display(0 + roll * widthAll, s.x, s.y, widthAll, win);
				break;
		}

		if(startTouchIdx){
			touchTemp[0].x= s.x + posKey[0].x;
			touchTemp[1].x= touchTemp[0].x + s.widthKey;
			touchTemp[0].y= s.y + posKey[0].y;
			touchTemp[1].y= touchTemp[0].y + win;
			LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT, s.startTouchIdx, TOUCH_GET_PER_ANY_PROBE);
			s.nmbTouch++;
		}
	}
*/
	void _ServiceSizeRoll(void)  //Zrobic strzalki i press je powoduje automatyczne w dol albo w gore !!!!!
	{
		const uint16_t dimKeys[] = {1,LCD_GetFontSizeMaxNmb()};
		const char *txtKey[dimKeys[1]];
		COLORS_DEFINITION colorTxtKey[dimKeys[1]];
		COLORS_DEFINITION colorTxtPressKey = DARKRED;
		XY_Touch_Struct posKey[dimKeys[1]];

		int frameNmbVis = 10;
		uint16_t roll = 0;
		uint16_t selFrame = 17;

		int countKey = dimKeys[1];
		int win = frameNmbVis * s.heightKey - (frameNmbVis-1);
		uint16_t offsX = 0;//(s.interSpace + s.widthKey + s.interSpace) + (s.widthKey + s.interSpace) + s.interSpace;

		if(shape!=0)
			LCD_TOUCH_ScrollSel_SetCalculate(7, &roll, &selFrame, 0,0,0,0);

		for(int i=0; i<countKey; ++i)
		{
			txtKey[i] = LCD_GetFontSizeStr(i);
			colorTxtKey[i] = WHITE;

			posKey[i].x = s.interSpace;
			posKey[i].y = (i+1)*s.interSpace + i*s.heightKey - i;
		}

		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s.widthKey  + (dimKeys[0]+1)*s.interSpace;
		heightAll = dimKeys[1]*s.heightKey + (dimKeys[1]+1)*s.interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				//if(shape==0)
					LCD_ShapeWindow( s.shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s.bold), bkColor,bkColor );

				if(shape==0)
					LCD_TOUCH_ScrollSel_SetCalculate(7, &roll, &selFrame, s.y, heightAll, s.heightKey, win);

				for(int i=0; i<countKey; ++i)
				{
					if(i == selFrame)
						_KeyStrPress(posKey[i],txtKey[i],colorTxtPressKey);
					else
						_KeyStr(posKey[i],txtKey[i],colorTxtKey[i]);

				}
				LCD_Display(0 + roll * widthAll, s.x + offsX, s.y, widthAll, win);
				break;
		}

		if(startTouchIdx){
			touchTemp[0].x= s.x + posKey[0].x;
			touchTemp[1].x= touchTemp[0].x + s.widthKey;
			touchTemp[0].y= s.y + posKey[0].y;
			touchTemp[1].y= touchTemp[0].y + win;
			LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT, s.startTouchIdx, TOUCH_GET_PER_ANY_PROBE);
			s.nmbTouch++;
		}
	}
	/* ----- End User Function Definitions ----- */

	switch((int)type)
	{
		case KEYBOARD_fontRGB:
		case KEYBOARD_bkRGB:
			_ServiceRGB();
			break;
		case KEYBOARD_fontStyle:
			_ServiceStyle();
			break;
		case KEYBOARD_fontType:
			_ServiceType();
			break;
		case KEYBOARD_fontSize:
			_ServiceSize();
			break;
		case KEYBOARD_fontCoeff:
			//_ServiceCoeff();
			_ServiceSizeRoll();
			break;

		default:
			break;
	}
	#undef MIDDLE_NR
	#undef GET_X
	#undef GET_Y
	return 0;
}

//#define aaa("sdsd", zmienna)	zmienna  //PRZEMYSLEC to !!!!!!!

void FILE_NAME(setTouch)(void)
{
	#define SELECT_CURRENT_FONT(src,dst,txt,coeff) \
		LCD_SetStrVar_fontID		(v.FONT_VAR_##src, v.FONT_ID_##dst);\
		LCD_SetStrVar_fontColor	(v.FONT_VAR_##src, v.FONT_COLOR_##dst);\
		LCD_SetStrVar_bkColor  	(v.FONT_VAR_##src, v.FONT_BKCOLOR_##dst);\
		LCD_SetStrVar_coeff		(v.FONT_VAR_##src, coeff);\
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_##src, txt)
/*
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
*/
	#define CASE_TOUCH_STATE(state,touchPoint, src,dst, txt,coeff) \
		case touchPoint:\
			if(0==CHECK_TOUCH(state)){\
				if(GET_TOUCH){ FILE_NAME(main)(1,NULL); CLR_ALL_TOUCH; }\
				SELECT_CURRENT_FONT(src, dst, txt, coeff);\
				SET_TOUCH(state);\
				SetFunc();\
			}\
			else{\
				FILE_NAME(main)(1,NULL);\
				KEYBOARD_TYPE(KEYBOARD_none,0);\
				CLR_TOUCH(state);\
			}

	#define _KEYS_RELEASE_fontRGB 		if(_WasStatePrev(Touch_fontRp,Touch_bkBm)) KEYBOARD_TYPE(KEYBOARD_fontRGB, KEY_All_release)
	#define _KEYS_RELEASE_fontSize 		if(_WasStatePrev(Touch_size_plus,Touch_size_minus)) KEYBOARD_TYPE(KEYBOARD_fontSize, KEY_All_release_and_select_one)

	static uint16_t statePrev=0;
	uint16_t state, function=0;
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
	int _WasStatePrev(int rangeMin,int rangeMax){
		return (IS_RANGE(statePrev,rangeMin,rangeMax) && statePrev!=state);
	}

	void SetFunc(void){
		function=1;
	}
	int IsFunc(void){
		if(function){
			function=0;
			return 1;
		}
		return 0;
	}

	state = LCD_TOUCH_GetTypeAndPosition(&pos);
	switch(state)
	{
		CASE_TOUCH_STATE(state,Touch_FontColor, FontColor,Press, TXT_FONT_COLOR,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontRGB, KEY_All_release, LCD_RoundRectangle,0, 300,160, 60,40, 4, state, Touch_fontRp); //dac wyrownanie ADJUTMENT to LEFT RIGHT TOP .....
			/* DisplayTouchPosXY(state,pos,"Touch_FontColor"); */
			break;

		CASE_TOUCH_STATE(state,Touch_BkColor, BkColor,Press, TXT_BK_COLOR,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_bkRGB, KEY_All_release, LCD_RoundRectangle,0, 400,160, 60,40, 4, state, Touch_bkRp);
			break;

		CASE_TOUCH_STATE(state,Touch_FontStyle2, FontStyle,Press, TXT_FONT_STYLE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontStyle, KEY_Select_one, LCD_Rectangle,0, 400,160, 200,40, 0, state, Touch_style1);
			break;

		CASE_TOUCH_STATE(state,Touch_FontType2, FontType,Press, TXT_FONT_TYPE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontType, KEY_Select_one, LCD_Rectangle,0, 400,160, 200,40, 0, state, Touch_type1);
			break;

		CASE_TOUCH_STATE(state,Touch_FontSize2, FontSize,Press, TXT_FONT_SIZE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontSize, KEY_All_release_and_select_one, LCD_RoundRectangle,0, 410,170, 80,40, 6, state, Touch_size_plus);
			break;

		CASE_TOUCH_STATE(state,Touch_FontCoeff2, Coeff,Press, TXT_COEFF,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontCoeff, KEY_Select_one, LCD_LittleRoundRectangle,0, 610,30, 160,40, 0, state, Touch_coeff1);
			break;

		case Touch_FontStyle:   //sPRAWDZIC tOUCH.IDX NA DEBUGU !!!!
			ChangeFontStyle(NONE_TYPE_REQ);
			if(CHECK_TOUCH(Touch_FontStyle2))
				KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one );
			_SaveState();
			break;

		case Touch_FontType:
			ReplaceLcdStrType(NONE_TYPE_REQ);
			if(CHECK_TOUCH(Touch_FontType2))
				KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one );
			_SaveState();
			break;

		case Touch_FontSize:
			ChangeFontBoldItalNorm(NONE_TYPE_REQ);
			if(CHECK_TOUCH(Touch_FontSize2))
				KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one );
			_SaveState();
			break;

//		case Touch_FontCoeff: //TEGO nie bedzie
//			INCR_WRAP(Test.usun,1,0,3);
//			if(CHECK_TOUCH(Touch_FontCoeff2))
//				KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one );
//			_SaveState();
//			break;

		case Touch_fontRp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','R', 1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Red_plus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontGp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','G', 1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Green_plus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontBp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','B', 1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Blue_plus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontRm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','R',-1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Red_minus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontGm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','G',-1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Green_minus ); Test.step=5; _SaveState(); break;
		case Touch_fontBm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','B',-1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Blue_minus ); 	Test.step=5; _SaveState(); break;

		case Touch_bkRp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('b','R', 1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Red_plus );  	Test.step=5; _SaveState(); break;
		case Touch_bkGp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('b','G', 1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Green_plus ); Test.step=5; _SaveState(); break;
		case Touch_bkBp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('b','B', 1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Blue_plus );  Test.step=5; _SaveState(); break;
		case Touch_bkRm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('b','R',-1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Red_minus ); 	Test.step=5; _SaveState(); break;
		case Touch_bkGm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('b','G',-1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Green_minus );Test.step=5; _SaveState(); break;
		case Touch_bkBm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('b','B',-1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Blue_minus ); Test.step=5; _SaveState(); break;

		case Touch_style1: ChangeFontStyle(Arial);  				KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one ); _SaveState(); break;
		case Touch_style2: ChangeFontStyle(Times_New_Roman);  	KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one ); _SaveState(); break;
		case Touch_style3: ChangeFontStyle(Comic_Saens_MS);  	KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one ); _SaveState(); break;

		case Touch_type1:	ReplaceLcdStrType(0);  KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one ); _SaveState(); break;
		case Touch_type2:	ReplaceLcdStrType(1);  KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one ); _SaveState(); break;
		case Touch_type3:	ReplaceLcdStrType(2);  KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one ); _SaveState(); break;

//		case Touch_size_plus:  //ROLL SIZE !!!
//			if(LCD_TOUCH_ScrollSel_Service(7,press, &pos.y))
//				KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_Select_one);
//			_SaveState();
//			DisplayTouchPosXY(Touch_size_plus,pos,"Touch_size_plus ");
//			break;
		case Touch_size_plus: 	_KEYS_RELEASE_fontSize;	IncFontSize();  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_Size_plus ); _SaveState();  break;
		case Touch_size_minus:	_KEYS_RELEASE_fontSize; DecFontSize();  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_Size_minus ); _SaveState();  break;
		case Touch_size_norm: 	ChangeFontBoldItalNorm(0);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one ); _SaveState(); break;
		case Touch_size_bold: 	ChangeFontBoldItalNorm(1);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one ); _SaveState(); break;
		case Touch_size_italic: ChangeFontBoldItalNorm(2);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one ); _SaveState(); break;


		case Touch_coeff1:
			if(LCD_TOUCH_ScrollSel_Service(7,press, &pos.y))
				KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one);
			_SaveState();
			break;
//		case Touch_coeff1: Test.usun=0; 	KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one );  break;
//		case Touch_coeff2: Test.usun=1; 	KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one );  break;
//		case Touch_coeff3: Test.usun=2; 	KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one );  break;
//		case Touch_coeff4: Test.usun=3; 	KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one );  break;

		case Point_1:
			DisplayTouchPosXY(Point_1,pos,"Point_1 ");
			break;
		case Move_4:
			DisplayTouchPosXY(Move_4,pos,"Move_4 ");
			break;
//		case AnyPress:
//			DbgVar(1,40,"\r\nAny Press: x= %03d y= %03d", pos.x, pos.y);
//			break;
//		case AnyPressWithWait:
//			DbgVar(1,40,"\r\nAny Press With Wait: x= %03d y= %03d", pos.x, pos.y);
//			break;
		default:

			if(_WasState(Touch_fontRp) || _WasState(Touch_fontRm) ||
				_WasState(Touch_fontGp) || _WasState(Touch_fontGm) ||
				_WasState(Touch_fontBp) || _WasState(Touch_fontBm) ||
				_WasState(Touch_bkRp) || _WasState(Touch_bkRm) ||
				_WasState(Touch_bkGp) || _WasState(Touch_bkGm) ||
				_WasState(Touch_bkBp) || _WasState(Touch_bkBm) )
			{
				KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_All_release );
				Test.step=1;
			}
			if(_WasState(Touch_size_plus) || _WasState(Touch_size_minus))   //a to zakomentuj !!!!!!  //ROLL SIZE !!!
				KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one );

			if(_WasState(Touch_FontStyle) ||
				_WasState(Touch_style1) ||
				_WasState(Touch_style2) ||
				_WasState(Touch_style3))
				SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontStyle,1,LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontStyle,TXT_FONT_STYLE));

			if(_WasState(Touch_FontType) ||
				_WasState(Touch_type1) ||
				_WasState(Touch_type2) ||
				_WasState(Touch_type3))
				SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontType,1,LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontType,TXT_FONT_TYPE));

			if(_WasState(Touch_FontSize) ||
				_WasState(Touch_size_norm) ||
				_WasState(Touch_size_bold) ||
				_WasState(Touch_size_italic))
				SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontSize,1,LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontSize,TXT_FONT_SIZE));

			if(_WasState(Touch_coeff1)){
				if(LCD_TOUCH_ScrollSel_Service(7,release, NULL))
					KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Select_one);
			}

//			if(_WasState(Touch_size_plus)){  //ROLL SIZE !!!
//				if(LCD_TOUCH_ScrollSel_Service(7,release, NULL))
//					KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_Select_one);
//			}

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
		ChangeFontStyle(NONE_TYPE_REQ);

	else if(DEBUG_RcvStr("`"))
		ReplaceLcdStrType(NONE_TYPE_REQ);

	else if(DEBUG_RcvStr("r"))
		ChangeFontBoldItalNorm(NONE_TYPE_REQ);

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

		//SCREEN_Fonts_funcSet(COLOR_FramePress, BLACK);

	}
	else if(DEBUG_RcvStr("-"))
	{
		FILE_NAME(printInfo)();
	}

}

static void LoadFonts(int startFontID, int endFontID){
	#define OMITTED_FONTS	1	/*this define delete for another screens*/
	#define A(x)	 *((int*)((int*)(&v)+x))

	int d = endFontID-startFontID + 1 + OMITTED_FONTS;
	int j=0;

	for(int i=startFontID; i<=endFontID; ++i){
		*((int*)((int*)(&v)+i)) = LCD_LoadFont_DependOnColors( A(j),A(j+d),A(j+3*d),A(j+2*d), FILE_NAME(GetDefaultParam)(i));
		j++;
	}
/*
	v.FONT_ID_Title 	 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Title),	  	FILE_NAME(GetDefaultParam)(FONT_ID_Title));
	v.FONT_ID_FontColor		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontColor),	FILE_NAME(GetDefaultParam)(FONT_ID_FontColor));
	v.FONT_ID_BkColor 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(BkColor),  	FILE_NAME(GetDefaultParam)(FONT_ID_BkColor));
	v.FONT_ID_FontType 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontType), 	FILE_NAME(GetDefaultParam)(FONT_ID_FontType));
	v.FONT_ID_FontSize 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontSize), 	FILE_NAME(GetDefaultParam)(FONT_ID_FontSize));
	v.FONT_ID_FontStyle  	= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(FontStyle),	FILE_NAME(GetDefaultParam)(FONT_ID_FontStyle));

	v.FONT_ID_Coeff 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Coeff), 		 FILE_NAME(GetDefaultParam)(FONT_ID_Coeff));
	v.FONT_ID_LenWin 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(LenWin), 		 FILE_NAME(GetDefaultParam)(FONT_ID_LenWin));
	v.FONT_ID_OffsWin 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(OffsWin), 	 FILE_NAME(GetDefaultParam)(FONT_ID_OffsWin));
	v.FONT_ID_LoadFontTime = LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(LoadFontTime),FILE_NAME(GetDefaultParam)(FONT_ID_LoadFontTime));
	v.FONT_ID_PosCursor 	= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(PosCursor), 	 FILE_NAME(GetDefaultParam)(FONT_ID_PosCursor));
	v.FONT_ID_CPUusage 		= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(CPUusage), 	 FILE_NAME(GetDefaultParam)(FONT_ID_CPUusage));
	v.FONT_ID_Speed 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Speed), 		 FILE_NAME(GetDefaultParam)(FONT_ID_Speed));
	v.FONT_ID_Press 			= LCD_LoadFont_DependOnColors( LOAD_FONT_PARAM(Press), 		 FILE_NAME(GetDefaultParam)(FONT_ID_Press));
*/
}

static void ELEMENT_fontRGB(int argNmb, StructTxtPxlLen *lenStr)
{
	int spaceMain_width = LCD_GetWholeStrPxlWidth(v.FONT_ID_FontColor," ",0,ConstWidth);
	int digit3main_width = LCD_GetWholeStrPxlWidth(v.FONT_ID_FontColor,INT2STR(Test.font[0]),0,ConstWidth);
	int _GetWidth(char *txt){ return LCD_GetWholeStrPxlWidth(v.FONT_ID_Descr,txt,0,ConstWidth); }
	int xPos = LCD_Xpos(*lenStr,SetPos,60);
	int yPos = LCD_Ypos(*lenStr,SetPos,30);

	int xPos_under_left = MIDDLE( xPos+spaceMain_width, digit3main_width, _GetWidth(GetSelTxt(0,FILE_NAME(Lang),3)));
	int xPos_under_right = MIDDLE( xPos + 3*spaceMain_width + 2*digit3main_width, digit3main_width, _GetWidth(GetSelTxt(0,FILE_NAME(Lang),5)));

	*lenStr=LCD_StrDependOnColorsDescrVar_array(STR_FONT_PARAM(FontColor, FillMainFrame), xPos, yPos, TXT_FONT_COLOR, fullHight, 0,250, ConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 								 Above_center, GetSelTxt(0,FILE_NAME(Lang),1), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, COLOR_GRAY(0x0A), 	 v.FONT_BKCOLOR_Descr, 4, 								 Left_mid, 		GetSelTxt(40,FILE_NAME(Lang),2), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(251,29,27), v.FONT_BKCOLOR_Descr, 4|(xPos_under_left<<16),  Under_left,	GetSelTxt(50,FILE_NAME(Lang),3), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(60,247,68), v.FONT_BKCOLOR_Descr, 4, 								 Under_center, GetSelTxt(60,FILE_NAME(Lang),4), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(51,90,245), v.FONT_BKCOLOR_Descr, 4|(xPos_under_right<<16), Under_right,	GetSelTxt(70,FILE_NAME(Lang),5), fullHight, 0,250, NoConstWidth,\
		LCD_STR_DESCR_PARAM_NUMBER(5) );

	LCD_SetBkFontShape(v.FONT_VAR_FontColor,BK_LittleRound);
	if(0==argNmb)
		SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT, Touch_FontColor, press, v.FONT_VAR_FontColor,0, *lenStr);
}

void FILE_NAME(main)(int argNmb, char **argVal)  //tu W **arcv PRZEKAZ TEXT !!!!!! dla fonts !!!
{
	char *ptr=NULL;     //ODKRYJ W usrtawienia debug aby tylko wyswietlic jeden leemnet z duzej struktury np Touch[].idx !!!!!!

	if(0==argNmb)
	{
		SCREEN_ResetAllParameters();		//ROBIMY TU JUZ KLAWIATUTE i wprowadzanie textu dowolnego !!!!!
		LCD_TOUCH_DeleteAllSetTouch();  //Przyciski do zmian paranetru !!!!
		FONTS_LCD_ResetParam();

		LCD_Clear(v.COLOR_BkScreen);

		LoadFonts(FONT_ID_Title, FONT_ID_Press); //ZROBIC AUTOMATYCZNE testy wszystkich mozliwosci !!!!!!! taki interfejs testowy
		LCD_LoadFontVar();



	 	touchTemp[0].y= 240;
	 	touchTemp[1].y= 400;
	 	touchTemp[0].x= 0;
	 	touchTemp[1].x= 200;
	 	LCD_TOUCH_Set(ID_TOUCH_MOVE_DOWN,Move_4,press);

	 	touchTemp[0].y= 240;
	 	touchTemp[1].y= 480;
	 	touchTemp[0].x= 650;
	 	touchTemp[1].x= 800;
	 	LCD_TOUCH_Set(ID_TOUCH_POINT,Point_1,release);


	}
	else{
		LCD_Clear(v.COLOR_BkScreen);   //ZROBIC animacje ze samo sie klioka i chmurka z info ze przytrzymac na 2 sekundy ....
	}
	/*FILE_NAME(printInfo)();*/

	LCD_DrawMainFrame(LCD_RoundRectangle,NoIndDisp,0, 0,0, LCD_X,180,SHAPE_PARAM(MainFrame,FillMainFrame,BkScreen)); // dlatego daj bk color MYGRAY aby zachowac kolory przy cieniowaniu !!!!!

	ptr = GetSelTxt(0,FILE_NAME(Lang),0);
	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(Title, FillMainFrame),LCD_Xpos(lenStr,SetPos,600),LCD_Ypos(lenStr,SetPos,8), ptr,fullHight,0,255,NoConstWidth);



	ELEMENT_fontRGB(argNmb,&lenStr);


	_StartDrawLine(0,LCD_X, 10,LCD_Ypos(lenStr,IncPos,20)+15);   _DrawRight(200, COLOR_GRAY(0xA0));



	lenStr=LCD_StrDependOnColorsDescrVar(STR_FONT_PARAM(BkColor, FillMainFrame), LCD_Xpos(lenStr,GetPos,0), LCD_Ypos(lenStr,IncPos,15), TXT_BK_COLOR, fullHight, 0,250, ConstWidth, \
													 v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, Left_up, "Mjyk:", fullHight, 0,250, NoConstWidth);
	LCD_SetBkFontShape(v.FONT_VAR_BkColor,BK_LittleRound);
	if(0==argNmb) SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT, Touch_BkColor, press, v.FONT_VAR_BkColor,0, lenStr);





	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontType, FillMainFrame),  LCD_Xpos(lenStr,SetPos,250), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_TYPE, 	fullHight,0,255,NoConstWidth);
	if(0==argNmb){ SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontType, LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontType,0, lenStr);
						SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, Touch_FontType2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontType,1, lenStr);
	}

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontSize, FillMainFrame),  LCD_Xpos(lenStr,IncPos,40), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_SIZE, 	fullHight,0,255,NoConstWidth); LCD_SetBkFontShape(v.FONT_VAR_FontSize,BK_LittleRound);
	if(0==argNmb){ SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontSize, LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontSize,0, lenStr);
						SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, Touch_FontSize2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontSize,1, lenStr);
	}

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(FontStyle, FillMainFrame), LCD_Xpos(lenStr,IncPos,75), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_STYLE, 	fullHight,0,255,NoConstWidth);
	if(0==argNmb){ SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontStyle, LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontStyle,0, lenStr);
						SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, Touch_FontStyle2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontStyle,1, lenStr);
	}

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(Coeff,FillMainFrame),300, 50, TXT_COEFF,  		 	fullHight,0,255,ConstWidth);  //DESCRiption malymi szarymi literkami !!!! oddzoelone kreseczkami !!!
	if(0==argNmb){ /*SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT, Touch_FontCoeff, press, v.FONT_VAR_Coeff,0, lenStr);*/
						SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, Touch_FontCoeff2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_Coeff,1, lenStr);
	}

	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(LenWin,FillMainFrame),400,  0, TXT_LEN_WIN,		 	 halfHight,0,255,ConstWidth);
	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(OffsWin,FillMainFrame),400, 20, TXT_OFFS_WIN,	    	 halfHight,0,255,ConstWidth);
	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(LoadFontTime,FillMainFrame),320, 20, TXT_LOAD_FONT_TIME,	 halfHight,0,255,ConstWidth);
	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(PosCursor,FillMainFrame),440, 40,TXT_PosCursor(),halfHight,0,255,ConstWidth);
	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(CPUusage,FillMainFrame),450, 0, 	TXT_CPU_USAGE,	 		 halfHight,0,255,ConstWidth);


	 Test.yFontsField=240;
	 LCD_Ymiddle(0,SetPos, SetPosAndWidth(Test.yFontsField,240) );
	 LCD_Xmiddle(0,SetPos, SetPosAndWidth(Test.xFontsField,LCD_GetXSize()),NULL,0,NoConstWidth);

	 LCD_SetBkFontShape(v.FONT_VAR_Fonts,BK_LittleRound);


	 if(0==argNmb) //To zmien!!!!
	 {
		 StartMeasureTime_us();    //daj mozliwosc wpisywania dowolnego textu aby korygowac odstepy miedzy kazdymi fontami jakimi sie chce !!!!!!!
		 if(Test.type)
			 lenStr=LCD_StrVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(0,GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(0,GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,v.COLOR_BkScreen,0,Test.constWidth,v.COLOR_BkScreen);
		 else
			 lenStr=LCD_StrChangeColorVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(0,GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(0,GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,RGB_BK,RGB_FONT,Test.coeff,Test.constWidth,v.COLOR_BkScreen);
		 Test.speed=StopMeasureTime_us("");
	 }
	 else
	 {
		 StartMeasureTime_us();
		 if(Test.type)
			 lenStr=LCD_StrVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(0,GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(0,GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,LCD_GetStrVar_bkColor(v.FONT_VAR_Fonts),0,Test.constWidth,v.COLOR_BkScreen);
		 else
			 lenStr=LCD_StrChangeColorVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(0,GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(0,GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,RGB_BK,RGB_FONT,Test.coeff,Test.constWidth,v.COLOR_BkScreen);
		 Test.speed=StopMeasureTime_us("");
	 }







	 lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(Speed, FillMainFrame),450,0,TXT_SPEED,fullHight,0,255,ConstWidth);

	LCD_Show();

	if(0==argNmb) DbgVar(DEBUG_ON,100,Clr_ Mag_"\r\nStart: %s\r\n"_X,GET_CODE_FUNCTION);  //przed dac load FOnts !!!!
}
