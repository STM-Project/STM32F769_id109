/*
 * Keyboard.c
 *
 *  Created on: Sep 8, 2024
 *      Author: maraf
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Keyboard.h"
#include "LCD_BasicGaphics.h"
#include "touch.h"
#include "SCREEN_ReadPanel.h"
#include "common.h"

#define MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY		20
#define KEYBOARD_NONE	0

#define MAX_WIN_X		50
#define MAX_WIN_Y		60

static char 				 txtKey			   [MAX_WIN_Y] [MAX_WIN_X];
static COLORS_DEFINITION colorTxtKey		[MAX_WIN_Y * MAX_WIN_X];
static COLORS_DEFINITION colorTxtPressKey [MAX_WIN_Y * MAX_WIN_X];
static uint16_t dimKeys[2];

static struct KEYBOARD_SETTINGS{
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
} s[MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY]={0}, c={0};

static int fontID = 0;
static int fontID_descr	= 0;
static int colorDescr	= 0;
static int frameMainColor = 0;
static int fillMainColor = 0;
static int frameColor = 0;
static int fillColor = 0;
static int framePressColor = 0;
static int fillPressColor = 0;
static int bkColor = 0;

static int frameColor_c[5]={0}, fillColor_c[5]={0}, bkColor_c[5]={0};
static uint16_t widthAll = 0, widthAll_c = 0;
static uint16_t heightAll = 0, heightAll_c = 0;

static void _deleteAllTouchs(void){
	for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
		for(int i=0; i<s[j].nmbTouch; ++i)
			LCD_TOUCH_DeleteSelectTouch(s[j].startTouchIdx+i);
	}
}
static void _deleteAllTouchParams(void){
	for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
		s[j].forTouchIdx = 0;  //NoTouch
		s[j].nmbTouch = 0;
	}
}
static void _deleteTouchs(int nr){
	for(int i=0; i<s[nr].nmbTouch; ++i)
		LCD_TOUCH_DeleteSelectTouch(s[nr].startTouchIdx+i);
}
static void _deleteTouchParams(int nr){
	s[nr].forTouchIdx = 0;  //NoTouch
	s[nr].nmbTouch = 0;
}
static void _Str(const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _StrDescr_XYoffs(XY_Touch_Struct pos,int offsX,int offsY, const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr, pos.x+offsX, pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _StrDescrWin_XYoffs(int nr,int offsX,int offsY, const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,s[nr].widthKey,s[nr].heightKey,fontID_descr, offsX,offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _Str_Xmidd_Yoffs(int nr,XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[nr].widthKey),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt), pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _StrWin_Xmidd_Yoffs(int nr,int offsY, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(0,s[nr].widthKey),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,s[nr].widthKey, s[nr].heightKey,fontID, GET_X((char*)txt),offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _StrLeft(int nr, const char *txt, XY_Touch_Struct pos, uint32_t color){
	LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[nr].heightKey));
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+LCD_GetFontHeight(fontID)/2, GET_Y, (char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _StrDescr_Xmidd_Yoffs(XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(0,widthAll),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),	pos.y+offsY, 	(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
}
static void _StrDescr(int nr,XY_Touch_Struct pos, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(pos.x,widthAll),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),		s[nr].interSpace,		(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
}
static void _StrPress(const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
}
static void _StrPressLeft(int nr,const char *txt, XY_Touch_Struct pos, uint32_t color){
	LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[nr].heightKey));
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+10,GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
}
static void _StrDisp(int nr,const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindowIndirect(0, s[nr].x, s[nr].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
static void _StrPressDisp(int nr,const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindowIndirect(0, s[nr].x, s[nr].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
}
static void _TxtPos(int nr,XY_Touch_Struct pos){
	LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[nr].widthKey),NULL,0,NoConstWidth);
	LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[nr].heightKey));
}
static void _Key(int nr,XY_Touch_Struct pos){
	LCD_ShapeWindow( s[nr].shape, 0, widthAll,heightAll, pos.x,pos.y, s[nr].widthKey,s[nr].heightKey, SetBold2Color(frameColor,s[nr].bold),fillColor,bkColor);
}
static void _KeyPressWin(int nr){
	LCD_ShapeWindow( s[nr].shape, 0, s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
}
static void _KeyStr(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color){
	_Key(nr,pos);
	_TxtPos(nr,pos);
	_Str(txt,color);
}
static void _KeyStrleft(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color){
	_Key(nr,pos);
	_StrLeft(nr,txt,pos,color);
}
static void _KeyStrDisp(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color){
	_Key(nr, pos);
	_TxtPos(nr,pos);
	_StrDisp(nr,txt,color);
}
static void _KeyPress(int nr, XY_Touch_Struct pos){
	LCD_ShapeWindow( s[nr].shape, 0, widthAll,heightAll, pos.x,pos.y, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
}
static void _KeyStrPress(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	_KeyPress(nr,pos);
	_TxtPos(nr,pos);
	_StrPress(txt,colorTxt);
}
static void _KeyStrPressLeft(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	_KeyPress(nr,pos);
	_StrPressLeft(nr,txt,pos,colorTxt);
}
static void _KeyStrPressDisp(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	_KeyPress(nr,pos);
	_TxtPos(nr,pos);
	_StrPressDisp(nr,txt,colorTxt);
}
static void _KeyStrPressDisp_oneBlock(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	LCD_ShapeWindow( s[nr].shape, 0, s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
	_TxtPos(nr,(XY_Touch_Struct){0});
	LCD_StrDependOnColorsWindowIndirect(0, s[nr].x+pos.x, s[nr].y+pos.y, s[nr].widthKey, s[nr].heightKey,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, colorTxt,255, NoConstWidth);
}
static void _KeyStrPressDisp_oneKey(int nr, XY_Touch_Struct pos, int nrTab){
	_KeyStrPressDisp_oneBlock(nr, pos, txtKey[nrTab], colorTxtPressKey[nrTab]);
}
static void _KeyShapePressDisp_oneBlock(int nr, XY_Touch_Struct pos, ShapeFunc pShape, SHAPE_PARAMS param){
	LCD_ShapeWindow( s[nr].shape, 0, s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
	pShape(0,param);
	LCD_Display(0, s[nr].x+pos.x, s[nr].y+pos.y, s[nr].widthKey, s[nr].heightKey);
}
static void _SetTouchSlider(int nr,uint16_t idx, SHAPE_PARAMS posElemSlider){
	for(int i=0; i<NMB_SLIDER_ELEMENTS; ++i){
		touchTemp[0].x= s[nr].x + posElemSlider.pos[i].x;
		touchTemp[1].x= touchTemp[0].x + posElemSlider.size[i].w;
		touchTemp[0].y= s[nr].y + posElemSlider.pos[i].y;
		touchTemp[1].y= touchTemp[0].y + posElemSlider.size[i].h;

		switch(i){ case 0: case 2: LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, idx + s[nr].nmbTouch++, TOUCH_GET_PER_X_PROBE   ); break;
					  case 1:			LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT,				idx + s[nr].nmbTouch++, TOUCH_GET_PER_ANY_PROBE ); break; }
		}
}
static void _ElemSliderPressDisp_oneBlock(int nr, uint16_t x, XY_Touch_Struct posSlider, uint32_t spaceTringLine, int selElem, uint32_t lineColor, uint32_t lineSelColor, int *pVal, int valType, int maxSlidVal, VOID_FUNCTION *pfunc){
	int value = 0;
	switch(valType){
		case PosX:		value = SetValType(CONDITION(0>x-s[nr].x-s[nr].interSpace,0,x-s[nr].x-s[nr].interSpace),PosX);  break;
		case Percent:	value = SetValType(PERCENT_SCALE(*pVal+1,maxSlidVal+1),Percent);  			  					  break;
	}
	LCD_ShapeWindow(LCD_Rectangle,0,s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, bkColor, bkColor,bkColor);
	SHAPE_PARAMS slid = LCD_SimpleSlider(0, s[nr].widthKey, s[nr].heightKey, 0,0, ChangeElemSliderSize(s[nr].widthKey,NORMAL_SLIDER_PARAM), SetSpaceTriangLineSlider(s[nr].heightKey,spaceTringLine), lineColor, lineSelColor ,selElem|0xFF000000, bkColor, value, selElem);
	LCD_Display(0, s[nr].x+posSlider.x, s[nr].y+posSlider.y, s[nr].widthKey, s[nr].heightKey);
	if(PtrSel==SHIFT_RIGHT(selElem,24,F))
		*pVal = SET_NEW_RANGE( ((maxSlidVal+1)*slid.param[0])/slid.param[1], slid.param[2],maxSlidVal-slid.param[2], 0,maxSlidVal );
	if(pfunc) pfunc();
}
static void _SetTouch(int nr, uint16_t ID, uint16_t idx, uint8_t paramTouch, XY_Touch_Struct pos){
	touchTemp[0].x= s[nr].x + pos.x;
	touchTemp[1].x= touchTemp[0].x + s[nr].widthKey;
	touchTemp[0].y= s[nr].y + pos.y;
	touchTemp[1].y= touchTemp[0].y + s[nr].heightKey;
	LCD_TOUCH_Set(ID,idx,paramTouch);
	s[nr].nmbTouch++;
}
static void SetPosKey(int nr, XY_Touch_Struct pos[], int interSpace, int head){
	int d=0;
	for(int j=0; j<dimKeys[1]; ++j){
		for(int i=0; i<dimKeys[0]; ++i){
			pos[d].x = s[nr].interSpace + i*(s[nr].widthKey  + interSpace);
			pos[d].y = s[nr].interSpace + j*(s[nr].heightKey + interSpace) + head;
			d++;
	}}
}
static int GetHeightHead(int nr){
	return s[nr].interSpace + LCD_GetFontHeight(fontID_descr) + s[nr].interSpace;
}
static void SetDimKey(int nr, figureShape shape, uint16_t width, uint16_t height){
	if(0!=shape && KeysAutoSize==width){
		int count = dimKeys[0]*dimKeys[1];
		int tab[count];

		for(int i=0; i<count; ++i)
			tab[i] = LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[i],0,NoConstWidth);
		INIT_MAXVAL(tab,STRUCT_TAB_SIZE(tab),0,maxVal);

		s[nr].widthKey =  height + maxVal + height;		/*	space + text + space */
		s[nr].heightKey = height + LCD_GetFontHeight(fontID) + height;
	}
}
static void SetDimAll(int nr, int interSpace, int head){
	widthAll =  dimKeys[0]*s[nr].widthKey  + (dimKeys[0]-1)*interSpace + 2*s[nr].interSpace;
	heightAll = dimKeys[1]*s[nr].heightKey + (dimKeys[1]-1)*interSpace + 2*s[nr].interSpace + head;
}
static void KeysAllRelease(int nr, XY_Touch_Struct posKeys[], char* headTxt){
	int countKey = dimKeys[0]*dimKeys[1];
	XY_Touch_Struct posHead={0,0};
	LCD_ShapeWindow( s[nr].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[nr].bold), fillMainColor,bkColor );
	_StrDescr(nr,posHead, headTxt, colorDescr);
	for(int i=0; i<countKey; ++i){
		i<countKey-1 ? _KeyStr(nr,posKeys[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(nr,posKeys[i],txtKey[i],colorTxtKey[i]);
	/*	_Key(posKey[i]);
		_TxtPos(k,posKey[i]);		i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(k,txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
}}
static void KeysSelectOne(int nr, XY_Touch_Struct posKeys[], char* headTxt, int value){
	int countKey = dimKeys[0]*dimKeys[1];
	LCD_ShapeWindow( s[nr].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[nr].bold), fillMainColor,bkColor );
	if(NULL!=headTxt){	XY_Touch_Struct posHead={0,0};	_StrDescr(nr,posHead, headTxt, colorDescr);	}
	for(int i=0; i<countKey; ++i){
		if(i == value)
			i<countKey-1 ? _KeyStrPress(nr,posKeys[i],txtKey[i],colorTxtPressKey[i]) : _KeyStrPressDisp(nr,posKeys[i],txtKey[i],colorTxtPressKey[i]);
		else
			i<countKey-1 ? _KeyStr(nr,posKeys[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(nr,posKeys[i],txtKey[i],colorTxtKey[i]);
}}
static void SetTouch_CountButton(int nr, uint16_t startTouchIdx, XY_Touch_Struct* posKeys){
	if(startTouchIdx){
		for(int i=0; i<dimKeys[0]*dimKeys[1]; ++i)
			_SetTouch(nr,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[nr].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKeys[i]);
}}
static void SetTouch_Select(int nr, uint16_t startTouchIdx, XY_Touch_Struct* posKeys){
	if(startTouchIdx){
		for(int i=0; i<dimKeys[0]*dimKeys[1]; ++i)
			_SetTouch(nr,ID_TOUCH_POINT, s[nr].startTouchIdx + i, press, posKeys[i]);
}}
static int _GetPosKeySize(void){
	int countKey = dimKeys[0]*dimKeys[1];		if(countKey > MAX_WIN_X * MAX_WIN_Y-1)  countKey= MAX_WIN_X * MAX_WIN_Y;
	return countKey;
}


/*	-----------	General Functions -----------------*/

void KEYBOARD_KeyParamSet(TXT_PARAM_KEY param, uint16_t dimX, uint16_t dimY, ...){
	va_list va;
	dimKeys[0] = dimX;
	dimKeys[1] = dimY;
	va_start(va,0);
	for(int i=0; i<dimKeys[0]*dimKeys[1]; ++i){
		switch(param){
			case StringTxt:
				char *ptr= va_arg(va,char*);
				int len= strlen(ptr);		if(len > MAX_WIN_X-1) len= MAX_WIN_X-1;
				strcpy(txtKey[i],ptr);
				break;
			case Color1Txt:
				colorTxtKey[i]= va_arg(va,COLORS_DEFINITION);
				break;
			case Color2Txt:
				colorTxtPressKey[i]= va_arg(va,COLORS_DEFINITION);
				break;
	}}
	va_end(va);
}

void KEYBOARD_SetGeneral(int vFontID,int vFontID_descr,int vColorDescr,int vFrameMainColor,int vFillMainColor,int vFrameColor,int vFillColor,int vFramePressColor,int vFillPressColor,int vBkColor){
	if(vFontID 			  !=N) fontID 			  = vFontID;
	if(vFontID_descr 	  !=N) fontID_descr 	  = vFontID_descr;
	if(vColorDescr 	  !=N) colorDescr 	  = vColorDescr;

	if(vFrameMainColor  !=N) frameMainColor  = vFrameMainColor;
	if(vFillMainColor   !=N) fillMainColor   = vFillMainColor;

	if(vFrameColor 	  !=N) frameColor 	  = vFrameColor;
	if(vFillColor 		  !=N) fillColor 		  = vFillColor;

	if(vFramePressColor !=N) framePressColor = vFramePressColor;
	if(vFillPressColor  !=N) fillPressColor  = vFillPressColor;
	if(vBkColor 		  !=N) bkColor 		  = vBkColor;
}

int KEYBOARD_StartUp(int type, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther){
	int k = type-1;
	if(KEYBOARD_NONE == type){
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
			_deleteTouchs(k);
			_deleteTouchParams(k);
		}

		s[k].shape = shape;
		s[k].bold = bold;
		s[k].x = x;
		s[k].y = y;
		s[k].widthKey = widthKey;
		s[k].heightKey = heightKey;
		s[k].interSpace = interSpace;
		s[k].forTouchIdx = forTouchIdx;  //nigdzie nie uzywany !!!!!
		s[k].startTouchIdx = startTouchIdx;
		s[k].nmbTouch = 0;
		s[k].param = 0;
		s[k].param2 = 0;
	}
	return 0;
}

/* ----- User Function Definitions ----- */


void KEYBOARD_Buttons(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int TOUCH_Release, int TOUCH_Action, char* txtDescr)
{
	XY_Touch_Struct posKey[_GetPosKeySize()];
	int head = GetHeightHead(k);

	int interSp = 4;

	SetDimKey(k,shape,widthKey,heightKey);
	SetPosKey(k,posKey,interSp,head);
	SetDimAll(k,interSp,head);

	if(TOUCH_Release == selBlockPress)				KeysAllRelease(k, posKey, txtDescr);
	else{	 INIT(nr,selBlockPress-TOUCH_Action);	_KeyStrPressDisp_oneKey(k,posKey[nr],nr);  }

	SetTouch_CountButton(k, startTouchIdx, posKey);
}

void KEYBOARD_Select(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int TOUCH_Release, int value)
{
	int interSp = -1;
	int head = 20;

	XY_Touch_Struct posKey[_GetPosKeySize()];

	SetDimKey(k,shape,widthKey,heightKey);
	SetPosKey(k,posKey,interSp,head);
	SetDimAll(k,interSp,head);

	if(TOUCH_Release == selBlockPress)
		KeysSelectOne(k, posKey, NULL, value);

	SetTouch_Select(k, startTouchIdx, posKey);
}

void KEYBOARD_ServiceStyle(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int value)
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

	if(touchRelease == selBlockPress)
	{
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );

		fillColor = 0xFF111111;

		for(int i=0; i<countKey; ++i)
		{
			if((i==0 && value==Arial) ||
				(i==1 && value==Times_New_Roman) ||
				(i==2 && value==Comic_Saens_MS))
			{
					i<countKey-1 ? _KeyStrPress(k,posKey[i],txtKey[i],colorTxtPressKey[i]) : _KeyStrPressDisp(k,posKey[i],txtKey[i],colorTxtPressKey[i]);
				/* _KeyPress(posKey[i]); _TxtPos(posKey[i]);    i<countKey-1 ? _StrPress(txtKey[i],colorTxtPressKey[i]) : _StrPressDisp(txtKey[i],colorTxtPressKey[i]); */		/* This is the same as up */
			}
			else{	i<countKey-1 ? _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(k,posKey[i],txtKey[i],colorTxtKey[i]);
					/* _Key(posKey[i]); _TxtPos(posKey[i]);   i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
			}
		}
	}

	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouch(k,ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
	}
}










void KEYBOARD__ServiceType(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int value)
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

	if(touchRelease == selBlockPress)
	{
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );

		for(int i=0; i<countKey; ++i)
		{
			if((i==0 && value==0) ||
				(i==1 && value==1) ||
				(i==2 && value==2))
			{
					i<countKey-1 ? _KeyStrPress(k,posKey[i],txtKey[i],colorTxtPressKey[i]) : _KeyStrPressDisp(k,posKey[i],txtKey[i],colorTxtPressKey[i]);
			}
			else{	i<countKey-1 ? _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(k,posKey[i],txtKey[i],colorTxtKey[i]);
			}
		}
	}

	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouch(k,ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
	}
}

void KEYBOARD__ServiceSize(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr, int value)
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

	if(touchRelease == selBlockPress)
	{
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );
		_StrDescr(k,posHead, txtDescr, colorDescr);

		for(int i=0; i<countKey; ++i)
			_KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]);

		c.shape = s[k].shape;	s[k].shape = LCD_Rectangle;
		framePressColor = frameColor;

		for(int i=0; i<countKey2; ++i)
		{
			if((i==0 && value==0) ||
				(i==1 && value==1) ||
				(i==2 && value==2))
			{
					i<countKey2-1 ? _KeyStrPress(k,posKey2[i],txtKey2[i],colorTxtPressKey2[i]) : _KeyStrPressDisp(k,posKey2[i],txtKey2[i],colorTxtPressKey2[i]);
			}
			else{	i<countKey2-1 ? _KeyStr(k,posKey2[i],txtKey2[i],colorTxtKey2[i]) : _KeyStrDisp(k,posKey2[i],txtKey2[i],colorTxtKey2[i]);
			}
		}
		s[k].shape = c.shape;
	}
	else if(touchAction+0 == selBlockPress)	 _KeyStrPressDisp_oneBlock(k,posKey[0], txtKey[0], colorTxtPressKey[0]);
	else if(touchAction+1 == selBlockPress)	 _KeyStrPressDisp_oneBlock(k,posKey[1], txtKey[1], colorTxtPressKey[1]);

	if(startTouchIdx){
		int touch_it=0;  // touch_it and  s[k].nmbTouch is the same  !!!!
		for(int i=0; i<countKey; ++i)
			_SetTouch(k,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + touch_it++, TOUCH_GET_PER_X_PROBE, posKey[i]);

		for(int i=0; i<countKey2; ++i)
			_SetTouch(k,ID_TOUCH_POINT, s[k].startTouchIdx + touch_it++, press, posKey2[i]);
	}
}

void KEYBOARD__ServiceSizeRoll(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int nrRoll, char* txtDescr, uint32_t colorDescr, int value)
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
	uint16_t selFrame = value;

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
		LCD_TOUCH_ScrollSel_SetCalculate(nrRoll, &roll, &selFrame, 0,0,0,countKey/frameNmbVis);
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

	if(touchRelease == selBlockPress)
	{
		if(shape!=0)
		{
			#ifdef _FRAME2ROLL
				BKCOPY_VAL(heightAll_c,heightAll,head+win+spaceFrame2Roll);
				BKCOPY_VAL(widthAll_c,widthAll,widthAll+2*spaceFrame2Roll);

					 LCD_ShapeWindow( LCD_RoundRectangle,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );

					 BKCOPY_VAL(c.interSpace,s[k].interSpace,posHead.y);
				 		_StrDescr(k,posHead, txtDescr, colorDescr);
				 	 BKCOPY(s[k].interSpace,c.interSpace);

				 	 LCD_Display(0, s[k].x-spaceFrame2Roll, s[k].y-head, widthAll, heightAll);

				 BKCOPY(widthAll,widthAll_c);
				 BKCOPY(heightAll,heightAll_c);
			#else
				 LCD_ShapeWindow( LCD_RoundRectangle,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );
			#endif
		}

		if(shape==0)
			LCD_TOUCH_ScrollSel_SetCalculate(nrRoll, &roll, &selFrame, s[k].y, heightAll, s[k].heightKey, win);

		for(int i=0; i<countKey; ++i)
		{
			if(i == selFrame)
				_KeyStrPressLeft(k,posKey[i],txtKey[i],colorTxtPressKey);		/* _KeyStrPress(posKey[i],txtKey[i],colorTxtPressKey); */
			else{
				fillColor = (i%2) ? BrightDecr(fillColor_copy,0x20) : fillColor_copy;
				_KeyStrleft(k,posKey[i],txtKey[i],colorTxtKey[i]);				/*	_KeyStr(posKey[i],txtKey[i],colorTxtKey[i]); */
			}
		}
		LCD_Display(0 + roll * widthAll, s[k].x, s[k].y, widthAll, win);
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
//SPACES DISP  numeracje dac ciemniejsza aby odroznic !!!!!!!!!!
int KEYBOARD__ServiceLenOffsWin(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, int touchAction2, int touchTimer, char* txtDescr, char* txtDescr2, char* txtDescr3, char* txtDescr4, uint32_t colorDescr,FUNC_MAIN *pfunc,FUNC_MAIN_INIT)
{extern uint32_t pLcd[];
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
			LCD_TOUCH_Set(ID_TOUCH_POINT,touchAction2,press);
			s[k].nmbTouch++;

			touchTemp[0].x= win.pos.x + xPosD;
			touchTemp[1].x= touchTemp[0].x + widthtUpDown;
			touchTemp[0].y= win.pos.y + yPosUD;
			touchTemp[1].y= touchTemp[0].y + heightUpDown;
			LCD_TOUCH_Set(ID_TOUCH_POINT,touchAction2+1,press);
			s[k].nmbTouch++;

			_SetCurrPosTxt(0);
			i_posTxtTab=0;
		}
		else if(Up==param){
			CONDITION(1<i_posTxtTab && 0<posTxt_temp, i_posTxtTab-=2, i_posTxtTab--);
			_SetCurrPosTxt(posTxtTab[i_posTxtTab]);
		}

		LCD_ShapeWindow( s[k].shape, 0, width,height, 0,0, width,height, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
		posTxt_temp = LCD_TxtWin(0,width,height,fontID_descr,spaceFromFrame,spaceFromFrame,LCD_DisplayRemeberedSpacesBetweenFonts(1,pCHAR_PLCD(width*height),NULL)+_GetCurrPosTxt(),fullHight,0,fillColor,colorDescr,250,NoConstWidth).inChar;
		if(posTxt_temp){
			_SetCurrPosTxt(_GetCurrPosTxt()+posTxt_temp);
			if(i_posTxtTab<sizeof(posTxtTab)-2)
				posTxtTab[++i_posTxtTab]= _GetCurrPosTxt();
		}

		LCD_TOUCH_SusspendTouch(touchAction2);
		LCD_TOUCH_SusspendTouch(touchAction2+1);

		if(1 < i_posTxtTab || (1==i_posTxtTab && 0==posTxt_temp)){
			LCDSHAPE_Window(LCDSHAPE_Arrow,0,LCD_Arrow(ToStructAndReturn,width,height, xPosU,yPosUD, SetLineBold2Width(widthtUpDown,7), SetTriangHeightCoeff2Height(heightUpDown,3), colorDescr, colorDescr, bkColor, Up));
			LCD_TOUCH_RestoreSusspendedTouch(touchAction2);
		}
		if(0 < posTxt_temp){
			LCD_Arrow(0,width,height, xPosD,yPosUD, SetLineBold2Width(widthtUpDown,7), SetTriangHeightCoeff2Height(heightUpDown-1,3), colorDescr, colorDescr, bkColor, Down);
			LCD_TOUCH_RestoreSusspendedTouch(touchAction2+1);
		}

		LCD_Display(0, x,y, width,height);
	}
	int retVal=0;
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
		pfunc(FUNC_MAIN_ARG);	_RstFlagWin();  LCD_DisplayPart(0,win.pos.x ,win.pos.y, win.size.w, win.size.h); retVal=1;
		LCD_TOUCH_DeleteSelectTouch(touchAction2);
		LCD_TOUCH_DeleteSelectTouch(touchAction2+1);
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


	if(touchRelease == selBlockPress)
	{
			BKCOPY_VAL(frameColor_c[0],frameColor,WHITE);
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );		/* s[k].shape(0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s[k].bold), bkColor,bkColor); */
			BKCOPY(frameColor,frameColor_c[0]);
		/*	posHead.y = s[k].interSpace/2; */											_StrDescr_Xmidd_Yoffs(posHead, 0, txtDescr3, colorDescr);		/* _StrDescr(k,posHead, SL(LANG_LenOffsWin), v.FONT_COLOR_Descr); */
			posHead.y += LCD_GetFontHeight(fontID_descr)+s[k].interSpace/3;	_StrDescr_Xmidd_Yoffs(posHead, 0, txtDescr4, colorDescr);

			BKCOPY_VAL(fillColor_c[0],fillColor,BrightIncr(fillColor,0xE));
			for(int i=0; i<_NMB2KEY; ++i)
			{
				_Key(k,posKey[i]);
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
						 _KeyStr(k,posKey[i],txtKey[i],colorTxtPressKey[i]);
						BKCOPY(fillColor,fillColor_c[1]);
						BKCOPY(frameColor,frameColor_c[0]);
					}
					else _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]);
				}
				else _KeyStrDisp(k,posKey[i],txtKey[i],colorTxtKey[i]);
			}
			BKCOPY(s[k].widthKey,c.widthKey);
			BKCOPY(fillColor,fillColor_c[0]);
	}
	else if(touchAction+0 == selBlockPress) 		_OverArrowTxt_oneBlockDisp(0,outside);
	else if(touchAction+1 == selBlockPress) 		_OverArrowTxt_oneBlockDisp(1,inside);
	else if(touchAction+2 == selBlockPress)  	_OverArrowTxt_oneBlockDisp(2,LeftLeft);
	else if(touchAction+3 == selBlockPress) 	_OverArrowTxt_oneBlockDisp(3,RightRight);
	else if(touchAction+4 == selBlockPress)    _OverArrowTxt_oneBlockDisp(4,LeftLeft);
	else if(touchAction+5 == selBlockPress) 	_OverArrowTxt_oneBlockDisp(5,RightRight);
	else if(touchAction+6 == selBlockPress)  _OverArrowTxt_oneBlockDisp(6,outside);
	else if(touchAction+7 == selBlockPress) _OverArrowTxt_oneBlockDisp(7,inside);

	else if(touchAction+8 == selBlockPress){	 BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);  _KeyStrPressDisp_oneBlock(k,posKey[8],txtKey[8],colorTxtPressKey[8]);		BKCOPY(s[k].widthKey,c.widthKey); CONDITION(_IsFlagWin(),_DeleteWindows(),_CreateWindows(0,NoDirect)); }
	else if(touchAction+9 == selBlockPress){ BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);  _KeyStrPressDisp_oneBlock(k,posKey[9],txtKey[9],colorTxtPressKey[9]);		BKCOPY(s[k].widthKey,c.widthKey); CONDITION(_IsFlagWin(),_DeleteWindows(),NULL);	 _WinInfo(txtDescr);}
	else if(touchAction+10 == selBlockPress){ BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);  _KeyStrPressDisp_oneBlock(k,posKey[10],txtKey[10],colorTxtPressKey[10]);	BKCOPY(s[k].widthKey,c.widthKey); CONDITION(_IsFlagWin(),_DeleteWindows(),NULL);  _WinInfo(txtDescr2); }

	else if(touchAction+11 == selBlockPress) 	 _CreateWindows(0,Up);
	else if(touchAction+12 == selBlockPress) _CreateWindows(0,Down);

	else if(touchTimer == selBlockPress){ pfunc(FUNC_MAIN_ARG);  LCD_DisplayPart(0, MIDDLE(0,LCD_X,win2.size.w)/* win2.pos.x */, win2.pos.y, win2.size.w, win2.size.h); }




	if(startTouchIdx){
		for(int i=0; i<_NMB2KEY; ++i)
			_SetTouch(k,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);

		BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);
		 for(int i=_NMB2KEY; i<countKey; ++i)
			 _SetTouch(k,ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
		BKCOPY(s[k].widthKey,c.widthKey);
	}
	return retVal;
	#undef _NMB2KEY
}

void KEYBOARD__ServiceSliderRGB(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr, int *value, VOID_FUNCTION *pfunc)
{
	const char *txtSliders[]							= {"Red","Green","Blue"};
	const COLORS_DEFINITION colorTxtSliders[]		= {RED, GREEN, BLUE};
	const uint16_t dimSlider[] = {1,3};

	uint32_t LineColor 	= frameColor;
	uint32_t LineSelColor = COLOR_GRAY(0x77);
	uint32_t spaceTriangLine = 11;	/* DelTriang */
	int maxSliderValue = 255;
	int *pValForSlider = (int*)value;
	VOID_FUNCTION *funcForSlider = pfunc;

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

	if(touchRelease == selBlockPress)
	{
			LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor,bkColor );
			_StrDescr(k,posHead, txtDescr, colorDescr);

			for(int i=0; i<countKey; ++i){
				elemSliderPos[i] = LCD_SimpleSlider(0, widthAll,heightAll, posSlider[i].x, posSlider[i].y, ChangeElemSliderSize(s[k].widthKey,NORMAL_SLIDER_PARAM), SetSpaceTriangLineSlider(s[k].heightKey,spaceTriangLine), LineColor, LineSelColor ,colorTxtSliders[i], bkColor, SetValType(PERCENT_SCALE(*(pValForSlider+i)+1,maxSliderValue+1),Percent), NoSel);
				_StrDescr_Xmidd_Yoffs(posSlider[i],- LCD_GetFontHeight(fontID_descr), txtSliders[i], colorTxtSliders[i]);
			}
			LCD_Display(0, s[k].x, s[k].y, widthAll, heightAll);
	}
	else if(touchAction+0 == selBlockPress)		_ElemSliderPressDisp_oneBlock(k,x,posSlider[0],spaceTriangLine, ChangeElemSliderColor(RightSel,colorTxtSliders[0]),LineColor,LineSelColor,pValForSlider+0, Percent,maxSliderValue,funcForSlider);
	else if(touchAction+1 == selBlockPress)		_ElemSliderPressDisp_oneBlock(k,x,posSlider[1],spaceTriangLine, ChangeElemSliderColor(RightSel,colorTxtSliders[1]),LineColor,LineSelColor,pValForSlider+1, Percent,maxSliderValue,funcForSlider);
	else if(touchAction+2 == selBlockPress)		_ElemSliderPressDisp_oneBlock(k,x,posSlider[2],spaceTriangLine, ChangeElemSliderColor(RightSel,colorTxtSliders[2]),LineColor,LineSelColor,pValForSlider+2, Percent,maxSliderValue,funcForSlider);
	else if(touchAction+3 == selBlockPress)		_ElemSliderPressDisp_oneBlock(k,x,posSlider[0],spaceTriangLine, ChangeElemSliderColor(LeftSel, colorTxtSliders[0]),LineColor,LineSelColor,pValForSlider+0, Percent,maxSliderValue,funcForSlider);
	else if(touchAction+4 == selBlockPress)	_ElemSliderPressDisp_oneBlock(k,x,posSlider[1],spaceTriangLine, ChangeElemSliderColor(LeftSel, colorTxtSliders[1]),LineColor,LineSelColor,pValForSlider+1, Percent,maxSliderValue,funcForSlider);
	else if(touchAction+5 == selBlockPress)		_ElemSliderPressDisp_oneBlock(k,x,posSlider[2],spaceTriangLine, ChangeElemSliderColor(LeftSel, colorTxtSliders[2]),LineColor,LineSelColor,pValForSlider+2, Percent,maxSliderValue,funcForSlider);
	else if(touchAction+6 == selBlockPress)		_ElemSliderPressDisp_oneBlock(k,x,posSlider[0],spaceTriangLine, ChangeElemSliderColor(PtrSel,  colorTxtSliders[0]),LineColor,LineSelColor,pValForSlider+0, PosX,maxSliderValue,funcForSlider);
	else if(touchAction+7 == selBlockPress)	_ElemSliderPressDisp_oneBlock(k,x,posSlider[1],spaceTriangLine, ChangeElemSliderColor(PtrSel,  colorTxtSliders[1]),LineColor,LineSelColor,pValForSlider+1, PosX,maxSliderValue,funcForSlider);
	else if(touchAction+8 == selBlockPress)	_ElemSliderPressDisp_oneBlock(k,x,posSlider[2],spaceTriangLine, ChangeElemSliderColor(PtrSel,  colorTxtSliders[2]),LineColor,LineSelColor,pValForSlider+2, PosX,maxSliderValue,funcForSlider);

	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouchSlider(k,s[k].startTouchIdx, elemSliderPos[i]); //po co 'k' i 's[k].startTouchIdx'  ????!!
	}
}

void KEYBOARD__ServiceSetTxt(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, int tBig,int tBack,int tEnter,uint32_t colorDescr) // klawiatura malutka i duza na caly LCD_X z liczbami
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

	uint16_t wKey[]= {W1,W1,W1,W1,W1,W1,W1,W1,W1,W1,\
								W1,W1,W1,W1,W1,W1,W1,W1,W1,\
								WA,W1,W1,W1,W1,W1,W1,W1,WA,\
								WA,W1,WS,W1,W1,WA};

	INIT(nr,0);		INIT(width_c,0);
	#define P(x,y,offs)	{(x+1)*s[k].interSpace + offs + CONDITION(0==x, width_c=_ReturnVal(0,CONDITION(0==y,nr=0,nr++)), width_c += wKey[nr++]),		(y+1)*s[k].interSpace + y*s[k].heightKey + head}
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

	void _KeyQ2P(int nr, int act){
		if(release==act)	_Key(k,posKey[nr]);
		else	 				_KeyPressWin(k);

		char *ptrTxt = Int2Str(nr,None,1,Sign_none);
		int widthDescr = LCD_GetWholeStrPxlWidth(fontID_descr,ptrTxt,0,ConstWidth);
		int heightTxt = LCD_GetFontHeight(fontID);

		if(release==act){	 _StrDescr_XYoffs(posKey[nr], s[k].widthKey-VALPERC(widthDescr,180), VALPERC(widthDescr,35), ptrTxt, 		BrightIncr(colorDescr,0x20));
								 _Str_Xmidd_Yoffs(k,posKey[nr], s[k].heightKey-VALPERC(heightTxt,115), 								txtKey[nr],  colorTxtKey[nr]);
		}
		else{			BKCOPY_VAL(fillColor_c[0],fillColor,fillPressColor);
								_StrDescrWin_XYoffs(k,s[k].widthKey-VALPERC(widthDescr,180), VALPERC(widthDescr,35), ptrTxt, 	  BrightIncr(colorDescr,0x20));
								_StrWin_Xmidd_Yoffs(k,s[k].heightKey-VALPERC(heightTxt,115), 								  txtKey[nr], colorTxtPressKey[nr]);
						BKCOPY(fillColor,fillColor_c[0]);
		}
		if(press==act) LCD_Display(0,s[k].x+posKey[nr].x,s[k].y+posKey[nr].y,s[k].widthKey,s[k].heightKey);
	}

	int colorFillBk = BrightDecr(bkColor,0x10);
	if(touchRelease == selBlockPress)
	{
			LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameMainColor,s[k].bold), fillMainColor/*colorFillBk*/,bkColor );
			LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, s[k].interSpace,s[k].interSpace, widthFieldTxt,heightFieldTxt, SetBold2Color(BrightDecr(frameColor,0x40),s[k].bold), BrightDecr(frameColor,0x40),colorFillBk );

			fillColor = BrightIncr(fillColor,0x10);
			bkColor = colorFillBk;

			c.widthKey = s[k].widthKey;
			for(int i=0; i<countKey; ++i)
			{
				s[k].widthKey = wKey[i];
				if(STRING_CmpTxt((char*)txtKey[i],_UP)){	 		_Key(k,posKey[i]);	_PARAM_ARROW_UP;
					LCD_Arrow(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_UP.w),MIDDLE(posKey[i].y,s[k].heightKey,size_UP.h), SetLineBold2Width(size_UP.w,bold_UP), SetTriangHeightCoeff2Height(size_UP.h,coeff_UP), frameColor,frameColor,bkColor, Up);
				}
				else if(STRING_CmpTxt((char*)txtKey[i],_LF)){	_Key(k,posKey[i]);	_PARAM_ARROW_LF;
					LCD_Arrow(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_LF.w),MIDDLE(posKey[i].y,s[k].heightKey,size_LF.h), SetLineBold2Width(size_LF.w,bold_LF), SetTriangHeightCoeff2Height(size_LF.h,coeff_LF), frameColor,frameColor,bkColor, Left);
				}
				else if(STRING_CmpTxt((char*)txtKey[i],_EN)){	_Key(k,posKey[i]);	_PARAM_ARROW_EN;
					LCD_Enter(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_EN.w),MIDDLE(posKey[i].y,s[k].heightKey,size_EN.h), SetLineBold2Width(size_EN.w,bold_EN), SetTriangHeightCoeff2Height(size_EN.h,coeff_EN), frameColor,frameColor,bkColor);
					LCD_Display(0, s[k].x, s[k].y, widthAll, heightAll);
				}
				else if(STRING_CmpTxt((char*)txtKey[i],_EX)){	_KeyPress(k,posKey[i]);	_PARAM_ARROW_EX;
					LCD_Exit(0,widthAll,heightAll, MIDDLE(posKey[i].x,s[k].widthKey,size_EX.w),MIDDLE(posKey[i].y,s[k].heightKey,size_EX.h), size_EX.w, size_EX.h, colorTxtPressKey[i],colorTxtPressKey[i],bkColor);
				}
				else{
					if(i<dimKeys[0]) _KeyQ2P(i,release);
					else 				  _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]);
				}
			}
			s[k].widthKey = c.widthKey;
	}
	else if(tBig == selBlockPress){
		nr = selBlockPress-touchAction;
		BKCOPY_VAL(c.widthKey,s[k].widthKey,wKey[nr]);		_PARAM_ARROW_UP;
		_KeyShapePressDisp_oneBlock(k,posKey[nr], LCDSHAPE_Arrow, LCD_Arrow(ToStructAndReturn,s[k].widthKey,s[k].heightKey, MIDDLE(0,s[k].widthKey,size_UP.w),MIDDLE(0,s[k].heightKey,size_UP.h), SetLineBold2Width(size_UP.w,bold_UP), SetTriangHeightCoeff2Height(size_UP.h,coeff_UP), colorTxtPressKey[nr],colorTxtPressKey[nr],bkColor, Up));
		BKCOPY(s[k].widthKey,c.widthKey);
	}
	else if(tBack == selBlockPress){
		nr = selBlockPress-touchAction;
		BKCOPY_VAL(c.widthKey,s[k].widthKey,wKey[nr]);		_PARAM_ARROW_LF;
		_KeyShapePressDisp_oneBlock(k,posKey[nr], LCDSHAPE_Arrow, LCD_Arrow(ToStructAndReturn,s[k].widthKey,s[k].heightKey, MIDDLE(0,s[k].widthKey,size_LF.w),MIDDLE(0,s[k].heightKey,size_LF.h), SetLineBold2Width(size_LF.w,bold_LF), SetTriangHeightCoeff2Height(size_LF.h,coeff_LF), colorTxtPressKey[nr],colorTxtPressKey[nr],bkColor, Left));
		BKCOPY(s[k].widthKey,c.widthKey);
	}
	else if(tEnter == selBlockPress){
		nr = selBlockPress-touchAction;
		BKCOPY_VAL(c.widthKey,s[k].widthKey,wKey[nr]);		_PARAM_ARROW_EN;
		_KeyShapePressDisp_oneBlock(k,posKey[nr], LCDSHAPE_Enter, LCD_Enter(ToStructAndReturn,s[k].widthKey,s[k].heightKey, MIDDLE(0,s[k].widthKey,size_EN.w),MIDDLE(0,s[k].heightKey,size_EN.h), SetLineBold2Width(size_EN.w,bold_EN), SetTriangHeightCoeff2Height(size_EN.h,coeff_EN), colorTxtPressKey[nr],colorTxtPressKey[nr],bkColor));
		BKCOPY(s[k].widthKey,c.widthKey);
	}
	else{
		  	  if(IS_RANGE(selBlockPress,touchAction,touchAction+9)){	 INIT(nr,selBlockPress-touchAction); BKCOPY_VAL(c.widthKey,s[k].widthKey,wKey[nr]);   _KeyQ2P(nr,press);  																		BKCOPY(s[k].widthKey,c.widthKey); }
		else if(IS_RANGE(selBlockPress,touchAction+10,tEnter)){ 	    INIT(nr,selBlockPress-touchAction); BKCOPY_VAL(c.widthKey,s[k].widthKey,wKey[nr]);   _KeyStrPressDisp_oneBlock(k,posKey[nr],txtKey[nr],colorTxtPressKey[nr]); 	BKCOPY(s[k].widthKey,c.widthKey); }
	}




	if(startTouchIdx){
		for(int i=0; i<countKey; ++i){
			BKCOPY_VAL(c.widthKey,s[k].widthKey,wKey[i]);
			_SetTouch(k,ID_TOUCH_POINT,s[k].startTouchIdx+i,press,posKey[i]);
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
