
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

#include "FreeRTOS.h"
#include "task.h"

/*----------------- Main Settings ------------------*/
#define FILE_NAME(extend) SCREEN_Fonts_##extend

#define SCREEN_FONTS_LANG \
	X(LANG_nazwa_0, "Czcionki LCD", "Fonts LCD") \
	X(LANG_nazwa_1, "Zmiana kolor"ó"w czcionki", "Press to change color fonts") \
	X(LANG_nazwa_2, "1.", "1.") \
	X(LANG_nazwa_3, "Czer", "Red") \
	X(LANG_nazwa_4, "Ziel", "Green") \
	X(LANG_nazwa_5, "Nieb", "Blue") \
	X(LANG_nazwa_6, "Zmiana kolor"ó"w t"ł"a", "Press to change color fonts background") \
	X(LANG_nazwa_7, "2.", "2.") \
	X(LANG_nazwa_8, "Klawiatura RGB", "Keyboard RGB") \
	X(LANG_FontTypeAbove, "Zmiana typu czcionki", "Press to change type fonts") \
	X(LANG_FontTypeLeft, "3.", "3.") \
	X(LANG_FontTypeUnder, "nacisnij przez 1 sekunde", "press by 1 second") \
	X(LANG_FontSizeAbove, "Zmiana rozmiaru czcionki", "Press to change size fonts") \
	X(LANG_FontSizeLeft, 	 "4.", "4.") \
	X(LANG_FontSizeUnder, 	 "---", "---") \
	X(LANG_FontStyleAbove, "Zmiana stylu czcionki", "Press to change style fonts") \
	X(LANG_FontStyleLeft, 	 "5.", "5.") \
	X(LANG_FontStyleUnder, 	 "Nazwa stylu pod czcionk"ą"", "---2") \
	X(LANG_FontCoeffAbove, "Wsp"ó""ł"czynnik", "Coefficient") \
	X(LANG_FontCoeffLeft, 	 "6.", "6.") \
	X(LANG_FontCoeffUnder, 	 "naci"ś"nij", "press me") \
	X(LANG_CoeffKeyName, "Wsp"ó""ł"", "Coeff") \
	X(LANG_LenOffsWin1, "Okre"ś"lenie odst"ę"p"ó"w pom"ę"dzy literami", "Specifying the spacing between letters") \
	X(LANG_LenOffsWin2, "Przesuwanie tekstu, zmiana pozycji kursora i zapis zmian", "Moving text, changing cursor position, editing and saving changes") \
	X(LANG_LenOffsWin3, "Szeroko"ś""ć" tekstu", "xxxxxxx") \
	X(LANG_LenOffsWin4, "i jego przesuni"ę"cie", "xxxxxxx") \
	X(LANG_TimeSpeed1, "Czas za"ł"adowania czcionek", "xxxxxxx") \
	X(LANG_TimeSpeed2, "i szybko"ś""ć" wy"ś"wietlenia", "xxxxxxx") \
	X(LANG_WinInfo, "Zmiany odst"ę"p"ó"w mi"ę"dzy literami zosta"ł"y zapisane", "xxxxxxx") \
	X(LANG_WinInfo2, "Reset wszystkich ustawie"ń" dla odst"ę"p"ó"w mi"ę"dzy literami", "xxxxxxx") \
	X(LANG_MainFrameType, "Zmie"ń" wygl"ą"d", "Change appearance") \


#define SCREEN_FONTS_SET_PARAMETERS \
/* id   name							default value */ \
	X(0, FONT_SIZE_Title, 	 		FONT_14_bold) \
	X(1, FONT_SIZE_FontColor, 	 	FONT_14) \
	X(2, FONT_SIZE_BkColor,			FONT_14) \
	X(3, FONT_SIZE_FontType,		FONT_14) \
	X(4, FONT_SIZE_FontSize,		FONT_14) \
	X(5, FONT_SIZE_FontStyle,		FONT_14) \
	X(6, FONT_SIZE_Coeff,			FONT_14) \
	X(7, FONT_SIZE_LenWin,			FONT_14) \
	X(8, FONT_SIZE_OffsWin,			FONT_10) \
	X(9, FONT_SIZE_LoadFontTime,	FONT_10) \
	X(10, FONT_SIZE_PosCursor,		FONT_10) \
	X(11, FONT_SIZE_CPUusage,		FONT_10) \
	X(12, FONT_SIZE_Speed,			FONT_10) \
	X(13, FONT_SIZE_Descr, 	 		FONT_12) \
	X(14, FONT_SIZE_Press, 	 		FONT_14) \
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
	X(45, FONT_COLOR_Descr, 		COLOR_GRAY(0x99)) \
	X(46, FONT_COLOR_Press, 		DARKRED) \
	X(47, FONT_COLOR_Fonts,  		0xFFE1A000) \
	\
	X(48, FONT_BKCOLOR_Title,  	 	MYGRAY2) \
	X(49, FONT_BKCOLOR_FontColor, 	MYGRAY2) \
	X(50, FONT_BKCOLOR_BkColor, 	 	MYGRAY2) \
	X(51, FONT_BKCOLOR_FontType,  	MYGRAY2) \
	X(52, FONT_BKCOLOR_FontSize,  	MYGRAY2) \
	X(53, FONT_BKCOLOR_FontStyle,  	MYGRAY2) \
	X(54, FONT_BKCOLOR_Coeff,  		MYGRAY2) \
	X(55, FONT_BKCOLOR_LenWin,  		MYGRAY2) \
	X(56, FONT_BKCOLOR_OffsWin,  		MYGRAY2) \
	X(57, FONT_BKCOLOR_LoadFontTime,	MYGRAY2) \
	X(58, FONT_BKCOLOR_PosCursor,		MYGRAY2) \
	X(59, FONT_BKCOLOR_CPUusage,		MYGRAY2) \
	X(60, FONT_BKCOLOR_Speed,			MYGRAY2) \
	X(61, FONT_BKCOLOR_Descr, 			MYGRAY2) \
	X(62, FONT_BKCOLOR_Press, 			WHITE) \
	X(63, FONT_BKCOLOR_Fonts,  		0x090440) \
	\
	X(64, COLOR_BkScreen,  			COLOR_GRAY(0x38)) \
	X(65, COLOR_MainFrame,  		COLOR_GRAY(0xD0)) \
	X(66, COLOR_FillMainFrame, 	COLOR_GRAY(0x31)) \
	X(67, COLOR_Frame,  				COLOR_GRAY(0xD0)) \
	X(68, COLOR_FillFrame, 			COLOR_GRAY(0x3B)) \
	X(69, COLOR_FramePress, 		COLOR_GRAY(0xBA)) \
	X(70, COLOR_FillFramePress,	COLOR_GRAY(0x60)) \
	X(71, DEBUG_ON,  	1) \
	X(72, BK_FONT_ROUND,  	1) \
	X(73, LANG_SELECT,  	Polish) \
	\
	X(74, FONT_ID_Title,			fontID_1) \
	X(75, FONT_ID_FontColor,	fontID_2) \
	X(76, FONT_ID_BkColor, 		fontID_3) \
	X(77, FONT_ID_FontType, 	fontID_4) \
	X(78, FONT_ID_FontSize, 	fontID_5) \
	X(79, FONT_ID_FontStyle,  	fontID_6) \
	X(80, FONT_ID_Coeff,  		fontID_7) \
	X(81, FONT_ID_LenWin,  		fontID_8) \
	X(82, FONT_ID_OffsWin,  	fontID_9) \
	X(83, FONT_ID_LoadFontTime,fontID_10) \
	X(84, FONT_ID_PosCursor,	fontID_11) \
	X(85, FONT_ID_CPUusage,		fontID_12) \
	X(86, FONT_ID_Speed,			fontID_13) \
	X(87, FONT_ID_Descr,  		fontID_14) \
	X(88, FONT_ID_Press,  		fontID_15) \
	X(89, FONT_ID_Fonts,  		fontID_16) \
	\
	X(90, FONT_VAR_Title,			fontVar_1) \
	X(91, FONT_VAR_FontColor,		fontVar_2) \
	X(92, FONT_VAR_BkColor, 		fontVar_3) \
	X(93, FONT_VAR_FontType, 		fontVar_4) \
	X(94, FONT_VAR_FontSize, 		fontVar_5) \
	X(95, FONT_VAR_FontStyle,  	fontVar_6) \
	X(96, FONT_VAR_Coeff,  			fontVar_7) \
	X(97, FONT_VAR_LenWin,  		fontVar_8) \
	X(98, FONT_VAR_OffsWin,  		fontVar_9) \
	X(99, FONT_VAR_LoadFontTime,	fontVar_10) \
	X(100, FONT_VAR_PosCursor,		fontVar_11) \
	X(101, FONT_VAR_CPUusage,		fontVar_12) \
	X(102, FONT_VAR_Speed,			fontVar_13) \
	X(103, FONT_VAR_Fonts,  		fontVar_14) \
	X(104, FONT_VAR_Press,  		fontVar_15) \
/*------------ End Main Settings -----------------*/

/*------------ Main Screen MACROs -----------------*/
#define SL(name)	(char*)FILE_NAME(Lang)[ v.LANG_SELECT==Polish ? 2*(name) : 2*(name)+1 ]

typedef enum{
	#define X(a,b,c) a,
		SCREEN_FONTS_LANG
	#undef X
}FILE_NAME(Lang_enum);

static const char *FILE_NAME(Lang)[]={
	#define X(a,b,c) b"\x00",c"\x00",
		SCREEN_FONTS_LANG
	#undef X
};

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
static uint32_t FILE_NAME(SelBits)[SEL_BITS_SIZE] = {0};
static uint32_t FILE_NAME(SelTouch)[SEL_BITS_SIZE] = {0};
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
		#define X(a,b,c) DbgVar2(1,200,CoGr_"%*d"_X	"%*s" 	CoGr_"= "_X	 	"%*s" 	"(%s0x%x)\r\n",-4,a,		-23,getName(b),	-15,getName(c), 	CHECK_bit( FILE_NAME(SelBits)[a/32], (a-32*(a/32)) )?CoR_"change to: "_X:"", v.b);
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
	SET_bit( FILE_NAME(SelBits)[offs/32], (offs-32*(offs/32)) );
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

#define TEXT_TO_SHOW		"Rafa"ł" Markielowski"

#define nrMIDDLE_TXT	19
#define POS_X_TXT		LCD_Xmiddle(nrMIDDLE_TXT,GetPos,v.FONT_ID_Fonts,Test.txt,Test.spaceBetweenFonts,Test.constWidth)
#define POS_Y_TXT		LCD_Ymiddle(nrMIDDLE_TXT,GetPos,v.FONT_ID_Fonts)

#define FLOAT2STR(val)	Float2Str(val,' ',4,Sign_plusMinus,1)
#define INT2STR(val)		  Int2Str(val,'0',3,Sign_none)
#define INT2STR_TIME(val) Int2Str(val,' ',6,Sign_none)
#define ONEBIT(val)	     Int2Str(val,None,1,Sign_none)

#define TXT_FONT_COLOR 	StrAll(7," ",INT2STR(Test.font[0])," ",INT2STR(Test.font[1])," ",INT2STR(Test.font[2])," ")
#define TXT_BK_COLOR 	StrAll(7," ",INT2STR(Test.bk[0]),  " ",INT2STR(Test.bk[1]),  " ",INT2STR(Test.bk[2])," ")
#define TXT_FONT_TYPE	StrAll(5," ",ONEBIT(Test.type+1)," ",LCD_FontType2Str(bufTemp,0,Test.type+1)+1," ")
#define TXT_FONT_SIZE	StrAll(3," ",LCD_FontSize2Str(bufTemp+25,Test.size)," ")
#define TXT_FONT_STYLE	StrAll(3," ",LCD_FontStyle2Str(bufTemp,Test.style)," ")
#define TXT_COEFF			StrAll(3," ",Int2Str(Test.coeff,Space,3,Sign_plusMinus)," ")
#define TXT_LEN_WIN		Int2Str(Test.lenWin ,' ',3,Sign_none)
#define TXT_OFFS_WIN		Int2Str(Test.offsWin,' ',3,Sign_none)
#define TXT_LENOFFS_WIN StrAll(5," ",TXT_LEN_WIN," ",TXT_OFFS_WIN," ")
#define TXT_TIMESPEED 			StrAll(4,Int2Str(Test.loadFontTime,' ',5,Sign_none)," ms   ",Int2Str(Test.speed,' ',6,Sign_none)," us")
#define TXT_CPU_USAGE		   StrAll(2,INT2STR(osGetCPUUsage()),"c")

#define RGB_FONT 	RGB2INT(Test.font[0],Test.font[1],Test.font[2])
#define RGB_BK    RGB2INT(Test.bk[0],  Test.bk[1],  Test.bk[2])

#define CHECK_TOUCH(state)		CHECK_bit(FILE_NAME(SelTouch)[state/32],(state-32*(state/32)-1))
#define SET_TOUCH(state) 		SET_bit(FILE_NAME(SelTouch)[state/32],(state-32*(state/32)-1))
#define CLR_TOUCH(state) 		RST_bit(FILE_NAME(SelTouch)[state/32],(state-32*(state/32)-1))
#define CLR_ALL_TOUCH 			for(int i=0;i<SEL_BITS_SIZE;++i) FILE_NAME(SelTouch)[i]=0
#define GET_TOUCH 				FILE_NAME(SelTouch)[0]!=0 || FILE_NAME(SelTouch)[1]!=0 || FILE_NAME(SelTouch)[2]!=0 || FILE_NAME(SelTouch)[3]!=0 || FILE_NAME(SelTouch)[4]!=0		/* determine by 'SEL_BITS_SIZE' */

#define NONE_TYPE_REQ	-1
#define MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY		20
/* #define TOUCH_MAINFONTS_WITHOUT_DESCR */

#define SELECT_CURRENT_FONT(src,dst,txt,coeff) \
	LCD_SetStrVar_fontID		(v.FONT_VAR_##src, v.FONT_ID_##dst);\
	LCD_SetStrVar_fontColor	(v.FONT_VAR_##src, v.FONT_COLOR_##dst);\
	LCD_SetStrVar_bkColor  	(v.FONT_VAR_##src, v.FONT_BKCOLOR_##dst);\
	LCD_SetStrVar_coeff		(v.FONT_VAR_##src, coeff);\
	LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_##src, txt)

#define ROLL_1		0

typedef enum{
	NoTouch,
	Touch_FontColor,
	Touch_FontColor2,
	Touch_BkColor,
	Touch_FontType,
	Touch_FontType2,
	Touch_FontSize,
	Touch_FontSize2,
	Touch_FontSizeMove,
	Touch_FontStyle,
	Touch_FontStyle2,
	Touch_FontCoeff,
	Touch_FontLenOffsWin,
	Touch_fontRp,
	Touch_fontGp,
	Touch_fontBp,
	Touch_fontRm,
	Touch_fontGm,
	Touch_fontBm,
	Touch_fontSliderR_left,
	Touch_fontSliderR,
	Touch_fontSliderR_right,
	Touch_fontSliderG_left,
	Touch_fontSliderG,
	Touch_fontSliderG_right,
	Touch_fontSliderB_left,
	Touch_fontSliderB,
	Touch_fontSliderB_right,
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
	Touch_FontSizeRoll,
	Touch_coeff_plus,
	Touch_coeff_minus,
	Touch_LenWin_plus,
	Touch_LenWin_minus,
	Touch_OffsWin_plus,
	Touch_OffsWin_minus,
	Touch_PosInWin_plus,
	Touch_PosInWin_minus,
	Touch_SpaceFonts_plus,
	Touch_SpaceFonts_minus,
	Touch_DispSpaces,
	Touch_WriteSpaces,
	Touch_ResetSpaces,
	Touch_SpacesInfoUp,
	Touch_SpacesInfoDown,
	Touch_MainFramesType,
	Touch_SetTxt,
	Move_1,
	Move_2,
	Move_3,
	Point_1,
	AnyPress,
	AnyPressWithWait,
	Touch_Q,Touch_W,Touch_E,Touch_R,Touch_T,Touch_Y,Touch_U,Touch_I,Touch_O,Touch_P,Touch_A,Touch_S,Touch_D,Touch_F,Touch_G,Touch_H,Touch_J,Touch_K,TOouch_L,Touch_big,Touch_Z,Touch_X,Touch_C,Touch_V,Touch_B,Touch_N,Touch_M,Touch_back,Touch_alt,Touch_exit,Touch_space,Touch_comma,Touch_dot,Touch_enter
}TOUCH_POINTS;		/* MAX_OPEN_TOUCH_SIMULTANEOUSLY */

typedef enum{
	KEYBOARD_none,
	KEYBOARD_fontRGB,
	KEYBOARD_bkRGB,
	KEYBOARD_fontSize,
	KEYBOARD_fontSize2,
	KEYBOARD_fontType,
	KEYBOARD_fontStyle,
	KEYBOARD_fontCoeff,
	KEYBOARD_LenOffsWin,
	KEYBOARD_sliderRGB,
	KEYBOARD_sliderBkRGB,
	KEYBOARD_setTxt,
}KEYBOARD_TYPES;	/* MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY */

typedef enum{
	KEY_All_release,
	KEY_Select_one,
	KEY_All_release_and_select_one,
	KEY_Timer,

	KEY_Red_plus,
	KEY_Green_plus,
	KEY_Blue_plus,
	KEY_Red_minus,
	KEY_Green_minus,
	KEY_Blue_minus,

	KEY_Red_slider,
	KEY_Green_slider,
	KEY_Blue_slider,

	KEY_Style_1,
	KEY_Style_2,
	KEY_Style_3,

	KEY_Size_plus,
	KEY_Size_minus,
	KEY_Size_norm,
	KEY_Size_bold,
	KEY_Size_italic,

	KEY_Coeff_plus,
	KEY_Coeff_minus,

	KEY_LenWin_plus,
	KEY_LenWin_minus,
	KEY_OffsWin_plus,
	KEY_OffsWin_minus,
	KEY_PosInWin_plus,
	KEY_PosInWin_minus,
	KEY_SpaceFonts_plus,
	KEY_SpaceFonts_minus,
	KEY_DispSpaces,
	KEY_WriteSpaces,
	KEY_ResetSpaces,
	KEY_InfoSpacesUp,
	KEY_InfoSpacesDown,

	KEY_Q,KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,KEY_O,KEY_P,KEY_A,KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,KEY_L,KEY_big,KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,KEY_back,KEY_alt,KEY_exit,KEY_space,KEY_comma,KEY_dot,KEY_enter,

}SELECT_PRESS_BLOCK;

typedef enum{
	KeysAutoSize,		/* Keys auto size to fontPress size*/
	KeysNotDel,
	KeysDel
}KEYBOARD_ANOTHER_PARAM;

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

typedef enum{	//takie ogolen funkcje umiesci w osobnym ogolnym pliku !!!!
	LoadWholeScreen,
	LoadPartScreen,
	LoadUserScreen,
	LoadNoDispScreen
}LOAD_SCREEN;

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
} RGB_BK_FONT;
static RGB_BK_FONT Test;

static void FRAMES_GROUP_combined(int argNmb, int startOffsX,int startOffsY, int offsX,int offsY,  int bold);
static void FRAMES_GROUP_separat(int argNmb, int startOffsX,int startOffsY, int offsX,int offsY,  int boldFrame);

static int incH = 30;  ///do usuniecia !!!
static int incW = 50;
static int incP = 50;

extern uint32_t pLcd[];

static int *ppMain[7] = {(int*)FRAMES_GROUP_combined,(int*)FRAMES_GROUP_separat,(int*)"Rafal", (int*)&Test, &incH, &incW, &incP };
/*
static char* TXT_PosCursor(void){
	return Test.posCursor>0 ? Int2Str(Test.posCursor-1,' ',3,Sign_none) : StrAll(1,"off");
}
*/
static void ClearCursorField(void){
	LCD_ShapeIndirect(LCD_GetStrVar_x(v.FONT_VAR_Fonts),LCD_GetStrVar_y(v.FONT_VAR_Fonts)+LCD_GetFontHeight(v.FONT_ID_Fonts)+Test.spaceCoursorY,LCD_Rectangle, lenStr.inPixel,Test.heightCursor, v.COLOR_BkScreen,v.COLOR_BkScreen,v.COLOR_BkScreen);
}
static void TxtTouch(TOUCH_SET_UPDATE type){
	switch((int)type){
		case TouchSetNew:
			LCD_TOUCH_DeleteSelectTouch(Touch_SetTxt);
			LCDTOUCH_Set(POS_X_TXT, POS_Y_TXT, lenStr.inPixel, lenStr.height, ID_TOUCH_POINT,Touch_SetTxt,press);
			break;
		case TouchUpdate:
			LCDTOUCH_Update(POS_X_TXT, POS_Y_TXT, lenStr.inPixel, lenStr.height, ID_TOUCH_POINT,Touch_SetTxt,press);
			break;
	}
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
#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
		SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontType,0, lenStr);
#endif
		break;
	case PARAM_SIZE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontSize,TXT_FONT_SIZE);
#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
		SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontSize,0, lenStr);
#endif
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
			   TxtTouch(TouchUpdate);
			}
			else{
				LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
				LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,MYGRAY);
				LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,0);
				StartMeasureTime_us();
				lenStr=LCD_StrVarIndirect(v.FONT_VAR_Fonts,Test.txt);
				Test.speed=StopMeasureTime_us("");
			   TxtTouch(TouchUpdate);
			}
			break;
		case 1:
			LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
			LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,RGB_BK);
			LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,Test.coeff);
			StartMeasureTime_us();
			lenStr=LCD_StrVarIndirect(v.FONT_VAR_Fonts,Test.txt);
		   Test.speed=StopMeasureTime_us("");
		   TxtTouch(TouchUpdate);
		   break;
		case 2:
			LCD_SetStrVar_fontID(v.FONT_VAR_Fonts,v.FONT_ID_Fonts);
			LCD_SetStrVar_bkColor(v.FONT_VAR_Fonts,WHITE);
			Test.coeff=0; LCD_SetStrVar_coeff(v.FONT_VAR_Fonts,0);
			StartMeasureTime_us();
			lenStr=LCD_StrVarIndirect(v.FONT_VAR_Fonts,Test.txt);
		   Test.speed=StopMeasureTime_us("");
		   TxtTouch(TouchUpdate);
			break;
		}
		break;
	case PARAM_COLOR_BK:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_BkColor,TXT_BK_COLOR);
		break;
	case PARAM_COLOR_FONT:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontColor,TXT_FONT_COLOR);
		break;
	case PARAM_OFFS_WINDOW:
	case PARAM_LEN_WINDOW:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_LenWin, TXT_LENOFFS_WIN);
		break;
	case PARAM_STYLE:
		lenStr=LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontStyle, TXT_FONT_STYLE);
#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
		SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontStyle,0, lenStr);
#endif
		break;
	case PARAM_COEFF:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_Coeff,TXT_COEFF);
		break;
	case PARAM_SPEED:
	case PARAM_LOAD_FONT_TIME:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_LoadFontTime, TXT_TIMESPEED);
		break;
/*	case PARAM_POS_CURSOR:
		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_PosCursor,TXT_PosCursor());
		break; */


//	case PARAM_CPU_USAGE:
//		LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_CPUusage,TXT_CPU_USAGE);
//		break;
	}
}
/*
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
*/
static void RefreshValRGB(void){
	Data2Refresh(FONTS);
	Data2Refresh(PARAM_COLOR_FONT);
	Data2Refresh(PARAM_COLOR_BK);
	Data2Refresh(PARAM_SPEED);
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
	RefreshValRGB();
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
	Test.yFontsField=240;

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
	LCD_SetStrVar_x(v.FONT_VAR_Fonts,POS_X_TXT);
}
static void AdjustMiddle_Y(void){
	LCD_SetStrVar_y(v.FONT_VAR_Fonts,POS_Y_TXT);
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
	ClearCursorField();
	SetCursor();
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

static void IncFontSize(int8_t typeReq)
{
	int sizeLimit;

	if(typeReq > NONE_TYPE_REQ){
		Test.size = typeReq;
		if(0==(typeReq%3)) Test.normBoldItal = 0;
		else if(0==((typeReq-1)%3)) Test.normBoldItal = 1;
		else if(0==((typeReq-2)%3)) Test.normBoldItal = 2;
	}
	else
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
			Data2Refresh(FONTS);		/* RefreshAllParam(); */
		}
	}
}
//dla DEBUG ON wlaczyc i wylaczyc caly debug dla danego pliku !!!! uregulowac to !!!
static void DisplayFontsWithChangeColorOrNot(void){
	TOOGLE(Test.dispChangeColorOrNot);
	Data2Refresh(FONTS);		/* RefreshAllParam(); */
}

static void LCD_DrawMainFrame(figureShape shape, int directDisplay, uint8_t bold, uint16_t x,uint16_t y, uint16_t w,uint16_t h, int frameColor,int fillColor,int bkColor)// zastanowic czy nie dac to do BasicGraphic.c
{
	figureShape pShape[5] = {LCD_Rectangle, LCD_BoldRectangle, LCD_RoundRectangle, LCD_BoldRoundRectangle, LCD_LittleRoundRectangle};

	if(shape==pShape[1] || shape==pShape[3])
		frameColor = SetBold2Color(frameColor,bold);

	if(shape==pShape[2] || shape==pShape[3])
		Set_AACoeff_RoundFrameRectangle(0.55, 0.73);

	if(IndDisp==directDisplay)
		LCD_ShapeIndirect(x,y,shape,w,h,frameColor,fillColor,bkColor);
	else
		LCD_Shape(x,y,shape,w,h,frameColor,fillColor,bkColor);
}

#define KEYBOARD_TYPE(type,key)	KeyboardTypeDisplay(type,key,0,0,0,0,0,0,0,NoTouch,NoTouch,0)
#define KEYBOARD_TYPE_PARAM(type,key,a,b,c,d,e)	KeyboardTypeDisplay(type,key,0,0,a,b,c,d,e,NoTouch,NoTouch,0)

static int RR=0;

int KeyboardTypeDisplay(KEYBOARD_TYPES type, SELECT_PRESS_BLOCK selBlockPress, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther)
{
	int fontID 			= v.FONT_ID_Press;
	int fontID_descr	= v.FONT_ID_Descr;
	int frameColor 	= v.COLOR_Frame;
	int fillColor 		= v.COLOR_FillFrame;
	int framePressColor 	= v.COLOR_FramePress;
	int fillPressColor 	= v.COLOR_FillFramePress;
	int bkColor 		= v.COLOR_BkScreen;

	#define MIDDLE_NR		1
	#define GET_X(txt)	LCD_Xmiddle(MIDDLE_NR,GetPos,fontID,txt,0,NoConstWidth)
	#define GET_Y			LCD_Ymiddle(MIDDLE_NR,GetPos,fontID)

	int k = type-1;
	int frameColor_c[5]={0}, fillColor_c[5]={0}, bkColor_c[5]={0};
	uint16_t widthAll = 0, widthAll_c = 0;
	uint16_t heightAll = 0, heightAll_c = 0;

	static struct SETTINGS{
		figureShape shape;
		uint8_t bold;
		uint16_t x;
		uint16_t y;
		uint16_t widthKey;
		uint16_t heightKey;
		uint16_t widthKey2;
		uint16_t heightKey2;
		uint8_t interSpace;
		uint8_t forTouchIdx;
		uint8_t startTouchIdx;
		uint8_t nmbTouch;
		uint8_t param;
		uint16_t param2;
	}s[MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY]={0}, c={0};

	void _Str(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _Str_XYoffs(XY_Touch_Struct pos,int offsX,int offsY, const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+offsX, pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrLeft(const char *txt, XY_Touch_Struct pos, uint32_t color){
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[k].heightKey));
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+LCD_GetFontHeight(fontID)/2, GET_Y, (char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrDescr_Xmidd_Yoffs(XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color){
		LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(0,widthAll),NULL,0,NoConstWidth);
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),	pos.y+offsY, 	(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
	}
	void _StrDescr(XY_Touch_Struct pos, const char *txt, uint32_t color){
		LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(pos.x,widthAll),NULL,0,NoConstWidth);
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),		s[k].interSpace,		(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
	}
	void _StrPress(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _StrPressLeft(const char *txt, XY_Touch_Struct pos, uint32_t color){
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[k].heightKey));
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+10,GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _StrDisp(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindowIndirect(0, s[k].x, s[k].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrPressDisp(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindowIndirect(0, s[k].x, s[k].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _TxtPos(XY_Touch_Struct pos){
		LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[k].widthKey),NULL,0,NoConstWidth);
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[k].heightKey));
	}
	void _Key(XY_Touch_Struct pos){
		LCD_ShapeWindow( s[k].shape, 0, widthAll,heightAll, pos.x,pos.y, s[k].widthKey,s[k].heightKey, SetBold2Color(frameColor,s[k].bold),fillColor,bkColor);
	}
	void _KeyStr(XY_Touch_Struct pos,const char *txt, uint32_t color){
		_Key(pos);
		_TxtPos(pos);
		_Str(txt,color);
	}
	void _KeyStrleft(XY_Touch_Struct pos,const char *txt, uint32_t color){
		_Key(pos);
		_StrLeft(txt,pos,color);
	}
	void _KeyStrDisp(XY_Touch_Struct pos,const char *txt, uint32_t color){
		_Key(pos);
		_TxtPos(pos);
		_StrDisp(txt,color);
	}
	void _KeyPress(XY_Touch_Struct pos){
		LCD_ShapeWindow( s[k].shape, 0, widthAll,heightAll, pos.x,pos.y, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
	}
	void _KeyStrPress(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_TxtPos(pos);
		_StrPress(txt,colorTxt);
	}
	void _KeyStrPressLeft(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_StrPressLeft(txt,pos,colorTxt);
	}
	void _KeyStrPressDisp(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_TxtPos(pos);
		_StrPressDisp(txt,colorTxt);
	}
	void _KeyStrPressDisp_oneBlock(XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
		LCD_ShapeWindow( s[k].shape, 0, s[k].widthKey,s[k].heightKey, 0,0, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
		_TxtPos((XY_Touch_Struct){0});
		LCD_StrDependOnColorsWindowIndirect(0, s[k].x+pos.x, s[k].y+pos.y, s[k].widthKey, s[k].heightKey,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, colorTxt,255, NoConstWidth);
	}
	void _KeyShapePressDisp_oneBlock(XY_Touch_Struct pos, ShapeFunc pShape, SHAPE_PARAMS param){
		LCD_ShapeWindow( s[k].shape, 0, s[k].widthKey,s[k].heightKey, 0,0, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
		pShape(0,param);
		LCD_Display(0, s[k].x+pos.x, s[k].y+pos.y, s[k].widthKey, s[k].heightKey);
	}
	void _SetTouchSlider(uint16_t idx, SHAPE_PARAMS posElemSlider){
		for(int i=0; i<NMB_SLIDER_ELEMENTS; ++i){
			touchTemp[0].x= s[k].x + posElemSlider.pos[i].x;
			touchTemp[1].x= touchTemp[0].x + posElemSlider.size[i].w;
			touchTemp[0].y= s[k].y + posElemSlider.pos[i].y;
			touchTemp[1].y= touchTemp[0].y + posElemSlider.size[i].h;

			switch(i){ case 0: case 2: LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, idx + s[k].nmbTouch++, TOUCH_GET_PER_X_PROBE   ); break;
						  case 1:			LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT,				idx + s[k].nmbTouch++, TOUCH_GET_PER_ANY_PROBE ); break; }
		}
	}

	void _ElemSliderPressDisp_oneBlock(XY_Touch_Struct posSlider, uint32_t spaceTringLine, int selElem, uint32_t lineColor, uint32_t lineSelColor, int *pVal, int valType, int maxSlidVal, VOID_FUNCTION *pfunc){
		int value = 0;
		switch(valType){
			case PosX:		value = SetValType(CONDITION(0>x-s[k].x-s[k].interSpace,0,x-s[k].x-s[k].interSpace),PosX);  break;
			case Percent:	value = SetValType(PERCENT_SCALE(*pVal+1,maxSlidVal+1),Percent);  			  					  break;
		}
		LCD_ShapeWindow(LCD_Rectangle,0,s[k].widthKey,s[k].heightKey, 0,0, s[k].widthKey,s[k].heightKey, bkColor, bkColor,bkColor);
		SHAPE_PARAMS slid = LCD_SimpleSlider(0, s[k].widthKey, s[k].heightKey, 0,0, ChangeElemSliderSize(s[k].widthKey,NORMAL_SLIDER_PARAM), SetSpaceTriangLineSlider(s[k].heightKey,spaceTringLine), lineColor, lineSelColor ,selElem|0xFF000000, bkColor, value, selElem);
		LCD_Display(0, s[k].x+posSlider.x, s[k].y+posSlider.y, s[k].widthKey, s[k].heightKey);
		if(PtrSel==SHIFT_RIGHT(selElem,24,F))
			*pVal = SET_NEW_RANGE( ((maxSlidVal+1)*slid.param[0])/slid.param[1], slid.param[2],maxSlidVal-slid.param[2], 0,maxSlidVal );
		if(pfunc) pfunc();
	}
	void _SetTouch(uint16_t ID, uint16_t idx, uint8_t paramTouch, XY_Touch_Struct pos){
		touchTemp[0].x= s[k].x + pos.x;
		touchTemp[1].x= touchTemp[0].x + s[k].widthKey;
		touchTemp[0].y= s[k].y + pos.y;
		touchTemp[1].y= touchTemp[0].y + s[k].heightKey;
		LCD_TOUCH_Set(ID,idx,paramTouch);
		s[k].nmbTouch++;
	}

	void _deleteAllTouchs(void){
		for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
			for(int i=0; i<s[j].nmbTouch; ++i)
				LCD_TOUCH_DeleteSelectTouch(s[j].startTouchIdx+i);
		}
	}
	void _deleteAllTouchParams(void){
		for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
			s[j].forTouchIdx = NoTouch;
			s[j].nmbTouch = 0;
		}
	}
	void _deleteTouchs(void){
		for(int i=0; i<s[k].nmbTouch; ++i)
			LCD_TOUCH_DeleteSelectTouch(s[k].startTouchIdx+i);
	}
	void _deleteTouchParams(void){
		s[k].forTouchIdx = NoTouch;
		s[k].nmbTouch = 0;
	}
	int _startUp(void){
		if(KEYBOARD_none == type){
			_deleteAllTouchs();
			_deleteAllTouchParams();
			return 1;
		}
		if(shape!=0){
			if(KeysDel == eraseOther){
				_deleteAllTouchs();
				_deleteAllTouchParams();
			}
			else{
				_deleteTouchs();
				_deleteTouchParams();
			}

			s[k].shape = shape;
			s[k].bold = bold;
			s[k].x = x;
			s[k].y = y;
			s[k].widthKey = widthKey;
			s[k].heightKey = heightKey;
			s[k].interSpace = interSpace;
			s[k].forTouchIdx = forTouchIdx;
			s[k].startTouchIdx = startTouchIdx;
			s[k].nmbTouch = 0;
			s[k].param = 0;
			s[k].param2 = 0;
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

		if(shape!=0){
			if(KeysAutoSize == widthKey){
				s[k].widthKey =  heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[0],0,NoConstWidth) + heightKey;		/*	space + text + space */
				s[k].heightKey = heightKey + LCD_GetFontHeight(fontID) + heightKey;
			}
		}
		int head = s[k].interSpace + LCD_GetFontHeight(fontID_descr) + s[k].interSpace;

		XY_Touch_Struct posHead={0,0};
		XY_Touch_Struct posKey[]=
		  {{1*s[k].interSpace + 0*s[k].widthKey,	1*s[k].interSpace + 0*s[k].heightKey + head},
			{2*s[k].interSpace + 1*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
			{3*s[k].interSpace + 2*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
			\
			{1*s[k].interSpace + 0*s[k].widthKey,	2*s[k].interSpace + 1*s[k].heightKey + head},
			{2*s[k].interSpace + 1*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head},
			{3*s[k].interSpace + 2*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head}};

		int countKey = dimKeys[0]*dimKeys[1]; 		/* = STRUCT_TAB_SIZE(txtKey); */

		widthAll =  dimKeys[0]*s[k].widthKey  + (dimKeys[0]+1)*s[k].interSpace;
		heightAll = dimKeys[1]*s[k].heightKey + (dimKeys[1]+1)*s[k].interSpace + head;

		switch((int)selBlockPress)
		{
			case KEY_All_release:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
				_StrDescr(posHead, SL(LANG_nazwa_8), v.FONT_COLOR_Descr);

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
				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
		}
	}

	void _ServiceCoeff(void)
	{
		const char *txtKey[]								= {"+",	  "-"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,  WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,DARKRED};
		const uint16_t dimKeys[] = {2,1};

		if(shape!=0){
			if(KeysAutoSize == widthKey){
				s[k].widthKey =  heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[0],0,NoConstWidth) + heightKey;		/*	space + text + space */
				s[k].heightKey = heightKey + LCD_GetFontHeight(fontID) + heightKey;
			}
		}
		int head = s[k].interSpace + LCD_GetFontHeight(fontID_descr)/* + s[k].interSpace*/;

		XY_Touch_Struct posHead={0,0};
		XY_Touch_Struct posKey[]=
		  {{1*s[k].interSpace + 0*s[k].widthKey,	s[k].interSpace + head},
			{2*s[k].interSpace + 1*s[k].widthKey, 	s[k].interSpace + head}};

		int countKey = dimKeys[0]*dimKeys[1]; 		/* = STRUCT_TAB_SIZE(txtKey); */

		widthAll =  dimKeys[0]*s[k].widthKey  + (dimKeys[0]+1)*s[k].interSpace;
		heightAll = dimKeys[1]*s[k].heightKey + (dimKeys[1]+1)*s[k].interSpace + head;

		switch((int)selBlockPress)
		{
			case KEY_All_release:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
				_StrDescr(posHead, SL(LANG_CoeffKeyName), v.FONT_COLOR_Descr);

				fillColor = BrightIncr(fillColor,0xE);
				for(int i=0; i<countKey; ++i){
					i<countKey-1 ? _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(posKey[i],txtKey[i],colorTxtKey[i]);
				}
				break;

			case KEY_Coeff_plus:	  _KeyStrPressDisp_oneBlock(posKey[0], txtKey[0], colorTxtPressKey[0]);	break;
			case KEY_Coeff_minus:  _KeyStrPressDisp_oneBlock(posKey[1], txtKey[1], colorTxtPressKey[1]);	break;
		}

		if(startTouchIdx){
			for(int i=0; i<countKey; ++i)
				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
		}
	}

	void _ServiceLenOffsWin(void)
	{
		#define _NMB2KEY	8
		const char *txtKey[]								= {"(.......)", "(...)", "(.......)",	"(.......)", " _ ", " _ ", "|  |", "||", "Info spaces", "Write spaces", "Reset spaces"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,  WHITE,  WHITE,  WHITE,  WHITE,  WHITE,  WHITE,	 WHITE,		WHITE,		 WHITE,			  WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,  DARKRED,	DARKRED, 	 DARKRED,		  DARKRED};
		const uint16_t dimKeys[] = {4,3};

		int widthKeyCorrect = 0;
		int heightKeyCorrect = 5;
		if(shape!=0){
			if(KeysAutoSize == widthKey){
				s[k].widthKey  = heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[ 			 STRING_GetTheLongestTxt(_NMB2KEY-1,(char**)txtKey) 				],0,NoConstWidth) + heightKey -widthKeyCorrect;		/*	space + text + space */
				s[k].widthKey2 = heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[_NMB2KEY + STRING_GetTheLongestTxt(2,			(char**)(txtKey+_NMB2KEY)) ],0,NoConstWidth) + heightKey -widthKeyCorrect;
				s[k].heightKey = heightKey +heightKeyCorrect + LCD_GetFontHeight(fontID) + heightKey +heightKeyCorrect;

				int diff;
				if		 (0 < (diff = dimKeys[0]*(s[k].widthKey+s[k].interSpace) - dimKeys[1]*(s[k].widthKey2+s[k].interSpace)))		s[k].widthKey2 += diff 		 / dimKeys[1];
				else if( 0 > diff)																																s[k].widthKey  += ABS(diff) / dimKeys[0];
		}}

		int countKey = dimKeys[0]*dimKeys[1] - 1;
		int head = s[k].interSpace + 2*LCD_GetFontHeight(fontID_descr) + s[k].interSpace;
		int edgeSpace = s[k].interSpace + s[k].interSpace/2;

		int widthShape1 =  s[k].widthKey - s[k].widthKey/4;
		int heightShape1 = LCD_GetFontHeight(fontID) + LCD_GetFontHeight(fontID)/3;  /* s[k].heightKey - s[k].heightKey/3; */

		XY_Touch_Struct posHead={0, s[k].interSpace};
		XY_Touch_Struct posKey[]=
		  {{edgeSpace		 					 + 0*s[k].widthKey,	1*s[k].interSpace + 0*s[k].heightKey + head},
			{edgeSpace + 1*s[k].interSpace + 1*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
			{edgeSpace + 2*s[k].interSpace + 2*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
			{edgeSpace + 3*s[k].interSpace + 3*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
			\
			{edgeSpace		 					 + 0*s[k].widthKey,	2*s[k].interSpace + 1*s[k].heightKey + head},
			{edgeSpace + 1*s[k].interSpace + 1*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head},
			{edgeSpace + 2*s[k].interSpace + 2*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head},
			{edgeSpace + 3*s[k].interSpace + 3*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head},
			\
			{edgeSpace		 					 + 0*s[k].widthKey2,	3*s[k].interSpace + 2*s[k].heightKey + head + s[k].interSpace-s[k].interSpace/3},
			{edgeSpace + 1*s[k].interSpace + 1*s[k].widthKey2, 3*s[k].interSpace + 2*s[k].heightKey + head + s[k].interSpace-s[k].interSpace/3},
			{edgeSpace + 2*s[k].interSpace + 2*s[k].widthKey2, 3*s[k].interSpace + 2*s[k].heightKey + head + s[k].interSpace-s[k].interSpace/3}};

		widthAll 	=   dimKeys[0]	  *s[k].widthKey   + (dimKeys[0]+1-2)*s[k].interSpace + 2*edgeSpace;
		widthAll_c 	=  (dimKeys[0]-1)*s[k].widthKey2  + (dimKeys[0]+0-2)*s[k].interSpace + 2*edgeSpace;
		if(widthAll_c > widthAll)	widthAll = widthAll_c;
		heightAll = head + dimKeys[1]*s[k].heightKey + (dimKeys[1]+1-1)*s[k].interSpace + s[k].interSpace-s[k].interSpace/3 + edgeSpace;

		void 	_SetFlagWin	 (void){	SET_bit(s[k].param,7); }
		int 	_IsFlagWin	 (void){ return CHECK_bit(s[k].param,7); }
		void 	_RstFlagWin	 (void){	RST_bit(s[k].param,7); }

		POS_SIZE win = { .pos={ s[k].x+widthAll+15, s[k].y 				 }, .size={200,250} };
		POS_SIZE win2 ={ .pos={ 15, 					  s[k].y+heightAll+15 }, .size={600, 60} };

		LCD_DisplayRemeberedSpacesBetweenFonts(1,pCHAR_PLCD(0),(int*)(&win.size.w));
		win.size.w *= LCD_GetWholeStrPxlWidth(fontID_descr,(char*)"r",0,NoConstWidth);

		void _WinInfo(char* txt){
			BKCOPY_VAL(fillColor_c[0],fillColor,BrightIncr(fillColor,0x1A));
			LCD_ShapeWindow( s[k].shape, 0, win2.size.w, win2.size.h, 0,0, win2.size.w, win2.size.h, SetBold2Color(frameColor,s[k].bold), fillColor,bkColor );
			LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(0,win2.size.w),NULL,0,NoConstWidth);
			LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(0,win2.size.h));
			LCD_StrDependOnColorsWindowIndirect(0,MIDDLE(0,LCD_X,win2.size.w) /* win2.pos.x */, win2.pos.y, win2.size.w, win2.size.h,fontID, GET_X(txt),GET_Y,txt, fullHight, 0, fillColor, DARKRED,250, NoConstWidth);
			BKCOPY(fillColor,fillColor_c[0]);
			vTimerService(1,start_time,2000);
		}
		void _WindowSpacesInfo(uint16_t x,uint16_t y, uint16_t width,uint16_t height, int param){
			int spaceFromFrame = 10;
			int heightUpDown = 17;
			int widthtUpDown = 26;

			int xPosU = MIDDLE(0,		width/2,widthtUpDown);
			int xPosD = MIDDLE(width/2,width/2,widthtUpDown);
			int yPosUD = height-heightUpDown-spaceFromFrame;

			static uint16_t posTxt_temp=0;
			static uint16_t posTxtTab[50]={0};
			static uint16_t i_posTxtTab=0;

			void 		_SetCurrPosTxt(uint16_t pos){ s[k].param2=pos; }
			uint16_t _GetCurrPosTxt(void)			 { return s[k].param2; }

			if(NoDirect==param){
				touchTemp[0].x= win.pos.x + xPosU;
				touchTemp[1].x= touchTemp[0].x + widthtUpDown;
				touchTemp[0].y= win.pos.y + yPosUD;
				touchTemp[1].y= touchTemp[0].y + heightUpDown;
				LCD_TOUCH_Set(ID_TOUCH_POINT,Touch_SpacesInfoUp,press);
				s[k].nmbTouch++;

				touchTemp[0].x= win.pos.x + xPosD;
				touchTemp[1].x= touchTemp[0].x + widthtUpDown;
				touchTemp[0].y= win.pos.y + yPosUD;
				touchTemp[1].y= touchTemp[0].y + heightUpDown;
				LCD_TOUCH_Set(ID_TOUCH_POINT,Touch_SpacesInfoDown,press);
				s[k].nmbTouch++;

				_SetCurrPosTxt(0);
				i_posTxtTab=0;
			}
			else if(Up==param){
				CONDITION(1<i_posTxtTab && 0<posTxt_temp, i_posTxtTab-=2, i_posTxtTab--);
				_SetCurrPosTxt(posTxtTab[i_posTxtTab]);
			}

			LCD_ShapeWindow( s[k].shape, 0, width,height, 0,0, width,height, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
			posTxt_temp = LCD_TxtWin(0,width,height,fontID_descr,spaceFromFrame,spaceFromFrame,LCD_DisplayRemeberedSpacesBetweenFonts(1,pCHAR_PLCD(width*height),NULL)+_GetCurrPosTxt(),fullHight,0,fillColor,v.FONT_COLOR_Descr,250,NoConstWidth).inChar;
			if(posTxt_temp){
				_SetCurrPosTxt(_GetCurrPosTxt()+posTxt_temp);
				if(i_posTxtTab<sizeof(posTxtTab)-2)
					posTxtTab[++i_posTxtTab]= _GetCurrPosTxt();
			}

			LCD_TOUCH_SusspendTouch(Touch_SpacesInfoUp);
			LCD_TOUCH_SusspendTouch(Touch_SpacesInfoDown);

			if(1 < i_posTxtTab || (1==i_posTxtTab && 0==posTxt_temp)){
				LCDSHAPE_Window(LCDSHAPE_Arrow,0,LCD_Arrow(ToStructAndReturn,width,height, xPosU,yPosUD, SetLineBold2Width(widthtUpDown,7), SetTriangHeightCoeff2Height(heightUpDown,3), v.FONT_COLOR_Descr, v.FONT_COLOR_Descr, v.COLOR_BkScreen, Up));
				LCD_TOUCH_RestoreSusspendedTouch(Touch_SpacesInfoUp);
			}
			if(0 < posTxt_temp){
				LCD_Arrow(0,width,height, xPosD,yPosUD, SetLineBold2Width(widthtUpDown,7), SetTriangHeightCoeff2Height(heightUpDown-1,3), v.FONT_COLOR_Descr, v.FONT_COLOR_Descr, v.COLOR_BkScreen, Down);
				LCD_TOUCH_RestoreSusspendedTouch(Touch_SpacesInfoDown);
			}

			LCD_Display(0, x,y, width,height);
		}

		void _CreateWindows(int nr,int param){
			switch(nr){
				case 0:
					_WindowSpacesInfo(win.pos.x ,win.pos.y, win.size.w, win.size.h, param); _SetFlagWin();
					break;
				case 1:
					break;
			}
		}
		void _DeleteWindows(void){		/* Use function only after displaying (not during) */
			FILE_NAME(main)(LoadNoDispScreen,(char**)ppMain);	_RstFlagWin();  LCD_DisplayPart(0,win.pos.x ,win.pos.y, win.size.w, win.size.h); SELECT_CURRENT_FONT(LenWin,Press, TXT_LENOFFS_WIN,255);
			LCD_TOUCH_DeleteSelectTouch(Touch_SpacesInfoUp);
			LCD_TOUCH_DeleteSelectTouch(Touch_SpacesInfoDown);
			s[k].nmbTouch-=2;
		}
		void _OverArrowTxt(int nr, DIRECTIONS direct){
			LCD_ArrowTxt(0,widthAll,heightAll, MIDDLE(posKey[nr].x, s[k].widthKey, widthShape1), MIDDLE(posKey[nr].y, s[k].heightKey, heightShape1), widthShape1,heightShape1, frameColor,frameColor,fillColor, direct,fontID,(char*)txtKey[nr],colorTxtKey[nr]);
		}
		void _OverArrowTxt_oneBlockDisp(int nr, DIRECTIONS direct){
			LCD_ShapeWindow( s[k].shape, 0, s[k].widthKey,s[k].heightKey, 0,0, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
			LCD_ArrowTxt(0,s[k].widthKey,s[k].heightKey, MIDDLE(0, s[k].widthKey, widthShape1), MIDDLE(0, s[k].heightKey, heightShape1), widthShape1,heightShape1, colorTxtPressKey[nr],colorTxtPressKey[nr],fillPressColor, direct,fontID,(char*)txtKey[nr],colorTxtPressKey[nr]);
			LCD_Display(0, s[k].x+posKey[nr].x, s[k].y+posKey[nr].y, s[k].widthKey, s[k].heightKey);
			if(_IsFlagWin())	_DeleteWindows();
		}

		switch((int)selBlockPress)
		{
			case KEY_All_release:
				BKCOPY_VAL(frameColor_c[0],frameColor,WHITE);
					LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );		/* s[k].shape(0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s[k].bold), bkColor,bkColor); */
				BKCOPY(frameColor,frameColor_c[0]);
			/*	posHead.y = s[k].interSpace/2; */											_StrDescr_Xmidd_Yoffs(posHead, 0, SL(LANG_LenOffsWin1), v.FONT_COLOR_Descr);		/* _StrDescr(posHead, SL(LANG_LenOffsWin), v.FONT_COLOR_Descr); */
				posHead.y += LCD_GetFontHeight(fontID_descr)+s[k].interSpace/3;	_StrDescr_Xmidd_Yoffs(posHead, 0, SL(LANG_LenOffsWin2), v.FONT_COLOR_Descr);

				BKCOPY_VAL(fillColor_c[0],fillColor,BrightIncr(fillColor,0xE));
				for(int i=0; i<_NMB2KEY; ++i)
				{
					_Key(posKey[i]);
					switch(i){
						case 0: 	_OverArrowTxt(i,outside); 	  break;
						case 1: 	_OverArrowTxt(i,inside); 	  break;
						case 2: 	_OverArrowTxt(i,LeftLeft);   break;
						case 3: 	_OverArrowTxt(i,RightRight); break;
						case 4: 	_OverArrowTxt(i,LeftLeft);   break;
						case 5: 	_OverArrowTxt(i,RightRight); break;
						case 6: 	_OverArrowTxt(i,outside);	  break;
						case 7: 	_OverArrowTxt(i,inside); 	  break;
					}
				}
				BKCOPY(fillColor,fillColor_c[0]);

				BKCOPY_VAL(fillColor_c[0],fillColor,BrightIncr(fillColor,0x6));
				BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);
				for(int i=_NMB2KEY; i<countKey; ++i)
				{
					if(i<countKey-1){
						if(_IsFlagWin() && _NMB2KEY==i){		/* find key to hold press */
							BKCOPY_VAL(fillColor_c[1],fillColor,fillPressColor);
							BKCOPY_VAL(frameColor_c[0],frameColor,framePressColor);
							 _KeyStr(posKey[i],txtKey[i],colorTxtPressKey[i]);
							BKCOPY(fillColor,fillColor_c[1]);
							BKCOPY(frameColor,frameColor_c[0]);
						}
						else _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]);
					}
					else _KeyStrDisp(posKey[i],txtKey[i],colorTxtKey[i]);
				}
				BKCOPY(s[k].widthKey,c.widthKey);
				BKCOPY(fillColor,fillColor_c[0]);
				break;

			case KEY_LenWin_plus: 		_OverArrowTxt_oneBlockDisp(0,outside);	 break;
			case KEY_LenWin_minus: 		_OverArrowTxt_oneBlockDisp(1,inside);	 break;
			case KEY_OffsWin_plus:  	_OverArrowTxt_oneBlockDisp(2,LeftLeft);	 break;
			case KEY_OffsWin_minus: 	_OverArrowTxt_oneBlockDisp(3,RightRight);	 break;
			case KEY_PosInWin_plus:    _OverArrowTxt_oneBlockDisp(4,LeftLeft);	 break;
			case KEY_PosInWin_minus: 	_OverArrowTxt_oneBlockDisp(5,RightRight);	 break;
			case KEY_SpaceFonts_plus:  _OverArrowTxt_oneBlockDisp(6,outside);	 break;
			case KEY_SpaceFonts_minus: _OverArrowTxt_oneBlockDisp(7,inside);	 break;

			case KEY_DispSpaces:	 BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);  _KeyStrPressDisp_oneBlock(posKey[8],txtKey[8],colorTxtPressKey[8]);		BKCOPY(s[k].widthKey,c.widthKey); CONDITION(_IsFlagWin(),_DeleteWindows(),_CreateWindows(0,NoDirect));  break;
			case KEY_WriteSpaces: BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);  _KeyStrPressDisp_oneBlock(posKey[9],txtKey[9],colorTxtPressKey[9]);		BKCOPY(s[k].widthKey,c.widthKey); CONDITION(_IsFlagWin(),_DeleteWindows(),NULL);	 _WinInfo(SL(LANG_WinInfo));	 break;
			case KEY_ResetSpaces: BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);  _KeyStrPressDisp_oneBlock(posKey[10],txtKey[10],colorTxtPressKey[10]);	BKCOPY(s[k].widthKey,c.widthKey); CONDITION(_IsFlagWin(),_DeleteWindows(),NULL);  _WinInfo(SL(LANG_WinInfo2));  break;

			case KEY_InfoSpacesUp: 	 _CreateWindows(0,Up);    break;
			case KEY_InfoSpacesDown: _CreateWindows(0,Down);  break;

			case KEY_Timer: FILE_NAME(main)(LoadNoDispScreen,(char**)ppMain);  LCD_DisplayPart(0, MIDDLE(0,LCD_X,win2.size.w)/* win2.pos.x */, win2.pos.y, win2.size.w, win2.size.h);  break;
		}

		if(startTouchIdx){
			for(int i=0; i<_NMB2KEY; ++i)
				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);

			BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);
			 for(int i=_NMB2KEY; i<countKey; ++i)
				 _SetTouch(ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
			BKCOPY(s[k].widthKey,c.widthKey);
		}
		#undef _NMB2KEY
	}

	void _ServiceStyle(void)
	{
		const char *txtKey[]								= {"Arial", "Times_New_Roman", "Comic_Saens_MS"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,	  WHITE, 	  			WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,	  DARKRED, 				DARKBLUE};
		const uint16_t dimKeys[] = {1,3};

		XY_Touch_Struct posKey[]=
		  {{s[k].interSpace, 1*s[k].interSpace + 0*s[k].heightKey - 0},
			{s[k].interSpace, 2*s[k].interSpace + 1*s[k].heightKey - 1},
			{s[k].interSpace, 3*s[k].interSpace + 2*s[k].heightKey - 2}};

		int countKey = STRUCT_TAB_SIZE(txtKey);
		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s[k].widthKey  + (dimKeys[0]+1)*s[k].interSpace;
		heightAll = dimKeys[1]*s[k].heightKey + (dimKeys[1]+1)*s[k].interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );

				fillColor = 0xFF111111;

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
				_SetTouch(ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
		}
	}

	void _ServiceType(void)
	{
		const char *txtKey[]								= {"(Gray-Green)", "(Gray-White)", "(White-Black)"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,	  		 WHITE, 	  			WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {BLACK,	  		 BROWN, 				ORANGE};
		const uint16_t dimKeys[] = {1,3};

		XY_Touch_Struct posKey[]=
		  {{s[k].interSpace, 1*s[k].interSpace + 0*s[k].heightKey - 0},
			{s[k].interSpace, 2*s[k].interSpace + 1*s[k].heightKey - 1},
			{s[k].interSpace, 3*s[k].interSpace + 2*s[k].heightKey - 2}};

		int countKey = STRUCT_TAB_SIZE(txtKey);

		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s[k].widthKey  + (dimKeys[0]+1)*s[k].interSpace;
		heightAll = dimKeys[1]*s[k].heightKey + (dimKeys[1]+1)*s[k].interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );

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
				_SetTouch(ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
		}
	}

	void _ServiceSize(void)
	{
		const char *txtKey[]								= {"Size +",	"Size -"};
		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,	  	WHITE};
		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,		BLACK};
		const uint16_t dimKeys[] = {1,2};

		XY_Touch_Struct posHead={0,0};
		int head = s[k].interSpace + LCD_GetFontHeight(fontID_descr) + s[k].interSpace;

		XY_Touch_Struct posKey[]=
		  {{1*s[k].interSpace + 0*s[k].widthKey,	1*s[k].interSpace + 0*s[k].heightKey + head},
			{1*s[k].interSpace + 0*s[k].widthKey,	2*s[k].interSpace + 1*s[k].heightKey + head}};

		const char *txtKey2[]								= {"Normal", "Bold", "Italic"};
		const COLORS_DEFINITION colorTxtKey2[]			= {WHITE,	  WHITE,  WHITE};
		const COLORS_DEFINITION colorTxtPressKey2[]	= {BLACK,	  BROWN,  ORANGE};
		const uint16_t dimKeys2[] = {1,3};
		int ofs=s[k].interSpace + s[k].widthKey;
		XY_Touch_Struct posKey2[]=
		  {{s[k].interSpace+ofs, 1*s[k].interSpace + 0*s[k].heightKey - 0 + head},
			{s[k].interSpace+ofs, 1*s[k].interSpace + 1*s[k].heightKey - 1 + head},
			{s[k].interSpace+ofs, 1*s[k].interSpace + 2*s[k].heightKey - 2 + head}};

		int countKey = dimKeys[0]*dimKeys[1];
		int countKey2 = dimKeys2[0]*dimKeys2[1];

		widthAll =  (s[k].interSpace + s[k].widthKey + s[k].interSpace) + (s[k].widthKey + s[k].interSpace);
		heightAll = s[k].interSpace + dimKeys2[1]*s[k].heightKey + s[k].interSpace - (countKey2-1) + head;

		switch((int)selBlockPress)
		{
			case KEY_All_release_and_select_one:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
				_StrDescr(posHead, SL(LANG_nazwa_0), v.FONT_COLOR_Descr);

				for(int i=0; i<countKey; ++i)
					_KeyStr(posKey[i],txtKey[i],colorTxtKey[i]);

				c.shape = s[k].shape;	s[k].shape = LCD_Rectangle;
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
				s[k].shape = c.shape;
				break;

			case KEY_Size_plus:	 _KeyStrPressDisp_oneBlock(posKey[0], txtKey[0], colorTxtPressKey[0]);	break;
			case KEY_Size_minus:	 _KeyStrPressDisp_oneBlock(posKey[1], txtKey[1], colorTxtPressKey[1]);	break;
		}

		if(startTouchIdx){
			int touch_it=0;  // touch_it and  s[k].nmbTouch is the same  !!!!
			for(int i=0; i<countKey; ++i)
				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + touch_it++, TOUCH_GET_PER_X_PROBE, posKey[i]);

			for(int i=0; i<countKey2; ++i)
				_SetTouch(ID_TOUCH_POINT, s[k].startTouchIdx + touch_it++, press, posKey2[i]);
		}
	}

	void _ServiceSizeRoll(void)
	{
		#define _FRAME2ROLL

		const uint16_t dimKeys[] = {1,LCD_GetFontSizeMaxNmb()};
		const char *txtKey[dimKeys[1]];
		COLORS_DEFINITION colorTxtKey[dimKeys[1]];
		COLORS_DEFINITION colorTxtPressKey = DARKRED;
		XY_Touch_Struct posKey[dimKeys[1]];

		#ifdef _FRAME2ROLL
			XY_Touch_Struct posHead = {0,5};
			uint16_t spaceFrame2Roll = 10;
			int head = posHead.y + LCD_GetFontHeight(fontID_descr) + posHead.y;
		#endif

		if(shape!=0){
			if(KeysAutoSize == widthKey){
				s[k].widthKey =  heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)LCD_GetFontSizeStr(50),0,NoConstWidth) + heightKey;		/*	space + text + space */
				s[k].heightKey = heightKey + LCD_GetFontHeight(fontID) + heightKey;
			}
		}

		int frameNmbVis = 8;
		uint16_t roll = 0, roll_copy = 0;
		uint16_t selFrame = Test.size;

		int countKey = dimKeys[1];
		int win = frameNmbVis * s[k].heightKey - (frameNmbVis-1);
		int fillColor_copy = fillColor;

		if(shape!=0){
			if(dimKeys[1]-selFrame <= frameNmbVis/2)	roll = dimKeys[1]-frameNmbVis-1;
			else if(		  selFrame <= frameNmbVis/2)	roll = 0;
			else													roll = selFrame - frameNmbVis/2;

			roll_copy = roll;
			roll *= s[k].heightKey;
			roll -= roll_copy;
			LCD_TOUCH_ScrollSel_SetCalculate(ROLL_1, &roll, &selFrame, 0,0,0,countKey/frameNmbVis);
		}

		for(int i=0; i<countKey; ++i)
		{
			txtKey[i] = LCD_GetFontSizeStr(i);
			colorTxtKey[i] = COLOR_GRAY(0xDD);

			posKey[i].x = s[k].interSpace;
			posKey[i].y = (i+1)*s[k].interSpace + i*s[k].heightKey - i;
		}

		framePressColor = frameColor;
		widthAll  = dimKeys[0]*s[k].widthKey  + (dimKeys[0]+1)*s[k].interSpace;
		heightAll = dimKeys[1]*s[k].heightKey + (dimKeys[1]+1)*s[k].interSpace - (countKey-1);

		switch((int)selBlockPress)
		{
			case KEY_Select_one:
				if(shape!=0)
				{
					#ifdef _FRAME2ROLL
						BKCOPY_VAL(heightAll_c,heightAll,head+win+spaceFrame2Roll);
						BKCOPY_VAL(widthAll_c,widthAll,widthAll+2*spaceFrame2Roll);

						  LCD_ShapeWindow( LCD_RoundRectangle,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );

						  BKCOPY_VAL(c.interSpace,s[k].interSpace,posHead.y);
					 		 _StrDescr(posHead, SL(LANG_nazwa_0), v.FONT_COLOR_Descr);
					 	  BKCOPY(s[k].interSpace,c.interSpace);

					 	  LCD_Display(0, s[k].x-spaceFrame2Roll, s[k].y-head, widthAll, heightAll);

					 	BKCOPY(widthAll,widthAll_c);
					 	BKCOPY(heightAll,heightAll_c);
					#else
					 	LCD_ShapeWindow( LCD_RoundRectangle,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
					#endif

				}

				if(shape==0)
					LCD_TOUCH_ScrollSel_SetCalculate(ROLL_1, &roll, &selFrame, s[k].y, heightAll, s[k].heightKey, win);

				for(int i=0; i<countKey; ++i)
				{
					if(i == selFrame)
						_KeyStrPressLeft(posKey[i],txtKey[i],colorTxtPressKey);		/* _KeyStrPress(posKey[i],txtKey[i],colorTxtPressKey); */
					else{
						fillColor = (i%2) ? BrightDecr(fillColor_copy,0x20) : fillColor_copy;
						_KeyStrleft(posKey[i],txtKey[i],colorTxtKey[i]);				/*	_KeyStr(posKey[i],txtKey[i],colorTxtKey[i]); */
					}
				}
				LCD_Display(0 + roll * widthAll, s[k].x, s[k].y, widthAll, win);
				break;
		}

		if(startTouchIdx){
			touchTemp[0].x= s[k].x + posKey[0].x;
			touchTemp[1].x= touchTemp[0].x + s[k].widthKey;
			touchTemp[0].y= s[k].y + posKey[0].y;
			touchTemp[1].y= touchTemp[0].y + win;
			LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT, s[k].startTouchIdx, TOUCH_GET_PER_ANY_PROBE);
			s[k].nmbTouch++;
		}
	}

	void _ServiceSliderRGB(void)
	{
		const char *txtSliders[]							= {"Red","Green","Blue"};
		const COLORS_DEFINITION colorTxtSliders[]		= {RED, GREEN, BLUE};
		const uint16_t dimSlider[] = {1,3};

		uint32_t LineColor 	= frameColor;
		uint32_t LineSelColor = COLOR_GRAY(0x77);
		uint32_t spaceTriangLine = 11;	/* DelTriang */
		int maxSliderValue = 255;
		int *pValForSlider = (int*)(&Test.font[0]);
		VOID_FUNCTION *funcForSlider = RefreshValRGB;

		int head = s[k].interSpace + LCD_GetFontHeight(fontID_descr) + s[k].interSpace;
		XY_Touch_Struct posHead={0,0};
		XY_Touch_Struct posSlider[]=
		  {{1*s[k].interSpace,	1*s[k].interSpace + 0*s[k].heightKey + head},
			{1*s[k].interSpace,	2*s[k].interSpace + 1*s[k].heightKey + head},
			{1*s[k].interSpace, 	3*s[k].interSpace + 2*s[k].heightKey + head}};

		int countKey = STRUCT_TAB_SIZE(txtSliders);
		SHAPE_PARAMS elemSliderPos[countKey];

		widthAll =  dimSlider[0]*s[k].widthKey  + (dimSlider[0]+1)*s[k].interSpace;
		heightAll = dimSlider[1]*s[k].heightKey + (dimSlider[1]+1)*s[k].interSpace + head;

		switch((int)selBlockPress)
		{
			case KEY_All_release:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
				_StrDescr(posHead, SL(LANG_nazwa_1), v.FONT_COLOR_Descr);

				for(int i=0; i<countKey; ++i){
					elemSliderPos[i] = LCD_SimpleSlider(0, widthAll,heightAll, posSlider[i].x, posSlider[i].y, ChangeElemSliderSize(s[k].widthKey,NORMAL_SLIDER_PARAM), SetSpaceTriangLineSlider(s[k].heightKey,spaceTriangLine), LineColor, LineSelColor ,colorTxtSliders[i], bkColor, SetValType(PERCENT_SCALE(*(pValForSlider+i)+1,maxSliderValue+1),Percent), NoSel);
					_StrDescr_Xmidd_Yoffs(posSlider[i],- LCD_GetFontHeight(fontID_descr), txtSliders[i], colorTxtSliders[i]);
				}
				LCD_Display(0, s[k].x, s[k].y, widthAll, heightAll);
				break;

			case KEY_Red_slider:		_ElemSliderPressDisp_oneBlock(posSlider[0],spaceTriangLine, ChangeElemSliderColor(PtrSel,  colorTxtSliders[0]),LineColor,LineSelColor,pValForSlider+0, PosX,maxSliderValue,funcForSlider);	 break;
			case KEY_Green_slider:	_ElemSliderPressDisp_oneBlock(posSlider[1],spaceTriangLine, ChangeElemSliderColor(PtrSel,  colorTxtSliders[1]),LineColor,LineSelColor,pValForSlider+1, PosX,maxSliderValue,funcForSlider);	 break;
			case KEY_Blue_slider:	_ElemSliderPressDisp_oneBlock(posSlider[2],spaceTriangLine, ChangeElemSliderColor(PtrSel,  colorTxtSliders[2]),LineColor,LineSelColor,pValForSlider+2, PosX,maxSliderValue,funcForSlider);	 break;
			case KEY_Red_minus:		_ElemSliderPressDisp_oneBlock(posSlider[0],spaceTriangLine, ChangeElemSliderColor(LeftSel, colorTxtSliders[0]),LineColor,LineSelColor,pValForSlider+0, Percent,maxSliderValue,funcForSlider);	 break;
			case KEY_Red_plus:		_ElemSliderPressDisp_oneBlock(posSlider[0],spaceTriangLine, ChangeElemSliderColor(RightSel,colorTxtSliders[0]),LineColor,LineSelColor,pValForSlider+0, Percent,maxSliderValue,funcForSlider);	 break;
			case KEY_Green_minus:	_ElemSliderPressDisp_oneBlock(posSlider[1],spaceTriangLine, ChangeElemSliderColor(LeftSel, colorTxtSliders[1]),LineColor,LineSelColor,pValForSlider+1, Percent,maxSliderValue,funcForSlider);	 break;
			case KEY_Green_plus:		_ElemSliderPressDisp_oneBlock(posSlider[1],spaceTriangLine, ChangeElemSliderColor(RightSel,colorTxtSliders[1]),LineColor,LineSelColor,pValForSlider+1, Percent,maxSliderValue,funcForSlider);	 break;
			case KEY_Blue_minus:		_ElemSliderPressDisp_oneBlock(posSlider[2],spaceTriangLine, ChangeElemSliderColor(LeftSel, colorTxtSliders[2]),LineColor,LineSelColor,pValForSlider+2, Percent,maxSliderValue,funcForSlider);	 break;
			case KEY_Blue_plus:		_ElemSliderPressDisp_oneBlock(posSlider[2],spaceTriangLine, ChangeElemSliderColor(RightSel,colorTxtSliders[2]),LineColor,LineSelColor,pValForSlider+2, Percent,maxSliderValue,funcForSlider);	 break;
		}

		if(startTouchIdx){
			for(int i=0; i<countKey; ++i)
				_SetTouchSlider(s[k].startTouchIdx, elemSliderPos[i]);
		}
	}

	void _ServiceSetTxt(void) // i to daj kako zewn funkcje w jednym pliku includowanym !!!!!   klawiatura malutka i duza na caly LCD_X z liczbami
	{
		#define _UP		"|"
		#define _LF		"<--"
		#define _EN		"<-|"
		#define _EX		"X"

		const char *txtKey[]								= {"q","w","e","r","t","y","u","i","o","p", \
																	  "a","s","d","f","g","h","j","k","l", \
																	_UP,"z","x","c","v","b","n","m",_LF, \
																	"alt",_EX,"space",",",".",_EN };  //Sign 'S' to klawiatyra liczbnowa dla malej klawiatury

//		const char *txtKey2[]							= {"/","1","2","3", \
//																	"*","4","5","6", \
//																	"-","7","8","9", \
//																	"+","=",".","0" };

		const COLORS_DEFINITION colorTxtKey[]		= {WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE, \
																	WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE, \
																	WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE, \
																	WHITE,WHITE,WHITE,WHITE,WHITE,WHITE };

		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED, \
																	DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED, \
																	DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED, \
																	DARKRED,DARKRED,DARKRED,DARKRED,DARKRED,DARKRED };

		const uint8_t dimKeys[] = {10,9,9,6};

		#define _PARAM_ARROW_UP		structSize 	size_UP = { (35*s[k].widthKey)/100,  (2*s[k].heightKey)/5 };		int bold_UP = 1;		int coeff_UP = 3
		#define _PARAM_ARROW_LF		structSize 	size_LF = { ( 2*s[k].widthKey)/4,    (2*s[k].heightKey)/7 };		int bold_LF = 1;		int coeff_LF = 0
		#define _PARAM_ARROW_EN		structSize 	size_EN = { ( 2*s[k].widthKey)/4,    (2*s[k].heightKey)/7 };		int bold_EN = 1;		int coeff_EN = 0
		#define _PARAM_ARROW_EX		structSize 	size_EX = { (35*s[k].heightKey)/100, (35*s[k].heightKey)/100 };

		if(shape!=0){
			if(KeysAutoSize == widthKey){
				s[k].widthKey =  heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[0],0,NoConstWidth) + heightKey;
				s[k].heightKey = heightKey + LCD_GetFontHeight(fontID) + heightKey;
		}}

		int widthFieldTxt = 0;
		int heightFieldTxt = LCD_GetFontHeight(fontID) * 3;
		int head = s[k].interSpace + heightFieldTxt + s[k].interSpace;

		#define W1	s[k].widthKey
		#define WS	4*W1 + 3*s[k].interSpace
		#define WA	W1 + (W1 + s[k].interSpace)/2

		uint8_t widthKey[]= {W1,W1,W1,W1,W1,W1,W1,W1,W1,W1,\
									W1,W1,W1,W1,W1,W1,W1,W1,W1,\
									WA,W1,W1,W1,W1,W1,W1,W1,WA,\
									WA,W1,WS,W1,W1,WA};

		INIT(nr,0);		INIT(width_c,0);
		#define P(x,y,offs)	{(x+1)*s[k].interSpace + offs + CONDITION(0==x, width_c=_ReturnVal(0,CONDITION(0==y,nr=0,nr++)), width_c += widthKey[nr++]),		(y+1)*s[k].interSpace + y*s[k].heightKey + head}
		#define F	(s[k].widthKey+s[k].interSpace)/2

		XY_Touch_Struct posKey[]= {P(0,0,0),P(1,0,0),P(2,0,0),P(3,0,0),P(4,0,0),P(5,0,0),P(6,0,0),P(7,0,0),P(8,0,0),P(9,0,0),\
											P(0,1,F),P(1,1,F),P(2,1,F),P(3,1,F),P(4,1,F),P(5,1,F),P(6,1,F),P(7,1,F),P(8,1,F),\
											P(0,2,0),P(1,2,0),P(2,2,0),P(3,2,0),P(4,2,0),P(5,2,0),P(6,2,0),P(7,2,0),P(8,2,0),\
											P(0,3,0),P(1,3,0),P(2,3,0),P(3,3,0),P(4,3,0),P(5,3,0)};

		INIT_INCVAL(sizeof(dimKeys),countKey,dimKeys);
		INIT_MAXVAL(dimKeys,sizeof(dimKeys),0,maxVal);

		widthAll =  maxVal*s[k].widthKey  + (maxVal+1)*s[k].interSpace;
		heightAll = sizeof(dimKeys)*s[k].heightKey + (sizeof(dimKeys)+1)*s[k].interSpace + head;
		widthFieldTxt = widthAll - 2*s[k].interSpace;

		int colorFillBk = BrightDecr(bkColor,0x10);
		switch((int)selBlockPress)
		{
			case KEY_All_release:
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), colorFillBk,bkColor );
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, s[k].interSpace,s[k].interSpace, widthFieldTxt,heightFieldTxt, SetBold2Color(BrightDecr(frameColor,0x40),s[k].bold), BrightDecr(frameColor,0x40),colorFillBk );

				fillColor = BrightIncr(fillColor,0x10);
				bkColor = colorFillBk;

				c.widthKey = s[k].widthKey;
				for(int i=0; i<countKey; ++i)
				{
					s[k].widthKey = widthKey[i];
					if(STRING_CmpTxt((char*)txtKey[i],_UP)){	 		_Key(posKey[i]);	_PARAM_ARROW_UP;
						LCD_Arrow(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_UP.w),MIDDLE(posKey[i].y,s[k].heightKey,size_UP.h), SetLineBold2Width(size_UP.w,bold_UP), SetTriangHeightCoeff2Height(size_UP.h,coeff_UP), frameColor,frameColor,bkColor, Up);
					}
					else if(STRING_CmpTxt((char*)txtKey[i],_LF)){	_Key(posKey[i]);	_PARAM_ARROW_LF;
						LCD_Arrow(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_LF.w),MIDDLE(posKey[i].y,s[k].heightKey,size_LF.h), SetLineBold2Width(size_LF.w,bold_LF), SetTriangHeightCoeff2Height(size_LF.h,coeff_LF), frameColor,frameColor,bkColor, Left);
					}
					else if(STRING_CmpTxt((char*)txtKey[i],_EN)){	_Key(posKey[i]);	_PARAM_ARROW_EN;
						LCD_Enter(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_EN.w),MIDDLE(posKey[i].y,s[k].heightKey,size_EN.h), SetLineBold2Width(size_EN.w,bold_EN), SetTriangHeightCoeff2Height(size_EN.h,coeff_EN), frameColor,frameColor,bkColor);
						LCD_Display(0, s[k].x, s[k].y, widthAll, heightAll);
					}
					else if(STRING_CmpTxt((char*)txtKey[i],_EX)){	_KeyPress(posKey[i]);	_PARAM_ARROW_EX;
						LCD_Exit(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_EX.w),MIDDLE(posKey[i].y,s[k].heightKey,size_EX.h), size_EX.w, size_EX.h, colorTxtPressKey[i],colorTxtPressKey[i],bkColor);
					}
					else{
						if(i<dimKeys[0]){
							_Key(posKey[i]);
							char *ptrTxt = Int2Str(i,None,1,Sign_none);
							_Str_XYoffs(posKey[i], s[k].widthKey-LCD_GetWholeStrPxlWidth(fontID,ptrTxt,0,ConstWidth), 0,ptrTxt,colorTxtKey[i]);
							_StrDescr_Xmidd_Yoffs(posKey[i], s[k].heightKey-LCD_GetFontHeight(fontID), txtKey[i], colorTxtKey[i]);
						}
						else _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]);
					}
				}
				s[k].widthKey = c.widthKey;
				break;

			case KEY_big:
				nr = selBlockPress-KEY_Q;
				BKCOPY_VAL(c.widthKey,s[k].widthKey,widthKey[nr]);		_PARAM_ARROW_UP;
				_KeyShapePressDisp_oneBlock(posKey[nr], LCDSHAPE_Arrow, LCD_Arrow(ToStructAndReturn,s[k].widthKey,s[k].heightKey, MIDDLE(0,s[k].widthKey,size_UP.w),MIDDLE(0,s[k].heightKey,size_UP.h), SetLineBold2Width(size_UP.w,bold_UP), SetTriangHeightCoeff2Height(size_UP.h,coeff_UP), colorTxtPressKey[nr],colorTxtPressKey[nr],bkColor, Up));
				BKCOPY(s[k].widthKey,c.widthKey);
				break;

			case KEY_back:
				nr = selBlockPress-KEY_Q;
				BKCOPY_VAL(c.widthKey,s[k].widthKey,widthKey[nr]);		_PARAM_ARROW_LF;
				_KeyShapePressDisp_oneBlock(posKey[nr], LCDSHAPE_Arrow, LCD_Arrow(ToStructAndReturn,s[k].widthKey,s[k].heightKey, MIDDLE(0,s[k].widthKey,size_LF.w),MIDDLE(0,s[k].heightKey,size_LF.h), SetLineBold2Width(size_LF.w,bold_LF), SetTriangHeightCoeff2Height(size_LF.h,coeff_LF), colorTxtPressKey[nr],colorTxtPressKey[nr],bkColor, Left));
				BKCOPY(s[k].widthKey,c.widthKey);
				break;

			case KEY_enter:
				nr = selBlockPress-KEY_Q;
				BKCOPY_VAL(c.widthKey,s[k].widthKey,widthKey[nr]);		_PARAM_ARROW_EN;
				_KeyShapePressDisp_oneBlock(posKey[nr], LCDSHAPE_Enter, LCD_Enter(ToStructAndReturn,s[k].widthKey,s[k].heightKey, MIDDLE(0,s[k].widthKey,size_EN.w),MIDDLE(0,s[k].heightKey,size_EN.h), SetLineBold2Width(size_EN.w,bold_EN), SetTriangHeightCoeff2Height(size_EN.h,coeff_EN), colorTxtPressKey[nr],colorTxtPressKey[nr],bkColor));
				BKCOPY(s[k].widthKey,c.widthKey);
				break;

			default:
				if(IS_RANGE(selBlockPress,KEY_Q,KEY_enter)){ 	INIT(nr,selBlockPress-KEY_Q); BKCOPY_VAL(c.widthKey,s[k].widthKey,widthKey[nr]);   _KeyStrPressDisp_oneBlock(posKey[nr],txtKey[nr],colorTxtPressKey[nr]); 	BKCOPY(s[k].widthKey,c.widthKey); }
				break;
		}

		if(startTouchIdx){
			for(int i=0; i<countKey; ++i){
				BKCOPY_VAL(c.widthKey,s[k].widthKey,widthKey[i]);
				_SetTouch(ID_TOUCH_POINT,s[k].startTouchIdx+i,press,posKey[i]);
				BKCOPY(s[k].widthKey,c.widthKey);
		}}
		#undef P
		#undef F
		#undef W1
		#undef WS
		#undef WA
		#undef _UP
		#undef _LF
		#undef _EN
		#undef _EX
	}





	/* ----- End User Function Definitions ----- */

	switch((int)type)
	{
		case KEYBOARD_fontRGB:
		case KEYBOARD_bkRGB:
			_ServiceRGB();
			break;
		case KEYBOARD_fontCoeff:
			_ServiceCoeff();
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
		case KEYBOARD_fontSize2:
			_ServiceSizeRoll();
			break;
		case KEYBOARD_LenOffsWin:
			_ServiceLenOffsWin();
			break;
		case KEYBOARD_sliderRGB:
			_ServiceSliderRGB();
			break;
		case KEYBOARD_setTxt:
			_ServiceSetTxt();
			break;

		default:
			break;
	}
	#undef MIDDLE_NR
	#undef GET_X
	#undef GET_Y
	return 0;
}


//suwag okrągły !!!

//dla ROLL bez select a drugie pole touch to z select,

static void FILE_NAME(timer)(void)
{
	if(vTimerService(1,check_time,2000)){
		vTimerService(1,stop_time,2000);
		KEYBOARD_TYPE(KEYBOARD_LenOffsWin, KEY_Timer);
	}
}

void FILE_NAME(setTouch)(void)
{/*
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
				if(GET_TOUCH){ FILE_NAME(main)(LoadPartScreen,(char**)ppMain); CLR_ALL_TOUCH; }\
				SELECT_CURRENT_FONT(src, dst, txt, coeff);\
				SET_TOUCH(state);\
				SetFunc();\
			}\
			else{\
				FILE_NAME(main)(LoadPartScreen,(char**)ppMain);\
				KEYBOARD_TYPE(KEYBOARD_none,0);\
				CLR_TOUCH(state);\
			}

	#define _KEYS_RELEASE_fontRGB 		if(_WasStatePrev(Touch_fontRp,     Touch_fontBm)) 	 	 KEYBOARD_TYPE(KEYBOARD_fontRGB,   KEY_All_release)
	#define _KEYS_RELEASE_fontBk 			if(_WasStatePrev(Touch_fontRp,     Touch_bkBm)) 		 KEYBOARD_TYPE(KEYBOARD_bkRGB,     KEY_All_release)
	#define _KEYS_RELEASE_fontSize 		if(_WasStatePrev(Touch_size_plus,  Touch_size_minus))	 KEYBOARD_TYPE(KEYBOARD_fontSize,  KEY_All_release_and_select_one)
	#define _KEYS_RELEASE_fontCoeff 		if(_WasStatePrev(Touch_coeff_plus, Touch_coeff_minus)) KEYBOARD_TYPE(KEYBOARD_fontCoeff, KEY_All_release)
	#define _KEYS_RELEASE_fontSliderRGB if(_WasStatePrev(Touch_fontSliderR, Touch_fontSliderB_right)) KEYBOARD_TYPE(KEYBOARD_sliderRGB, KEY_All_release)
	#define _KEYS_RELEASE_LenOffsWin 	if(_WasStatePrev(Touch_LenWin_plus, Touch_ResetSpaces)) KEYBOARD_TYPE(KEYBOARD_LenOffsWin, KEY_All_release)
	#define _KEYS_RELEASE_setTxt 	if(_WasStatePrev(Touch_Q, Touch_enter)) KEYBOARD_TYPE(KEYBOARD_setTxt, KEY_All_release)

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
	int _WasStateRange(int point1, int point2){
		if(release==LCD_TOUCH_isPress() && IS_RANGE(statePrev,point1,point2)){
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

	int deltaForEndPress=0;

	state = LCD_TOUCH_GetTypeAndPosition(&pos);
	switch(state)
	{
		CASE_TOUCH_STATE(state,Touch_FontColor, FontColor,Press, TXT_FONT_COLOR,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontRGB, KEY_All_release, LCD_RoundRectangle,0, 230,160, KeysAutoSize,12, 4, state, Touch_fontRp,KeysDel); //dac wyrownanie ADJUTMENT to LEFT RIGHT TOP .....
			/* DisplayTouchPosXY(state,pos,"Touch_FontColor"); */
			break;

		CASE_TOUCH_STATE(state,Touch_FontColor2, FontColor,Press, TXT_FONT_COLOR,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_sliderRGB, KEY_All_release, LCD_RoundRectangle,0, 445,160, 200+100,46, 16, state, Touch_fontSliderR_left,KeysDel);
			break;

		CASE_TOUCH_STATE(state,Touch_BkColor, BkColor,Press, TXT_BK_COLOR,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_bkRGB, KEY_All_release, LCD_RoundRectangle,0, 400,160, KeysAutoSize,12, 4, state, Touch_bkRp,KeysDel);
			break;

		CASE_TOUCH_STATE(state,Touch_FontLenOffsWin, LenWin,Press, TXT_LENOFFS_WIN,252);
			if(IsFunc()){	KeyboardTypeDisplay(KEYBOARD_LenOffsWin, KEY_All_release, LCD_RoundRectangle,0, 0,0, KeysAutoSize,8, 10, state, Touch_LenWin_plus,KeysDel);
								LCDTOUCH_ActiveOnly(state,0,0,0,0,0,0,0,0,0,Touch_LenWin_plus,Touch_ResetSpaces); }
			else LCD_TOUCH_RestoreAllSusspendedTouchs();
			break;

		CASE_TOUCH_STATE(state,Touch_FontCoeff, Coeff,Press, TXT_COEFF,255);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontCoeff, KEY_All_release, LCD_RoundRectangle,0, 400,205, KeysAutoSize,10, 10, state, Touch_coeff_plus,KeysDel);
			break;

		CASE_TOUCH_STATE(state,Touch_FontStyle2, FontStyle,Press, TXT_FONT_STYLE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontStyle, KEY_Select_one, LCD_Rectangle,0, 400,160, 200,40, 0, state, Touch_style1,KeysDel);
			break;

		CASE_TOUCH_STATE(state,Touch_FontType2, FontType,Press, TXT_FONT_TYPE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontType, KEY_Select_one, LCD_Rectangle,0, 400,160, 200,40, 0, state, Touch_type1,KeysDel);
			break;

		CASE_TOUCH_STATE(state,Touch_FontSize2, FontSize,Press, TXT_FONT_SIZE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontSize, KEY_All_release_and_select_one, LCD_RoundRectangle,0, 410,170, 80,40, 6, state, Touch_size_plus,KeysDel);
			break;

		CASE_TOUCH_STATE(state,Touch_FontSizeMove, FontSize,Press, TXT_FONT_SIZE,252);
			if(IsFunc())
				KeyboardTypeDisplay(KEYBOARD_fontSize2, KEY_Select_one, LCD_Rectangle,0, 610,50, KeysAutoSize,10, 0, state, Touch_FontSizeRoll,KeysDel);
			else
				_SaveState();
			break;

		case Touch_SetTxt:
			KeyboardTypeDisplay(KEYBOARD_setTxt,KEY_All_release,LCD_RoundRectangle,0,15,15,KeysAutoSize,10,10,state,Touch_Q,KeysDel);
			LCDTOUCH_ActiveOnly(0,0,0,0,0,0,0,0,0,0,Touch_Q,Touch_enter);
			break;

		case Touch_FontStyle:
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

		case Touch_FontSize:		/* When 'Touch_FontSizeMove' was cleared then not service for release 'Touch_FontSize' */
			if(!CHECK_TOUCH(Touch_FontSizeMove) && !_WasState(Touch_FontSizeMove)){
				ChangeFontBoldItalNorm(NONE_TYPE_REQ);
				if(CHECK_TOUCH(Touch_FontSize2))
					KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one );
				_SaveState();
			}
			break;

		case Touch_fontRp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','R', 1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Red_plus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontGp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','G', 1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Green_plus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontBp: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','B', 1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Blue_plus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontRm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','R',-1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Red_minus ); 	Test.step=5; _SaveState(); break;
		case Touch_fontGm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','G',-1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Green_minus ); Test.step=5; _SaveState(); break;
		case Touch_fontBm: _KEYS_RELEASE_fontRGB;	ChangeValRGB('f','B',-1); KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_Blue_minus ); 	Test.step=5; _SaveState(); break;

		case Touch_fontSliderR:  		_KEYS_RELEASE_fontSliderRGB;  KEYBOARD_TYPE_PARAM( KEYBOARD_sliderRGB, KEY_Red_slider,  pos.x,0,0,0,0 );  _SaveState(); break;
		case Touch_fontSliderG:  		_KEYS_RELEASE_fontSliderRGB;  KEYBOARD_TYPE_PARAM( KEYBOARD_sliderRGB, KEY_Green_slider,pos.x,0,0,0,0 );  _SaveState(); break;
		case Touch_fontSliderB:  		_KEYS_RELEASE_fontSliderRGB;  KEYBOARD_TYPE_PARAM( KEYBOARD_sliderRGB, KEY_Blue_slider, pos.x,0,0,0,0 );  _SaveState(); break;
		case Touch_fontSliderR_left:  _KEYS_RELEASE_fontSliderRGB;	ChangeValRGB('f','R',-1); KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_Red_minus );   Test.step=5; _SaveState();  break;
		case Touch_fontSliderR_right: _KEYS_RELEASE_fontSliderRGB;	ChangeValRGB('f','R', 1); KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_Red_plus ); 	  Test.step=5; _SaveState();  break;
		case Touch_fontSliderG_left: 	_KEYS_RELEASE_fontSliderRGB;	ChangeValRGB('f','G',-1); KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_Green_minus ); Test.step=5; _SaveState();  break;
		case Touch_fontSliderG_right: _KEYS_RELEASE_fontSliderRGB;	ChangeValRGB('f','G', 1); KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_Green_plus );  Test.step=5; _SaveState();  break;
		case Touch_fontSliderB_left:  _KEYS_RELEASE_fontSliderRGB;	ChangeValRGB('f','B',-1); KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_Blue_minus );  Test.step=5; _SaveState();  break;
		case Touch_fontSliderB_right: _KEYS_RELEASE_fontSliderRGB;	ChangeValRGB('f','B', 1); KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_Blue_plus );   Test.step=5; _SaveState();  break;

		case Touch_bkRp: _KEYS_RELEASE_fontBk;	ChangeValRGB('b','R', 1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Red_plus );  	Test.step=5; _SaveState(); break;
		case Touch_bkGp: _KEYS_RELEASE_fontBk;	ChangeValRGB('b','G', 1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Green_plus ); Test.step=5; _SaveState(); break;
		case Touch_bkBp: _KEYS_RELEASE_fontBk;	ChangeValRGB('b','B', 1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Blue_plus );  Test.step=5; _SaveState(); break;
		case Touch_bkRm: _KEYS_RELEASE_fontBk;	ChangeValRGB('b','R',-1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Red_minus ); 	Test.step=5; _SaveState(); break;
		case Touch_bkGm: _KEYS_RELEASE_fontBk;	ChangeValRGB('b','G',-1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Green_minus );Test.step=5; _SaveState(); break;
		case Touch_bkBm: _KEYS_RELEASE_fontBk;	ChangeValRGB('b','B',-1); KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_Blue_minus ); Test.step=5; _SaveState(); break;

		case Touch_style1: ChangeFontStyle(Arial);  				KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one ); _SaveState(); break;
		case Touch_style2: ChangeFontStyle(Times_New_Roman);  KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one ); _SaveState(); break;
		case Touch_style3: ChangeFontStyle(Comic_Saens_MS);  	KEYBOARD_TYPE( KEYBOARD_fontStyle, KEY_Select_one ); _SaveState(); break;

		case Touch_type1:	ReplaceLcdStrType(0);  KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one ); _SaveState(); break;
		case Touch_type2:	ReplaceLcdStrType(1);  KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one ); _SaveState(); break;
		case Touch_type3:	ReplaceLcdStrType(2);  KEYBOARD_TYPE( KEYBOARD_fontType, KEY_Select_one ); _SaveState(); break;

		case Touch_size_plus: 	_KEYS_RELEASE_fontSize;	IncFontSize(NONE_TYPE_REQ);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_Size_plus ); _SaveState();  break;
		case Touch_size_minus:	_KEYS_RELEASE_fontSize; DecFontSize();  				  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_Size_minus ); _SaveState();  break;
		case Touch_size_norm: 	ChangeFontBoldItalNorm(0);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one ); _SaveState(); break;
		case Touch_size_bold: 	ChangeFontBoldItalNorm(1);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one ); _SaveState(); break;
		case Touch_size_italic: ChangeFontBoldItalNorm(2);  KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one ); _SaveState(); break;

		case Touch_coeff_plus:  _KEYS_RELEASE_fontCoeff;	IncCoeffRGB(); KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Coeff_plus );    _SaveState(); break;
		case Touch_coeff_minus: _KEYS_RELEASE_fontCoeff;	DecCoeefRGB(); KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_Coeff_minus );   _SaveState(); break;

		case Touch_LenWin_plus: 		_KEYS_RELEASE_LenOffsWin;	 Inc_lenWin();  					KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_LenWin_plus ); 	 	_SaveState();  break;
		case Touch_LenWin_minus: 		_KEYS_RELEASE_LenOffsWin;	 Dec_lenWin();  					KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_LenWin_minus ); 	 	_SaveState();  break;
		case Touch_OffsWin_plus: 		_KEYS_RELEASE_LenOffsWin;	 Inc_offsWin();  					KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_OffsWin_plus );   	_SaveState();  break;
		case Touch_OffsWin_minus: 		_KEYS_RELEASE_LenOffsWin;	 Dec_offsWin();  					KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_OffsWin_minus );  	_SaveState();  break;
		case Touch_PosInWin_plus: 		_KEYS_RELEASE_LenOffsWin;	 Dec_PosCursor();  				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_PosInWin_plus );  	_SaveState();  break;
		case Touch_PosInWin_minus: 	_KEYS_RELEASE_LenOffsWin;	 Inc_PosCursor();  				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_PosInWin_minus ); 	_SaveState();  break;
		case Touch_SpaceFonts_plus: 	_KEYS_RELEASE_LenOffsWin;	 IncDec_SpaceBetweenFont(1);  KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_SpaceFonts_plus );  _SaveState();  break;
		case Touch_SpaceFonts_minus: 	_KEYS_RELEASE_LenOffsWin;	 IncDec_SpaceBetweenFont(0);  KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_SpaceFonts_minus ); _SaveState();  break;
		case Touch_DispSpaces: 	_KEYS_RELEASE_LenOffsWin;	 /* here do nothing */  				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_DispSpaces ); _SaveState();  break;
		case Touch_WriteSpaces: _KEYS_RELEASE_LenOffsWin;	LCD_WriteSpacesBetweenFontsOnSDcard();  KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_WriteSpaces ); _SaveState();  break;
		case Touch_ResetSpaces: _KEYS_RELEASE_LenOffsWin;	LCD_ResetSpacesBetweenFonts();  			 KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_ResetSpaces ); _SaveState();  break;
		case Touch_SpacesInfoUp: 									 /* here do nothing */  				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_InfoSpacesUp );  break;
		case Touch_SpacesInfoDown: 								 /* here do nothing */  				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_InfoSpacesDown ); break;

		case Touch_FontSizeRoll:
			vTimerService(0,start_time,1000);
			deltaForEndPress = LCD_TOUCH_ScrollSel_Service(ROLL_1,press, &pos.y,	LCD_TOUCH_ScrollSel_DetermineRateRoll(ROLL_1,state,pos.x) );
			if(deltaForEndPress)
				KEYBOARD_TYPE( KEYBOARD_fontSize2, KEY_Select_one);
			_SaveState();
			break;

		case Touch_MainFramesType:
			if(ppMain[0]==(int*)FRAMES_GROUP_separat)	*ppMain=(int*)FRAMES_GROUP_combined;
			else													*ppMain=(int*)FRAMES_GROUP_separat;
			FILE_NAME(main)(LoadPartScreen,(char**)ppMain);
			break;

		default:
			if(IS_RANGE(state,Touch_Q,Touch_enter)){
				if(Touch_exit==state){
					LCD_TOUCH_RestoreAllSusspendedTouchs();
					FILE_NAME(main)(LoadPartScreen,(char**)ppMain);
					KEYBOARD_TYPE(KEYBOARD_none,0);
				}
				else{	_KEYS_RELEASE_setTxt;	KEYBOARD_TYPE(KEYBOARD_setTxt,KEY_Q+(state-Touch_Q));  _SaveState(); }
				break;
			}

			if(_WasState(Touch_fontRp) || _WasState(Touch_fontRm) ||
				_WasState(Touch_fontGp) || _WasState(Touch_fontGm) ||
				_WasState(Touch_fontBp) || _WasState(Touch_fontBm) )
			{
				KEYBOARD_TYPE( KEYBOARD_fontRGB, KEY_All_release );
				Test.step=1;
			}

			if(_WasState(Touch_fontSliderR) || _WasState(Touch_fontSliderR_right) || _WasState(Touch_fontSliderR_left) ||
				_WasState(Touch_fontSliderG) || _WasState(Touch_fontSliderG_right) || _WasState(Touch_fontSliderG_left) ||
				_WasState(Touch_fontSliderB) || _WasState(Touch_fontSliderB_right) || _WasState(Touch_fontSliderB_left) )
			{
				KEYBOARD_TYPE( KEYBOARD_sliderRGB, KEY_All_release );
				Test.step=1;
			}

			if(_WasState(Touch_bkRp) || _WasState(Touch_bkRm) ||
				_WasState(Touch_bkGp) || _WasState(Touch_bkGm) ||
				_WasState(Touch_bkBp) || _WasState(Touch_bkBm) )
			{
				KEYBOARD_TYPE( KEYBOARD_bkRGB, KEY_All_release );
				Test.step=1;
			}

			if(_WasState(Touch_size_plus) || _WasState(Touch_size_minus))
				KEYBOARD_TYPE( KEYBOARD_fontSize, KEY_All_release_and_select_one );

			if(_WasState(Touch_coeff_plus) || _WasState(Touch_coeff_minus))
				KEYBOARD_TYPE( KEYBOARD_fontCoeff, KEY_All_release );

			if(_WasStateRange(Touch_LenWin_plus, Touch_ResetSpaces))
				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_All_release );
		/*	if(_WasState(Touch_LenWin_plus)   || _WasState(Touch_LenWin_minus) 	 || _WasState(Touch_OffsWin_plus) 	 || _WasState(Touch_OffsWin_minus) 	  ||
				_WasState(Touch_PosInWin_plus) || _WasState(	Touch_PosInWin_minus) || _WasState(Touch_SpaceFonts_plus) || _WasState(Touch_SpaceFonts_minus) ||
				_WasState(Touch_DispSpaces) 	 || _WasState(Touch_WriteSpaces) 	 || _WasState(Touch_ResetSpaces))
				KEYBOARD_TYPE( KEYBOARD_LenOffsWin, KEY_All_release ); */

			if(_WasStateRange(Touch_Q,Touch_enter))
				KEYBOARD_TYPE( KEYBOARD_setTxt, KEY_All_release );

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
			if(_WasState(Touch_FontStyle) ||
				_WasState(Touch_style1) ||
				_WasState(Touch_style2) ||
				_WasState(Touch_style3))
				SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontStyle,1,LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontStyle,TXT_FONT_STYLE));
#endif

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
			if(_WasState(Touch_FontType) ||
				_WasState(Touch_type1) ||
				_WasState(Touch_type2) ||
				_WasState(Touch_type3))
				SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontType,1,LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontType,TXT_FONT_TYPE));
#endif

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
			if(_WasState(Touch_FontSize) ||
				_WasState(Touch_size_norm) ||
				_WasState(Touch_size_bold) ||
				_WasState(Touch_size_italic))
				SCREEN_SetTouchForNewEndPos(v.FONT_VAR_FontSize,1,LCD_StrDependOnColorsVarIndirect(v.FONT_VAR_FontSize,TXT_FONT_SIZE));
#endif

			if(_WasState(Touch_FontSizeRoll)){
				if(LCD_TOUCH_ScrollSel_Service(ROLL_1,release,NULL,0) && 0==vTimerService(0,stop_time,1000)){
					KEYBOARD_TYPE( KEYBOARD_fontSize2, KEY_Select_one);
					IncFontSize( LCD_TOUCH_ScrollSel_GetSel(ROLL_1) );
				}
				else{
					LCD_TOUCH_ScrollSel_FreeRolling(ROLL_1, FUNC1_SET(KeyboardTypeDisplay,KEYBOARD_fontSize2,KEY_Select_one,0,0,0,0,0,0,0,0,0,0) );
					vTimerService(0,reset_time,paramNoUse);
				}
			}

			if(_WasState(Touch_FontSizeMove));

			break;
	}

	FILE_NAME(timer)();
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
		IncFontSize(NONE_TYPE_REQ);
	else if(DEBUG_RcvStr("b"))
		DecFontSize();

	else if(DEBUG_RcvStr(" "))
		ChangeFontStyle(NONE_TYPE_REQ);

	else if(DEBUG_RcvStr("`"))
		ReplaceLcdStrType(NONE_TYPE_REQ);

	else if(DEBUG_RcvStr("r"))
		ChangeFontBoldItalNorm(NONE_TYPE_REQ);

	else if(DEBUG_RcvStr("t"))  //!!!!!!!!!!!!!!!!!!!
		Dec_offsWin();
	else if(DEBUG_RcvStr("y"))  //!!!!!!!!!!!!!!!!!!!
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
		LCD_DisplayRemeberedSpacesBetweenFonts(0,NULL,NULL);
	else if(DEBUG_RcvStr("o"))
		LCD_WriteSpacesBetweenFontsOnSDcard();
	else if(DEBUG_RcvStr("m"))
		LCD_ResetSpacesBetweenFonts();
	else if(DEBUG_RcvStr("p"))
	{
		DbgVar(DEBUG_ON,100,Clr_ Mag_"\r\nStart: %s\r\n"_X,GET_CODE_FUNCTION);
		DisplayCoeffCalibration();

//		SCREEN_Fonts_funcSet(FONT_COLOR_LoadFontTime, BLACK);
//		SCREEN_Fonts_funcSet(COLOR_FramePress, BLACK);

	}
	else if(DEBUG_RcvStr("-"))
	{
		FILE_NAME(printInfo)();
	}
	else if(DEBUG_RcvStr("6"))
	{
		TOOGLE(RR);

		if(RR)
		{
			KeyboardTypeDisplay(KEYBOARD_fontRGB, KEY_All_release, LCD_RoundRectangle,0, 230,160, 60,40, 4, Touch_FontColor, Touch_fontRp,KeysDel);
			KeyboardTypeDisplay(KEYBOARD_bkRGB, KEY_All_release, LCD_RoundRectangle,0, 500,160, 60,40, 4, Touch_BkColor, Touch_bkRp,KeysNotDel);
		}
		else
		{
			FILE_NAME(main)(LoadPartScreen,(char**)ppMain);
			KEYBOARD_TYPE(KEYBOARD_none,0);
		}
	}

	//- Zrobic szablon na TEST SHAPE -------!!!!
	else if(DEBUG_RcvStr("7")){
		*ppMain=(int*)FRAMES_GROUP_separat;
		INCR(incH,1,255);
		FILE_NAME(main)(LoadUserScreen,(char**)ppMain);

	}
	else if(DEBUG_RcvStr("8")){
		*ppMain=(int*)FRAMES_GROUP_combined;
		DECR(incH,1,20);
		FILE_NAME(main)(LoadUserScreen,(char**)ppMain);

	}
	else if(DEBUG_RcvStr("A"))
	{
		INCR(incW,1,600);
		FILE_NAME(main)(LoadUserScreen,(char**)ppMain);

	}
	else if(DEBUG_RcvStr("Z"))
	{
		DECR(incW,1,50);
		FILE_NAME(main)(LoadUserScreen,(char**)ppMain);
	}
	else if(DEBUG_RcvStr("S"))
	{
		INCR(incP,1,600);
		FILE_NAME(main)(LoadUserScreen,(char**)ppMain);

	}
	else if(DEBUG_RcvStr("X"))
	{
		DECR(incP,1,0);
		FILE_NAME(main)(LoadUserScreen,(char**)ppMain);
	}
	//- Zrobic szablon na TEST SHAPE -------!!!!



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

static StructTxtPxlLen ELEMENT_fontRGB(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	#define _TXT_R(x)		 SL(LANG_nazwa_3)
	#define _TXT_G(x)		 SL(LANG_nazwa_4)
	#define _TXT_B(x)		 SL(LANG_nazwa_5)
	#define _TXT_LEFT(x)	 SL(LANG_nazwa_2)

	StructTxtPxlLen lenStr = {0};

	int spaceMain_width 		= LCD_GetWholeStrPxlWidth(v.FONT_ID_FontColor," ",0,ConstWidth);
	int digit3main_width 	= LCD_GetWholeStrPxlWidth(v.FONT_ID_FontColor,INT2STR(Test.font[0]),0,ConstWidth);
	int xPos_main 				= xPos + LCD_GetWholeStrPxlWidth(v.FONT_ID_Descr,_TXT_LEFT(0),0,NoConstWidth) + 4;

	int _GetWidth(char *txt){ return LCD_GetWholeStrPxlWidth(v.FONT_ID_Descr,txt,0,ConstWidth); }

	int xPos_under_left 		= MIDDLE( xPos_main+spaceMain_width, digit3main_width, _GetWidth(_TXT_R(0)) );
	int xPos_under_right 	= MIDDLE( xPos_main + 3*spaceMain_width + 2*digit3main_width, digit3main_width, _GetWidth(_TXT_B(0)) );

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(FontColor), xPos, yPos, TXT_FONT_COLOR, fullHight, 0,250, ConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	 			 Above_left, 	SL(LANG_nazwa_1), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 								 Left_mid, 		_TXT_LEFT(0), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(251,29,27), v.FONT_BKCOLOR_Descr, 4|(xPos_under_left<<16),  Under_left,	_TXT_R(20), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(60,247,68), v.FONT_BKCOLOR_Descr, 4, 								 Under_center, _TXT_G(40), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(51,90,245), v.FONT_BKCOLOR_Descr, 4|(xPos_under_right<<16), Under_right,	_TXT_B(60), fullHight, 0,250, NoConstWidth,\
		LCD_STR_DESCR_PARAM_NUMBER(5) );

	LCD_SetBkFontShape(v.FONT_VAR_FontColor,BK_LittleRound);

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
	if(LoadWholeScreen==argNmb)	SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT, Touch_FontColor, press, v.FONT_VAR_FontColor,0, field->len);
#else
	if(LoadWholeScreen==argNmb){	SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontColor,  LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontColor,0, *field);
											SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_WITH_HOLD, 		    Touch_FontColor2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontColor,1, *field);
	}
#endif


	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;

	#undef _TXT_R
	#undef _TXT_G
	#undef _TXT_B
	#undef _TXT_LEFT
}

static StructTxtPxlLen ELEMENT_fontBkRGB(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	#define _TXT_R(x)		 SL(LANG_nazwa_3)
	#define _TXT_G(x)		 SL(LANG_nazwa_4)
	#define _TXT_B(x)		 SL(LANG_nazwa_5)
	#define _TXT_LEFT(x)	 SL(LANG_nazwa_7)

	StructTxtPxlLen lenStr = {0};

	int spaceMain_width 		= LCD_GetWholeStrPxlWidth(v.FONT_ID_BkColor," ",0,ConstWidth);
	int digit3main_width 	= LCD_GetWholeStrPxlWidth(v.FONT_ID_BkColor,INT2STR(Test.bk[0]),0,ConstWidth);
	int xPos_main 				= xPos + LCD_GetWholeStrPxlWidth(v.FONT_ID_Descr,_TXT_LEFT(0),0,NoConstWidth) + 4;

	int _GetWidth(char *txt){ return LCD_GetWholeStrPxlWidth(v.FONT_ID_Descr,txt,0,ConstWidth); }

	int xPos_under_left 		= MIDDLE( xPos_main+spaceMain_width, digit3main_width, _GetWidth(_TXT_R(0)) );
	int xPos_under_right 	= MIDDLE( xPos_main + 3*spaceMain_width + 2*digit3main_width, digit3main_width, _GetWidth(_TXT_B(0)) );

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(BkColor), xPos, yPos, TXT_BK_COLOR, fullHight, 0,250, ConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	 			 Above_left,   SL(LANG_nazwa_6), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 								 Left_mid, 		_TXT_LEFT(0), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(251,29,27), v.FONT_BKCOLOR_Descr, 4|(xPos_under_left<<16),  Under_left,	_TXT_R(20), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(60,247,68), v.FONT_BKCOLOR_Descr, 4, 								 Under_center, _TXT_G(40), fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(51,90,245), v.FONT_BKCOLOR_Descr, 4|(xPos_under_right<<16), Under_right,	_TXT_B(60), fullHight, 0,250, NoConstWidth,\
		LCD_STR_DESCR_PARAM_NUMBER(5) );

	LCD_SetBkFontShape(v.FONT_VAR_BkColor,BK_LittleRound);

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
	if(LoadWholeScreen==argNmb)	SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT, Touch_BkColor, press, v.FONT_VAR_BkColor,0, field->len);
#else
	if(LoadWholeScreen==argNmb)	SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT, Touch_BkColor, press, v.FONT_VAR_BkColor,0, *field);
#endif

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;

	#undef _TXT_R
	#undef _TXT_G
	#undef _TXT_B
	#undef _TXT_LEFT
}

static StructTxtPxlLen ELEMENT_fontLenOffsWin(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	StructTxtPxlLen lenStr = {0};

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(LenWin), xPos, yPos, TXT_LENOFFS_WIN, fullHight, 0,250, ConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	Above_left,  SL(LANG_LenOffsWin3), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 					Left_mid, 	  "8.",  fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	Under_left,  SL(LANG_LenOffsWin4), fullHight, 0,250, NoConstWidth, \
		LCD_STR_DESCR_PARAM_NUMBER(3) );

	LCD_SetBkFontShape(v.FONT_VAR_LenWin,BK_LittleRound);

	if(LoadWholeScreen==argNmb)	SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT, Touch_FontLenOffsWin, press, v.FONT_VAR_LenWin,0, *field);

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;
}

static StructTxtPxlLen ELEMENT_fontCoeff(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	StructTxtPxlLen lenStr = {0};

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(Coeff), xPos, yPos, TXT_COEFF, fullHight, 0,250, ConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	Above_left,   SL(LANG_FontCoeffAbove), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 					Left_mid, 	  SL(LANG_FontCoeffLeft),  fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 					Under_center, SL(LANG_FontCoeffUnder), fullHight, 0,250, NoConstWidth, \
		LCD_STR_DESCR_PARAM_NUMBER(3) );

	LCD_SetBkFontShape(v.FONT_VAR_Coeff,BK_LittleRound);

	if(LoadWholeScreen==argNmb)	SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT, Touch_FontCoeff, press, v.FONT_VAR_Coeff,0, *field);

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;
}

static StructTxtPxlLen ELEMENT_fontType(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	StructTxtPxlLen lenStr = {0};

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(FontType), xPos, yPos, TXT_FONT_TYPE, fullHight, 0,255, NoConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16), Above_left,   SL(LANG_FontTypeAbove), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 				 Left_mid, 		SL(LANG_FontTypeLeft), 	fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(251,29,27), v.FONT_BKCOLOR_Descr, 4,  			 Under_center,	SL(LANG_FontTypeUnder), fullHight, 0,250, NoConstWidth, \
		LCD_STR_DESCR_PARAM_NUMBER(3) );

	LCD_SetBkFontShape(v.FONT_VAR_FontType,BK_LittleRound);

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
	if(LoadWholeScreen==argNmb){ SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontType,  LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontType,0, field->len);
										  SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, 		  Touch_FontType2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontType,1, field->len); }
#else
	if(LoadWholeScreen==argNmb){ SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontType,  LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontType,0, *field);
										  SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_WITH_HOLD, 		    Touch_FontType2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontType,1, *field); }
#endif

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;
}

static StructTxtPxlLen ELEMENT_fontSize(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	StructTxtPxlLen lenStr = {0};
	StructFieldPos fieldTouch = {0};

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(FontSize), xPos, yPos, TXT_FONT_SIZE, fullHight, 0,255, NoConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16), Above_left,   SL(LANG_FontSizeAbove), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 				 Left_mid, 		SL(LANG_FontSizeLeft),  fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, RGB2INT(251,29,27), v.FONT_BKCOLOR_Descr, 4,  			 Under_center,	SL(LANG_FontSizeUnder), fullHight, 0,250, NoConstWidth, \
		LCD_STR_DESCR_PARAM_NUMBER(3) );

	LCD_SetBkFontShape(v.FONT_VAR_FontSize,BK_LittleRound);

	fieldTouch 			= *field;
	fieldTouch.width 	= fieldTouch.width/3;
	fieldTouch.x 		= fieldTouch.x + fieldTouch.width;

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
	if(LoadWholeScreen==argNmb){ SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontSize, LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontSize,0, field->len);
										  SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, 		  Touch_FontSize2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontSize,1, field->len); }
#else
	if(LoadWholeScreen==argNmb){ SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontSize,  	  LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontSize,0, *field);
										  SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_WITH_HOLD, 		   Touch_FontSize2, 	  LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontSize,1, *field);
										  SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_MOVE_RIGHT, 		    	   Touch_FontSizeMove, press, 								 v.FONT_VAR_FontSize,2, fieldTouch); }
#endif

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;
}

static StructTxtPxlLen ELEMENT_fontStyle(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	StructTxtPxlLen lenStr = {0};
	StructFieldPos field_copy = {0};

	StructFieldPos _Function_FontStyleElement(int noDisp, char *txt){
		return LCD_StrDependOnColorsDescrVar_array_xyCorrect(noDisp,STR_FONT_PARAM2(FontStyle), xPos, yPos, txt, fullHight, 0,255, NoConstWidth, \
			v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16), Above_left,   SL(LANG_FontStyleAbove), fullHight, 0,250, NoConstWidth,\
			v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 				 Left_mid, 		SL(LANG_FontStyleLeft),  fullHight, 0,250, NoConstWidth, \
			v.FONT_ID_Descr, RGB2INT(251,29,27), v.FONT_BKCOLOR_Descr, 4|(xPos<<16), Under_left,	SL(LANG_FontStyleUnder), fullHight, 0,250, NoConstWidth, \
			LCD_STR_DESCR_PARAM_NUMBER(3) );
	}

	*field = _Function_FontStyleElement(1," "LONGEST_TXT_FONTSTYLE" ");		field_copy = *field;		/* To adjust frame width to the longest possible mainTxt */
	*field = _Function_FontStyleElement(0,TXT_FONT_STYLE);

	field->width = field_copy.width;
	field->height = field_copy.height;

	LCD_SetBkFontShape(v.FONT_VAR_FontStyle,BK_LittleRound);

#ifdef TOUCH_MAINFONTS_WITHOUT_DESCR
	if(LoadWholeScreen==argNmb){ SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontStyle, LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontStyle,0, field->len);
										  SCREEN_ConfigTouchForStrVar(ID_TOUCH_POINT_WITH_HOLD, 		  Touch_FontStyle2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontStyle,1, field->len); }
#else
	if(LoadWholeScreen==argNmb){ SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_RELEASE_WITH_HOLD, Touch_FontStyle, LCD_TOUCH_SetTimeParam_ms(600), v.FONT_VAR_FontStyle,0, *field);
										  SCREEN_ConfigTouchForStrVar_2(ID_TOUCH_POINT_WITH_HOLD, 		  	 Touch_FontStyle2, LCD_TOUCH_SetTimeParam_ms(700), v.FONT_VAR_FontStyle,1, *field); }
#endif

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;
}

static StructTxtPxlLen ELEMENT_fontTime(StructFieldPos *field, int xPos,int yPos, int argNmb)
{
	StructTxtPxlLen lenStr = {0};

	*field = LCD_StrDependOnColorsDescrVar_array_xyCorrect(0,STR_FONT_PARAM2(LoadFontTime), xPos, yPos, TXT_TIMESPEED, fullHight, 0,250, ConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	Above_left,  SL(LANG_TimeSpeed1), fullHight, 0,250, NoConstWidth,\
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4, 					Left_mid, 	  "7.",  fullHight, 0,250, NoConstWidth, \
		v.FONT_ID_Descr, v.FONT_COLOR_Descr, v.FONT_BKCOLOR_Descr, 4|(xPos<<16),	Under_left,  SL(LANG_TimeSpeed2), fullHight, 0,250, NoConstWidth, \
		LCD_STR_DESCR_PARAM_NUMBER(3) );

	LCD_SetBkFontShape(v.FONT_VAR_LenWin,BK_LittleRound);

	lenStr.inPixel = field->width;
	lenStr.height 	= field->height;

	return lenStr;
}

static void FRAMES_GROUP_combined(int argNmb, int startOffsX,int startOffsY, int offsX,int offsY, int bold)
{
	#define _LINES_COLOR		COLOR_GRAY(0x77)
	#define _FILL_COLOR		v.COLOR_FillMainFrame

	#define	_Element(name,cmdX,offsX,cmdY,offsY)		lenStr=ELEMENT_##name(&field, LCD_Xpos(lenStr,cmdX,offsX), LCD_Ypos(lenStr,cmdY,offsY), argNmb);
	#define	_LineH(width,cmdX,offsX,cmdY,offsY)		 LCD_LineH(LCD_X,LCD_Xpos(lenStr,cmdX,offsX)-2, LCD_Ypos(lenStr,cmdY,offsY), width+4, _LINES_COLOR, bold );
	#define	_LineV(width,cmdX,offsX,cmdY,offsY)		 LCD_LineV(LCD_X,LCD_Xpos(lenStr,cmdX,offsX), LCD_Ypos(lenStr,cmdY,offsY)-2, width+4, _LINES_COLOR, bold );

	StructFieldPos field={0}, field1={0};
	uint16_t tab[4]={0};
	int X_start=0;

	FILE_NAME(funcSet)(FONT_BKCOLOR_Descr, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontColor, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_BkColor, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontType, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontSize, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontStyle, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_Coeff, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_LenWin, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_LoadFontTime, _FILL_COLOR);

	_Element(fontRGB,SetPos,X_start=startOffsX,SetPos,startOffsY)		/* _LineV(field.height,GetPos,-startOffsX/2-1,GetPos,0) */	field1=field;
	_Element(fontBkRGB,GetPos,0,IncPos,offsY)									/* _LineV(field.height,GetPos,-startOffsX/2-1,GetPos,0) */
	tab[0]=field1.width;
	tab[1]=field.width;
	MAXVAL(tab,2,0,tab[3])
	_LineH(tab[3],GetPos,0,GetPos,-offsY/2-1)

	_Element(fontType,SetPos,X_start+=tab[3]+offsX,SetPos,startOffsY)		_LineV(field.height,GetPos,-offsX/2-1,GetPos,0)	field1=field;
	_Element(fontSize,GetPos,0,IncPos,offsY)										_LineV(field.height,GetPos,-offsX/2-1,GetPos,0)
	tab[0]=field1.width;
	tab[1]=field.width;
	MAXVAL(tab,2,0,tab[3])
	_LineH(tab[3],GetPos,0,GetPos,-offsY/2-1)

	_Element(fontStyle,SetPos,X_start+=tab[3]+offsX,SetPos,startOffsY)	_LineV(field.height,GetPos,-offsX/2-1,GetPos,0)	field1=field;
	_Element(fontTime,GetPos,0,IncPos,offsY)										_LineV(field.height,GetPos,-offsX/2-1,GetPos,0)
	tab[0]=field1.width;
	tab[1]=field.width;
	MAXVAL(tab,2,0,tab[3])
	_LineH(tab[3],GetPos,0,GetPos,-offsY/2-1)

	int offsX_temp = 0;
	_Element(fontLenOffsWin,SetPos,X_start+=tab[3]+offsX,SetPos,startOffsY)	_LineV(field.height,GetPos,-offsX/2-1,				 GetPos,0)	field1=field;
	_Element(fontCoeff,GetPos,0-offsX_temp,IncPos,offsY)							_LineV(field.height,GetPos,-offsX/2-1-offsX_temp,GetPos,0)
	tab[0]=field1.width;
	tab[1]=field.width;
	MAXVAL(tab,2,0,tab[3])
	_LineH(tab[3],GetPos,0,GetPos,-offsY/2-1)

	#undef _Element
	#undef _LineH
	#undef _LineV
	#undef _FILL_COLOR
	#undef _LINES_COLOR
}

static int FRAME_bold2Space(uint8_t bold, uint8_t space){
	return ((uint32_t)bold&0x000000FF)|(uint32_t)space<<8;
}

static void FRAMES_GROUP_separat(int argNmb, int startOffsX,int startOffsY, int offsX,int offsY, int boldFrame)		/* Parameters ..Offs.. is counted from STR (not from FRAME) */
{
	#define _FRAME_COLOR		v.COLOR_Frame
	#define _FILL_COLOR		v.COLOR_FillFrame
																									 /* LCD_BoldRoundRectangle */
	#define _Rectan LCD_Shape(field.x-fontsFrameSpace, field.y-fontsFrameSpace, LCD_RoundRectangle, field.width+2*fontsFrameSpace, field.height+2*fontsFrameSpace, SetBold2Color(_FRAME_COLOR,bold), _FILL_COLOR, v.COLOR_FillMainFrame)

	#define _Element(name,nrX,cmdX,Xoffs,nrY,cmdY,Yoffs)	\
			lenStr=ELEMENT_##name(&field, LCD_posX(nrX,lenStr,cmdX,Xoffs), LCD_posY(nrY,lenStr,cmdY,Yoffs), argNmb); \
			_Rectan; \
			lenStr=ELEMENT_##name(&field, LCD_posX(nrX,lenStr,GetPos,0), 	LCD_posY(nrY,lenStr,GetPos,0), 	argNmb); \
			LCD_posY(nrY,lenStr,IncPos,offsY);

	StructFieldPos field={0};
	uint8_t fontsFrameSpace = boldFrame >>8;
	int bold = boldFrame&0x000000FF;

	FILE_NAME(funcSet)(FONT_BKCOLOR_Descr, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontColor, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_BkColor, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontType, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontSize, 	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_FontStyle,	_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_Coeff, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_LenWin, 		_FILL_COLOR);
	FILE_NAME(funcSet)(FONT_BKCOLOR_LoadFontTime, _FILL_COLOR);

	_Element(fontRGB,0,SetPos,startOffsX,0,SetPos,startOffsY) 	_Element(fontType,0,IncPos,offsX,1,SetPos,startOffsY)  _Element(fontStyle,0,IncPos,offsX,2,SetPos,startOffsY)  _Element(fontLenOffsWin,0,IncPos,offsX,3,SetPos,startOffsY)
	_Element(fontBkRGB,0,SetPos,startOffsX,0,GetPos,0) 		 	_Element(fontSize,0,IncPos,offsX,1,GetPos,0)				 _Element(fontTime,0,IncPos,offsX,2,GetPos,0)				_Element(fontCoeff,0,IncPos,offsX,3,GetPos,0)

	#undef _Element
	#undef _Rectan
	#undef _FRAME_COLOR
	#undef _FILL_COLOR
}

void FILE_NAME(main)(int argNmb, char **argVal)
{
	if(NULL == argVal)
		argVal = (char**)ppMain;

	LCD_Clear(v.COLOR_BkScreen);

	if(LoadWholeScreen == argNmb)
	{
		SCREEN_ResetAllParameters();
		LCD_TOUCH_DeleteAllSetTouch();
		FONTS_LCD_ResetParam();

		DbgVar(DEBUG_ON,100,Clr_ Cya_"\r\nStart: %s\r\n"_X,GET_CODE_FUNCTION);

		LoadFonts(FONT_ID_Title, FONT_ID_Press);
		LCD_LoadFontVar();

		LCDTOUCH_Set(LCD_X-FV(SetVal,0,LCD_GetWholeStrPxlWidth(v.FONT_ID_Descr,SL(LANG_MainFrameType),0,NoConstWidth)+5), \
						 LCD_Y-FV(SetVal,1,LCD_GetFontHeight(v.FONT_ID_Descr)+5), \
						 	 	 FV(GetVal,0,NoUse),\
								 FV(GetVal,1,NoUse), ID_TOUCH_POINT,Touch_MainFramesType,press);

		LCD_Ymiddle(nrMIDDLE_TXT,SetPos, SetPosAndWidth(Test.yFontsField,240) );
		LCD_Xmiddle(nrMIDDLE_TXT,SetPos, SetPosAndWidth(Test.xFontsField,LCD_GetXSize()),NULL,0,NoConstWidth);
		LCD_SetBkFontShape(v.FONT_VAR_Fonts,BK_Rectangle);
	}

	/*FILE_NAME(printInfo)();*/
	LCD_DrawMainFrame(LCD_RoundRectangle,NoIndDisp,0, 0,0, LCD_X,220,SHAPE_PARAM(MainFrame,FillMainFrame,BkScreen));

	if		 (*(argVal+0)==(char*)FRAMES_GROUP_combined)
		FRAMES_GROUP_combined(argNmb,15,15,25,25,1);
	else if(*(argVal+0)==(char*)FRAMES_GROUP_separat)
		FRAMES_GROUP_separat(argNmb,23,20,25,25,FRAME_bold2Space(0,6));

	/*	lenStr=LCD_StrDependOnColorsVar(STR_FONT_PARAM(CPUusage,FillMainFrame),450,0,TXT_CPU_USAGE,halfHight,0,255,ConstWidth); */
	LCD_StrDependOnColors(v.FONT_ID_Descr, LCD_X-FV(GetVal,0,NoUse), LCD_Y-FV(GetVal,1,NoUse), SL(LANG_MainFrameType), fullHight,0, v.COLOR_FillFrame, v.FONT_COLOR_Descr, 255, NoConstWidth);

	if(LoadUserScreen == argNmb){

	}

	StartMeasureTime_us();
	 if(Test.type) lenStr= LCD_StrVar			  (v.FONT_VAR_Fonts,v.FONT_ID_Fonts, POS_X_TXT, POS_Y_TXT, Test.txt, fullHight, Test.spaceBetweenFonts, argNmb==0 ? v.COLOR_BkScreen : LCD_GetStrVar_bkColor(v.FONT_VAR_Fonts), 0,			  Test.constWidth, v.COLOR_BkScreen);
	 else 			lenStr= LCD_StrChangeColorVar(v.FONT_VAR_Fonts,v.FONT_ID_Fonts, POS_X_TXT, POS_Y_TXT, Test.txt, fullHight, Test.spaceBetweenFonts, RGB_BK, RGB_FONT,																		  Test.coeff, Test.constWidth, v.COLOR_BkScreen);
	Test.speed=StopMeasureTime_us("");

	if(LoadWholeScreen  == argNmb) TxtTouch(TouchSetNew);
	if(LoadNoDispScreen != argNmb) LCD_Show();
}

#undef INT2STR_TIME
#undef POS_X_TXT
#undef POS_Y_TXT




//ZROBIC animacje ze samo sie klioka i chmurka z info ze przytrzymac na 2 sekundy ....
//ZROBIC AUTOMATYCZNE testy wszystkich mozliwosci !!!!!!! taki interfejs testowy
//tu W **arcv PRZEKAZ TEXT !!!!!! dla fonts !!!
