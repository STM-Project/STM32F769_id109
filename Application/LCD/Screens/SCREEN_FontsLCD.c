
#include "SCREEN_FontsLCD.h"
#include "LCD_BasicGaphics.h"
#include "LCD_fonts_images.h"
#include "LCD_Common.h"
#include "timer.h"
#include "lang.h"
#include "SCREEN_ReadPanel.h"
#include "debug.h"
#include "string_oper.h"
#include "cpu_utils.h"
#include "tim.h"
#include "touch.h"

/*----------------- Main Settings ------------------*/

#define FILE_NAME(extend) SCREEN_Fonts_##extend

static const char FILE_NAME(Lang)[]="\
Czcionki LCD,Fonts LCD,\
";\

#define SCREEN_FONTS_SET_PARAMETERS \
/* id   name							default value */ \
	X(0, FONT_SIZE_Title, 	 		FONT_14_bold) \
	X(1, FONT_SIZE_FontColor, 	 	FONT_10) \
	X(2, FONT_SIZE_BkColor,			FONT_10) \
	X(3, FONT_SIZE_FontSize,		FONT_10) \
	X(4, FONT_SIZE_FontStyle,		FONT_10) \
	X(5, FONT_SIZE_Coeff,			FONT_10) \
	X(6, FONT_SIZE_LenWin,			FONT_10) \
	X(7, FONT_SIZE_OffsWin,			FONT_10) \
	X(8, FONT_SIZE_LoadFontTime,	FONT_10) \
	X(9, FONT_SIZE_PosCursor,		FONT_10) \
	X(10, FONT_SIZE_CPUusage,		FONT_10) \
	X(11, FONT_SIZE_Speed,			FONT_10) \
	X(12, FONT_SIZE_Fonts,			FONT_10) \
	\
	X(13, FONT_STYLE_Title, 	 	Arial) \
	X(14, FONT_STYLE_FontColor, 	Arial) \
	X(15, FONT_STYLE_BkColor, 		Arial) \
	X(16, FONT_STYLE_FontSize, 	Arial) \
	X(17, FONT_STYLE_FontStyle, 	Arial) \
	X(18, FONT_STYLE_Coeff, 		Arial) \
	X(19, FONT_STYLE_LenWin, 		Arial) \
	X(20, FONT_STYLE_OffsWin, 		Arial) \
	X(21, FONT_STYLE_LoadFontTime,Arial) \
	X(22, FONT_STYLE_PosCursor,	Arial) \
	X(23, FONT_STYLE_CPUusage,		Arial) \
	X(24, FONT_STYLE_Speed,			Arial) \
	X(25, FONT_STYLE_Fonts, 		Arial) \
	\
	X(26, FONT_COLOR_Title,  	 	WHITE) \
	X(27, FONT_COLOR_FontColor, 	WHITE) \
	X(28, FONT_COLOR_BkColor, 	 	WHITE) \
	X(29, FONT_COLOR_FontSize,  	WHITE) \
	X(30, FONT_COLOR_FontStyle,  	WHITE) \
	X(31, FONT_COLOR_Coeff,  		WHITE) \
	X(32, FONT_COLOR_LenWin,  		WHITE) \
	X(33, FONT_COLOR_OffsWin,  	WHITE) \
	X(34, FONT_COLOR_LoadFontTime,WHITE) \
	X(35, FONT_COLOR_PosCursor,	WHITE) \
	X(36, FONT_COLOR_CPUusage,		WHITE) \
	X(37, FONT_COLOR_Speed,			WHITE) \
	X(38, FONT_COLOR_Fonts,  		WHITE) \
	\
	X(39, COLOR_BkScreen,  		MYGRAY) \
	X(40, DEBUG_ON,  	1) \
	\
	X(41, FONT_SIZE_Press, 	 	FONT_12_bold) \
	X(42, FONT_STYLE_Press, 	Comic_Saens_MS) \
	X(43, FONT_COLOR_Press, 	RED) \
	X(44, FONT_BKCOLOR_Press, 	WHITE) \
	\
	X(45, FONT_ID_Title,			fontID_1) \
	X(46, FONT_ID_FontColor,	fontID_2) \
	X(47, FONT_ID_BkColor, 		fontID_3) \
	X(48, FONT_ID_FontSize, 	fontID_4) \
	X(49, FONT_ID_FontStyle,  	fontID_5) \
	X(50, FONT_ID_Coeff,  		fontID_6) \
	X(51, FONT_ID_LenWin,  		fontID_7) \
	X(52, FONT_ID_OffsWin,  	fontID_8) \
	X(53, FONT_ID_LoadFontTime,fontID_9) \
	X(54, FONT_ID_PosCursor,	fontID_10) \
	X(55, FONT_ID_CPUusage,		fontID_11) \
	X(56, FONT_ID_Speed,			fontID_12) \
	X(57, FONT_ID_Fonts,  		fontID_13) \
	\
	X(58, FONT_VAR_Title,			fontVar_1) \
	X(59, FONT_VAR_FontColor,		fontVar_2) \
	X(60, FONT_VAR_BkColor, 		fontVar_3) \
	X(61, FONT_VAR_FontSize, 		fontVar_4) \
	X(62, FONT_VAR_FontStyle,  	fontVar_5) \
	X(63, FONT_VAR_Coeff,  			fontVar_6) \
	X(64, FONT_VAR_LenWin,  		fontVar_7) \
	X(65, FONT_VAR_OffsWin,  		fontVar_8) \
	X(66, FONT_VAR_LoadFontTime,	fontVar_9) \
	X(67, FONT_VAR_PosCursor,		fontVar_10) \
	X(68, FONT_VAR_CPUusage,		fontVar_11) \
	X(68, FONT_VAR_Speed,			fontVar_12) \
	X(69, FONT_VAR_Fonts,  			fontVar_13) \
	X(70, FONT_BKCOLOR_FontColor,  		MYGRAY) \
/*------------ End Main Settings -----------------*/

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

static int FILE_NAME(SetDefaultParam)(int param){
	int temp;
	#define X(a,b,c) \
		if(b==param){ v.b=c; temp=c; }
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
		return temp;
}

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

void FILE_NAME(setDefaultAllParam)(void){
	#define X(a,b,c) FILE_NAME(funcSet)(b,c);
		SCREEN_FONTS_SET_PARAMETERS
	#undef X
	FILE_NAME(SelBits)=0;
}

void FILE_NAME(debugRcvStr)(void);
void FILE_NAME(setTouch)(void);

void 	FILE_NAME(main)(int argNmb, char **argVal);

/*------------ End Main Screen MACRO -----------------*/

static char bufTemp[50];

#define FLOAT2STR(val)	Float2Str(val,' ',4,Sign_plusMinus,1)
#define INT2STR(val)		  Int2Str(val,'0',3,Sign_none)
#define INT2STR_TIME(val) Int2Str(val,' ',6,Sign_none)
#define ONEBIT(val)	     Int2Str(val,' ',0,Sign_none)

#define TXT_FONT_COLOR 	StrAll(5,INT2STR(Test.font[0])," ",INT2STR(Test.font[1])," ",INT2STR(Test.font[2]))
#define TXT_BK_COLOR 	StrAll(5,INT2STR(Test.bk[0]),  " ",INT2STR(Test.bk[1]),  " ",INT2STR(Test.bk[2]))
#define TXT_FONT_SIZE	StrAll(3,LCD_LoadFontStrType(bufTemp,0,Test.type+1),":",LCD_FontSize2Str(bufTemp+25,Test.size))
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

static void ClearCursorField(void){
	LCD_ShapeIndirect(LCD_GetStrVar_x(v.FONT_VAR_Fonts),LCD_GetStrVar_y(v.FONT_VAR_Fonts)+LCD_GetFontHeight(v.FONT_ID_Fonts)+Test.spaceCoursorY,LCD_Rectangle, lenStr.inPixel,Test.heightCursor, MYGRAY,MYGRAY,MYGRAY);
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
	case PARAM_SIZE:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontSize,TXT_FONT_SIZE);
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
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontStyle,TXT_FONT_STYLE);
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

static int ChangeTxt(void){
	return CopyCharsTab(Test.txt,Test.lenWin,Test.offsWin,Test.size);
}
static void ResetRGB(void)
{
	Test.xFontsField=0;

	Test.bk[0]=0;
	Test.bk[1]=0;
	Test.bk[2]=0;

	Test.font[0]=R_PART(v.FONT_COLOR_Fonts);
	Test.font[1]=G_PART(v.FONT_COLOR_Fonts);
	Test.font[2]=B_PART(v.FONT_COLOR_Fonts);

	Test.step=5;
	Test.coeff=255;
	Test.coeff_prev=Test.coeff;

	Test.type=0;
	Test.speed=0;

	Test.size=v.FONT_SIZE_Fonts;
	Test.style=v.FONT_STYLE_Fonts;

	//strcpy(Test.txt,"Rafa"ł" Markielowski");

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
	case Comic_Saens_MS:  Test.style=Arial; 			 break;
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

int aaaaa=0;
void FILE_NAME(setTouch)(void)
{
	uint16_t state;
	XY_Touch_Struct pos;

	state = LCD_Touch_Get(&pos);
	switch(state)
	{
		case Point_1:


//			if(aaaaa == 0)
//			{
//				v.FONT_SIZE_FontColor = FONT_14_bold;
//				v.FONT_COLOR_FontColor = BLACK;
//				v.COLOR_BkScreen = WHITE;
//				v.FONT_ID_FontColor	= LCD_LoadFont_DependOnColors(v.FONT_SIZE_FontColor, v.FONT_STYLE_FontColor, v.COLOR_BkScreen, v.FONT_COLOR_FontColor, FILE_NAME(GetDefaultParam)(FONT_ID_FontColor));
//
//
//				LCD_SetStrVar_fontID(v.FONT_VAR_FontColor,v.FONT_ID_FontColor);
//				LCD_SetStrVar_fontColor(v.FONT_VAR_FontColor,v.FONT_COLOR_FontColor);
//				LCD_SetStrVar_bkColor(v.FONT_VAR_FontColor,v.COLOR_BkScreen);
//				LCD_SetStrVar_coeff(v.FONT_VAR_FontColor,1);
//				LCD_SetStrVar_bkScreenColor(v.FONT_VAR_FontColor, v.COLOR_BkScreen);
//				LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontColor,TXT_FONT_COLOR);
//
//HAL_Delay(1000);
//
//				FILE_NAME(setDefaultAllParam)();
//				v.FONT_COLOR_FontColor =YELLOW;
//				v.FONT_SIZE_FontColor =FONT_14_bold;
//				FILE_NAME(main)();
//
//
//					aaaaa=1;
//
//
//			}
//			else
//			{
//
//
//				FILE_NAME(setDefaultAllParam)();
//
//				FILE_NAME(main)();
//
//
//
//					aaaaa=0;
//			}

			if(aaaaa == 0)
			{

				v.FONT_ID_FontColor = LCD_LoadFont_DependOnColors(v.FONT_SIZE_Press, v.FONT_STYLE_Press, v.FONT_BKCOLOR_Press, v.FONT_COLOR_Press, FILE_NAME(GetDefaultParam)(FONT_ID_FontColor));

				LCD_SetStrVar_fontID(v.FONT_VAR_FontColor,v.FONT_ID_FontColor);
				LCD_SetStrVar_fontColor(v.FONT_VAR_FontColor,v.FONT_COLOR_Press);
				LCD_SetStrVar_bkColor(v.FONT_VAR_FontColor,v.FONT_BKCOLOR_Press);
				LCD_SetStrVar_bkScreenColor(v.FONT_VAR_FontColor, v.COLOR_BkScreen);

				if		 (v.FONT_BKCOLOR_Press==MYGRAY && v.FONT_COLOR_Press == WHITE)	  LCD_SetStrVar_coeff(v.FONT_VAR_FontColor,0);  // to zamienic na 1 funkcje ukrytą !!!!!!!
				else if(v.FONT_BKCOLOR_Press==MYGRAY && v.FONT_COLOR_Press == MYGREEN) LCD_SetStrVar_coeff(v.FONT_VAR_FontColor,0);
				else if(v.FONT_BKCOLOR_Press==WHITE  && v.FONT_COLOR_Press == BLACK)	  LCD_SetStrVar_coeff(v.FONT_VAR_FontColor,1);
				else																						  LCD_SetStrVar_coeff(v.FONT_VAR_FontColor,240);


				LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontColor,TXT_FONT_COLOR);

//HAL_Delay(1000);

				FILE_NAME(setDefaultAllParam)();
				v.FONT_SIZE_FontColor  = v.FONT_SIZE_Press;
				v.FONT_COLOR_FontColor = v.FONT_COLOR_Press;
				v.FONT_STYLE_FontColor = v.FONT_STYLE_Press;
				v.FONT_BKCOLOR_FontColor = v.FONT_BKCOLOR_Press;
				FILE_NAME(main)(argNmb,argVal);


					aaaaa=1;



			}
			else
			{
				///DAC TU funkcje czy IsStillPress()

				FILE_NAME(setDefaultAllParam)();

				FILE_NAME(main)(argNmb,argVal);



					aaaaa=0;
			}



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
}

void FILE_NAME(main)(int argNmb, char **argVal)
{
	char *ptr=NULL;

	SCREEN_ResetAllParameters();
	DeleteAllTouch();
	ResetRGB();
	LCD_Clear(v.COLOR_BkScreen);

		 	touchTemp[0].x= 0;
		 	touchTemp[0].y= 0;
		 	touchTemp[1].x= touchTemp[0].x+200;
		 	touchTemp[1].y= touchTemp[0].y+150;
		 	SetTouch(ID_TOUCH_POINT,Point_1,press);
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

		 	touchTemp[0].x= 400;
		 	touchTemp[1].x= 800;
		 	touchTemp[0].y= 240;
		 	touchTemp[1].y= 480;
		 	//SetTouch(ID_TOUCH_GET_ANY_POINT,AnyPress,TOUCH_GET_PER_X_PROBE);
		 	SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT,AnyPressWithWait,TOUCH_GET_PER_X_PROBE);  //W DEBUG FPNTS WPISZ JESZCZE JAKI LCD_STR !!!!!


	v.FONT_ID_Title 	 	= LCD_LoadFont_DependOnColors(v.FONT_SIZE_Title, 	  v.FONT_STYLE_Title, 	  v.COLOR_BkScreen, v.FONT_COLOR_Title,	  FILE_NAME(GetDefaultParam)(FONT_ID_Title));
	v.FONT_ID_FontColor	= LCD_LoadFont_DependOnColors(v.FONT_SIZE_FontColor, v.FONT_STYLE_FontColor, v.FONT_BKCOLOR_FontColor, v.FONT_COLOR_FontColor, FILE_NAME(GetDefaultParam)(FONT_ID_FontColor));
	v.FONT_ID_BkColor 	= LCD_LoadFont_DependOnColors(v.FONT_SIZE_BkColor,   v.FONT_STYLE_BkColor,   v.COLOR_BkScreen, v.FONT_COLOR_BkColor,   FILE_NAME(GetDefaultParam)(FONT_ID_BkColor));
	v.FONT_ID_FontSize 	= LCD_LoadFont_DependOnColors(v.FONT_SIZE_FontSize,  v.FONT_STYLE_FontSize,  v.COLOR_BkScreen, v.FONT_COLOR_FontSize,  FILE_NAME(GetDefaultParam)(FONT_ID_FontSize));
	v.FONT_ID_FontStyle  = LCD_LoadFont_DependOnColors(v.FONT_SIZE_FontStyle, v.FONT_STYLE_FontStyle, v.COLOR_BkScreen, v.FONT_COLOR_FontStyle, FILE_NAME(GetDefaultParam)(FONT_ID_FontStyle));

	v.FONT_ID_Coeff 			= LCD_LoadFont_DependOnColors(v.FONT_SIZE_Coeff,			v.FONT_STYLE_Coeff, 			v.COLOR_BkScreen, v.FONT_COLOR_Coeff, 			FILE_NAME(GetDefaultParam)(FONT_ID_Coeff));
	v.FONT_ID_LenWin 			= LCD_LoadFont_DependOnColors(v.FONT_SIZE_LenWin,			v.FONT_STYLE_LenWin, 		v.COLOR_BkScreen, v.FONT_COLOR_LenWin, 		FILE_NAME(GetDefaultParam)(FONT_ID_LenWin));
	v.FONT_ID_OffsWin 		= LCD_LoadFont_DependOnColors(v.FONT_SIZE_OffsWin,			v.FONT_STYLE_OffsWin, 		v.COLOR_BkScreen, v.FONT_COLOR_OffsWin, 		FILE_NAME(GetDefaultParam)(FONT_ID_OffsWin));
	v.FONT_ID_LoadFontTime 	= LCD_LoadFont_DependOnColors(v.FONT_SIZE_LoadFontTime,	v.FONT_STYLE_LoadFontTime, v.COLOR_BkScreen, v.FONT_COLOR_LoadFontTime, FILE_NAME(GetDefaultParam)(FONT_ID_LoadFontTime));
	v.FONT_ID_PosCursor 		= LCD_LoadFont_DependOnColors(v.FONT_SIZE_PosCursor,		v.FONT_STYLE_PosCursor, 	v.COLOR_BkScreen, v.FONT_COLOR_PosCursor, 	FILE_NAME(GetDefaultParam)(FONT_ID_PosCursor));
	v.FONT_ID_CPUusage 		= LCD_LoadFont_DependOnColors(v.FONT_SIZE_CPUusage,		v.FONT_STYLE_CPUusage, 		v.COLOR_BkScreen, v.FONT_COLOR_CPUusage, 		FILE_NAME(GetDefaultParam)(FONT_ID_CPUusage));
	v.FONT_ID_Speed 			= LCD_LoadFont_DependOnColors(v.FONT_SIZE_Speed,			v.FONT_STYLE_Speed, 			v.COLOR_BkScreen, v.FONT_COLOR_Speed, 			FILE_NAME(GetDefaultParam)(FONT_ID_Speed));

	LCD_LoadFontVar();
	//FILE_NAME(printInfo)();

	ptr = GetSelTxt(0,FILE_NAME(Lang),0);
	lenStr=LCD_StrDependOnColorsVar(v.FONT_VAR_Title,	   v.FONT_ID_Title, 	 	LCD_Xpos(lenStr,SetPos,600),LCD_Ypos(lenStr,SetPos,0), ptr, 				fullHight,0,v.COLOR_BkScreen,v.FONT_COLOR_Title,	  255,0,v.COLOR_BkScreen);
	lenStr=LCD_StrDependOnColorsVar(v.FONT_VAR_FontColor, v.FONT_ID_FontColor, 23, 								 LCD_Ypos(lenStr,SetPos,0), TXT_FONT_COLOR, 	fullHight,0,v.FONT_BKCOLOR_FontColor,v.FONT_COLOR_FontColor, 240,1,v.COLOR_BkScreen);
	lenStr=LCD_StrDependOnColorsVar(v.FONT_VAR_BkColor,   v.FONT_ID_BkColor,   23, 								 LCD_Ypos(lenStr,IncPos,5), TXT_BK_COLOR,    fullHight,0,v.COLOR_BkScreen,v.FONT_COLOR_BkColor,	  255,1,v.COLOR_BkScreen);
	lenStr=LCD_StrDependOnColorsVar(v.FONT_VAR_FontSize,  v.FONT_ID_FontSize,  LCD_Xpos(lenStr,SetPos,23), LCD_Ypos(lenStr,IncPos,5), TXT_FONT_SIZE, 	fullHight,0,v.COLOR_BkScreen,v.FONT_COLOR_FontSize,  255,0,v.COLOR_BkScreen);
	lenStr=LCD_StrDependOnColorsVar(v.FONT_VAR_FontStyle, v.FONT_ID_FontStyle, LCD_Xpos(lenStr,IncPos,70), LCD_Ypos(lenStr,GetPos,0), TXT_FONT_STYLE, 	fullHight,0,v.COLOR_BkScreen,v.FONT_COLOR_FontStyle, 255,0,v.COLOR_BkScreen);

	LCD_StrDependOnColorsVar	  (v.FONT_VAR_Coeff, 		 v.FONT_ID_Coeff, 		150, 20, TXT_COEFF,  		 	 fullHight,0,v.COLOR_BkScreen,v.FONT_COLOR_Coeff, 			255,1,v.COLOR_BkScreen);
	LCD_StrDependOnColorsVar	  (v.FONT_VAR_LenWin,		 v.FONT_ID_LenWin,		400,  0, TXT_LEN_WIN,		 	 halfHight,0,v.COLOR_BkScreen,v.FONT_COLOR_LenWin,			255,1,v.COLOR_BkScreen);
	LCD_StrDependOnColorsVar	  (v.FONT_VAR_OffsWin,	  	 v.FONT_ID_OffsWin,	   400, 20, TXT_OFFS_WIN,	    	 halfHight,0,v.COLOR_BkScreen,v.FONT_COLOR_OffsWin,		255,1,v.COLOR_BkScreen);
	LCD_StrDependOnColorsVar	  (v.FONT_VAR_LoadFontTime, v.FONT_ID_LoadFontTime,320, 20, TXT_LOAD_FONT_TIME,	 halfHight,0,v.COLOR_BkScreen,v.FONT_COLOR_LoadFontTime, 255,1,v.COLOR_BkScreen);
	LCD_StrDependOnColorsDescrVar(v.FONT_VAR_PosCursor,	 v.FONT_ID_PosCursor,	440, 40, "T: ",TXT_PosCursor(),halfHight,0,v.COLOR_BkScreen,v.FONT_COLOR_PosCursor,		255,1,v.COLOR_BkScreen);
	LCD_StrDependOnColorsVar	  (v.FONT_VAR_CPUusage,	  	 v.FONT_ID_CPUusage,	 	450, 0, 	TXT_CPU_USAGE,	 		 halfHight,0,v.COLOR_BkScreen,v.FONT_COLOR_CPUusage,		255,1,v.COLOR_BkScreen);

	 Test.yFontsField=LCD_Ypos(lenStr,IncPos,5);
	 LCD_Ymiddle(SetPos, Test.yFontsField|(LCD_GetYSize()-2)<<16 );
	 LCD_Xmiddle(SetPos, Test.xFontsField|LCD_GetXSize()<<16,"",0,NoConstWidth);

	StartMeasureTime_us();    //daj mozliwosc wpisywania dowolnego textu aby korygowac odstepy miedzy kazdymi fontami jakimi sie chce !!!!!!!
	if(Test.type)
		lenStr=LCD_StrVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,v.COLOR_BkScreen,0,Test.constWidth,v.COLOR_BkScreen);
	else
		lenStr=LCD_StrChangeColorVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, LCD_Xmiddle(GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth), LCD_Ymiddle(GetPos,v.FONT_ID_Fonts), Test.txt, fullHight, Test.spaceBetweenFonts,RGB_BK,RGB_FONT,Test.coeff,Test.constWidth,v.COLOR_BkScreen);
	Test.speed=StopMeasureTime_us("");

	LCD_StrDependOnColorsVar(v.FONT_VAR_Speed,v.FONT_ID_Speed,450,0,TXT_SPEED,fullHight,0,v.COLOR_BkScreen,v.FONT_COLOR_Speed,255,1,v.COLOR_BkScreen);

	LCD_Show();
}
