/*
 * Keyboard.c
 *
 *  Created on: Sep 8, 2024
 *      Author: maraf
 */

#include "Keyboard.h"
#include "LCD_BasicGaphics.h"
//#include "LCD_fonts_images.h"
//#include "LCD_BasicGaphics.h"
#include "touch.h"
#include "SCREEN_ReadPanel.h"
#include "common.h"

#define MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY		20

KEYBOARD_SETTINGS s[MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY]={0}, c={0};

int fontID 			= 0;//v.FONT_ID_Press;  //to dac jako strucy Keyboard
int fontID_descr	= 0;//v.FONT_ID_Descr;
int frameColor 	= 0;//v.COLOR_Frame;
int fillColor 		= 0;//v.COLOR_FillFrame;
int framePressColor 	= 0;//v.COLOR_FramePress;
int fillPressColor 	= 0;//v.COLOR_FillFramePress;
int bkColor 		= 0;//v.COLOR_BkScreen;

int frameColor_c[5]={0}, fillColor_c[5]={0}, bkColor_c[5]={0};
uint16_t widthAll = 0, widthAll_c = 0;
uint16_t heightAll = 0, heightAll_c = 0;

void Wpisz__(int a1,int a2,int a3,int a4,int a5,int a6,int a7){
	fontID = a1;
	fontID_descr = a2;
	frameColor = a3;
	fillColor = a4;
	framePressColor = a5;
	fillPressColor = a6;
	bkColor = a7;
}


void _deleteAllTouchs(void){
	for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
		for(int i=0; i<s[j].nmbTouch; ++i)
			LCD_TOUCH_DeleteSelectTouch(s[j].startTouchIdx+i);
	}
}
void _deleteAllTouchParams(void){
	for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
		s[j].forTouchIdx = 0;  //NoTouch
		s[j].nmbTouch = 0;
	}
}
void _deleteTouchs(int nr){
	for(int i=0; i<s[nr].nmbTouch; ++i)
		LCD_TOUCH_DeleteSelectTouch(s[nr].startTouchIdx+i);
}
void _deleteTouchParams(int nr){
	s[nr].forTouchIdx = 0;  //NoTouch
	s[nr].nmbTouch = 0;
}

int _startUp(int type, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther){
	int k = type-1;
	if(0 == type){   //KEYBOARD_none
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

void _Str(const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _StrDescr_XYoffs(XY_Touch_Struct pos,int offsX,int offsY, const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr, pos.x+offsX, pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _StrDescrWin_XYoffs(int nr,int offsX,int offsY, const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,s[nr].widthKey,s[nr].heightKey,fontID_descr, offsX,offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _Str_Xmidd_Yoffs(int nr,XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[nr].widthKey),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt), pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _StrWin_Xmidd_Yoffs(int nr,int offsY, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(0,s[nr].widthKey),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,s[nr].widthKey, s[nr].heightKey,fontID, GET_X((char*)txt),offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _StrLeft(int nr, const char *txt, XY_Touch_Struct pos, uint32_t color){
	LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[nr].heightKey));
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+LCD_GetFontHeight(fontID)/2, GET_Y, (char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _StrDescr_Xmidd_Yoffs(XY_Touch_Struct pos,int offsY, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(0,widthAll),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),	pos.y+offsY, 	(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
}
void _StrDescr(int nr,XY_Touch_Struct pos, const char *txt, uint32_t color){
	LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(pos.x,widthAll),NULL,0,NoConstWidth);
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),		s[nr].interSpace,		(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
}
void _StrPress(const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
}
void _StrPressLeft(int nr,const char *txt, XY_Touch_Struct pos, uint32_t color){
	LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[nr].heightKey));
	LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+10,GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
}
void _StrDisp(int nr,const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindowIndirect(0, s[nr].x, s[nr].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
}
void _StrPressDisp(int nr,const char *txt, uint32_t color){
	LCD_StrDependOnColorsWindowIndirect(0, s[nr].x, s[nr].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
}
void _TxtPos(int nr,XY_Touch_Struct pos){
	LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[nr].widthKey),NULL,0,NoConstWidth);
	LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[nr].heightKey));
}
void _Key(int nr,XY_Touch_Struct pos){
	LCD_ShapeWindow( s[nr].shape, 0, widthAll,heightAll, pos.x,pos.y, s[nr].widthKey,s[nr].heightKey, SetBold2Color(frameColor,s[nr].bold),fillColor,bkColor);
}
void _KeyPressWin(int nr){
	LCD_ShapeWindow( s[nr].shape, 0, s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
}
void _KeyStr(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color){
	_Key(nr,pos);
	_TxtPos(nr,pos);
	_Str(txt,color);
}
void _KeyStrleft(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color){
	_Key(nr,pos);
	_StrLeft(nr,txt,pos,color);
}
void _KeyStrDisp(int nr,XY_Touch_Struct pos,const char *txt, uint32_t color){
	_Key(nr, pos);
	_TxtPos(nr,pos);
	_StrDisp(nr,txt,color);
}
void _KeyPress(int nr, XY_Touch_Struct pos){
	LCD_ShapeWindow( s[nr].shape, 0, widthAll,heightAll, pos.x,pos.y, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
}
void _KeyStrPress(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	_KeyPress(nr,pos);
	_TxtPos(nr,pos);
	_StrPress(txt,colorTxt);
}
void _KeyStrPressLeft(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	_KeyPress(nr,pos);
	_StrPressLeft(nr,txt,pos,colorTxt);
}
void _KeyStrPressDisp(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	_KeyPress(nr,pos);
	_TxtPos(nr,pos);
	_StrPressDisp(nr,txt,colorTxt);
}
void _KeyStrPressDisp_oneBlock(int nr, XY_Touch_Struct pos, const char *txt, uint32_t colorTxt){
	LCD_ShapeWindow( s[nr].shape, 0, s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
	_TxtPos(nr,(XY_Touch_Struct){0});
	LCD_StrDependOnColorsWindowIndirect(0, s[nr].x+pos.x, s[nr].y+pos.y, s[nr].widthKey, s[nr].heightKey,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, colorTxt,255, NoConstWidth);
}
void _KeyShapePressDisp_oneBlock(int nr, XY_Touch_Struct pos, ShapeFunc pShape, SHAPE_PARAMS param){
	LCD_ShapeWindow( s[nr].shape, 0, s[nr].widthKey,s[nr].heightKey, 0,0, s[nr].widthKey,s[nr].heightKey, SetBold2Color(framePressColor,s[nr].bold),fillPressColor,bkColor);
	pShape(0,param);
	LCD_Display(0, s[nr].x+pos.x, s[nr].y+pos.y, s[nr].widthKey, s[nr].heightKey);
}

void _SetTouchSlider(int nr,uint16_t idx, SHAPE_PARAMS posElemSlider){
	for(int i=0; i<NMB_SLIDER_ELEMENTS; ++i){
		touchTemp[0].x= s[nr].x + posElemSlider.pos[i].x;
		touchTemp[1].x= touchTemp[0].x + posElemSlider.size[i].w;
		touchTemp[0].y= s[nr].y + posElemSlider.pos[i].y;
		touchTemp[1].y= touchTemp[0].y + posElemSlider.size[i].h;

		switch(i){ case 0: case 2: LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, idx + s[nr].nmbTouch++, TOUCH_GET_PER_X_PROBE   ); break;
					  case 1:			LCD_TOUCH_Set(ID_TOUCH_GET_ANY_POINT,				idx + s[nr].nmbTouch++, TOUCH_GET_PER_ANY_PROBE ); break; }
		}
}
void _ElemSliderPressDisp_oneBlock(int nr, uint16_t x, XY_Touch_Struct posSlider, uint32_t spaceTringLine, int selElem, uint32_t lineColor, uint32_t lineSelColor, int *pVal, int valType, int maxSlidVal, VOID_FUNCTION *pfunc){
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
void _SetTouch(int nr, uint16_t ID, uint16_t idx, uint8_t paramTouch, XY_Touch_Struct pos){
	touchTemp[0].x= s[nr].x + pos.x;
	touchTemp[1].x= touchTemp[0].x + s[nr].widthKey;
	touchTemp[0].y= s[nr].y + pos.y;
	touchTemp[1].y= touchTemp[0].y + s[nr].heightKey;
	LCD_TOUCH_Set(ID,idx,paramTouch);
	s[nr].nmbTouch++;
}



void KEYBOARD_SetPosKey(int nr, XY_Touch_Struct pos[], const uint16_t dim[], int head){
	int d=0;
	for(int j=0; j<dim[1]; ++j){
		for(int i=0; i<dim[0]; ++i){
			pos[d].x = (i+1)*s[nr].interSpace + i*s[nr].widthKey;
			pos[d].y = (j+1)*s[nr].interSpace + j*s[nr].heightKey + head;
			d++;
	}}
/*
	XY_Touch_Struct posKey[]=
		{{1*s[k].interSpace + 0*s[k].widthKey,	1*s[k].interSpace + 0*s[k].heightKey + head},
		{2*s[k].interSpace + 1*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
		{3*s[k].interSpace + 2*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
		\
		{1*s[k].interSpace + 0*s[k].widthKey,	2*s[k].interSpace + 1*s[k].heightKey + head},
		{2*s[k].interSpace + 1*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head},
		{3*s[k].interSpace + 2*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head}};
*/
}
int KEYBOARD_GetHead(int nr){
	return s[nr].interSpace + LCD_GetFontHeight(fontID_descr) + s[nr].interSpace;
}
void KEYBOARD_SetDimKey(int nr, figureShape shape, uint16_t width, uint16_t height, const char *txt){
	if(shape!=0){
		if(KeysAutoSize == width){
			s[nr].widthKey =  height + LCD_GetWholeStrPxlWidth(fontID,(char*)txt,0,NoConstWidth) + height;		/*	space + text + space */
			s[nr].heightKey = height + LCD_GetFontHeight(fontID) + height;
	}}
}
void KEYBOARD_SetDimAll(int nr, const uint16_t dim[], int head){
	widthAll =  dim[0]*s[nr].widthKey  + (dim[0]+1)*s[nr].interSpace;
	heightAll = dim[1]*s[nr].heightKey + (dim[1]+1)*s[nr].interSpace + head;
}
void KEYBOARD_KeysAllRelease(int nr, XY_Touch_Struct posKeys[], const char *txtKeys[], const COLORS_DEFINITION colorTxtKeys[], const uint16_t dim[], char* headTxt, uint32_t colorDescr){
	int countKey = dim[0]*dim[1];
	XY_Touch_Struct posHead={0,0};
	LCD_ShapeWindow( s[nr].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[nr].bold), bkColor,bkColor );
	_StrDescr(nr,posHead, headTxt, colorDescr);

	for(int i=0; i<countKey; ++i){
		i<countKey-1 ? _KeyStr(nr,posKeys[i],txtKeys[i],colorTxtKeys[i]) : _KeyStrDisp(nr,posKeys[i],txtKeys[i],colorTxtKeys[i]);
	/*	_Key(posKey[i]);
		_TxtPos(k,posKey[i]);		i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(k,txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
	}
}
void KEYBOARD_SetTouch(int nr, uint16_t startTouchIdx, const uint16_t dim[], XY_Touch_Struct posKeys[]){
	int countKey = dim[0]*dim[1];	/* = STRUCT_TAB_SIZE(txtKey); */
	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouch(nr,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[nr].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKeys[i]);
	}
}





void _ServiceRGB__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr)
{
	const char *txtKey[]								= {"R+",	  "G+", 	  "B+",		 "R-", 	    "G-",	  "B-"};
	const COLORS_DEFINITION colorTxtKey[]		= {RED,	  GREEN, 	BLUE,		 RED, 	    GREEN,	 BLUE};
	const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,DARKRED, LIGHTGREEN,LIGHTGREEN, DARKBLUE,DARKBLUE};
	const uint16_t dimKeys[] = {3,2};

	if(shape!=0){
		if(KeysAutoSize == widthKey){
			s[k].widthKey =  heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[0],0,NoConstWidth) + heightKey;		/*	space + text + space */
			s[k].heightKey = heightKey + LCD_GetFontHeight(fontID) + heightKey;
	}}
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

	if(touchRelease == selBlockPress){
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
		_StrDescr(k,posHead, txtDescr, colorDescr);

		for(int i=0; i<countKey; ++i){
			i<countKey-1 ? _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(k,posKey[i],txtKey[i],colorTxtKey[i]);
		/*	_Key(posKey[i]);
			_TxtPos(k,posKey[i]);		i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(k,txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
		}
	}
	else if(touchAction+0 == selBlockPress) 	_KeyStrPressDisp_oneBlock(k,posKey[0], txtKey[0], colorTxtPressKey[0]);
	else if(touchAction+1 == selBlockPress) 	_KeyStrPressDisp_oneBlock(k,posKey[1], txtKey[1], colorTxtPressKey[1]);
	else if(touchAction+2 == selBlockPress) 	_KeyStrPressDisp_oneBlock(k,posKey[2], txtKey[2], colorTxtPressKey[2]);
	else if(touchAction+3 == selBlockPress) 	_KeyStrPressDisp_oneBlock(k,posKey[3], txtKey[3], colorTxtPressKey[3]);
	else if(touchAction+4 == selBlockPress) 	_KeyStrPressDisp_oneBlock(k,posKey[4], txtKey[4], colorTxtPressKey[4]);
	else if(touchAction+5 == selBlockPress) 	_KeyStrPressDisp_oneBlock(k,posKey[5], txtKey[5], colorTxtPressKey[5]);

	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouch(k,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
	}
}

void _ServiceCoeff__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr)
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

	if(touchRelease == selBlockPress){
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
		_StrDescr(k,posHead, txtDescr, colorDescr);

		fillColor = BrightIncr(fillColor,0xE);
		for(int i=0; i<countKey; ++i){
			i<countKey-1 ? _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(k,posKey[i],txtKey[i],colorTxtKey[i]);
		}

	}
	else if(touchAction+0 == selBlockPress)	  _KeyStrPressDisp_oneBlock(k,posKey[0], txtKey[0], colorTxtPressKey[0]);
	else if(touchAction+1 == selBlockPress)  _KeyStrPressDisp_oneBlock(k,posKey[1], txtKey[1], colorTxtPressKey[1]);

	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouch(k,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
	}
}


void _ServiceStyle__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int value)
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
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );

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

void _ServiceType__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int value)
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
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );

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

void _ServiceSize__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr, int value)
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
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
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

void _ServiceSizeRoll__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int nrRoll, char* txtDescr, uint32_t colorDescr, int value)
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

					 LCD_ShapeWindow( LCD_RoundRectangle,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );

					 BKCOPY_VAL(c.interSpace,s[k].interSpace,posHead.y);
				 		_StrDescr(k,posHead, txtDescr, colorDescr);
				 	 BKCOPY(s[k].interSpace,c.interSpace);

				 	 LCD_Display(0, s[k].x-spaceFrame2Roll, s[k].y-head, widthAll, heightAll);

				 BKCOPY(widthAll,widthAll_c);
				 BKCOPY(heightAll,heightAll_c);
			#else
				 LCD_ShapeWindow( LCD_RoundRectangle,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
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
extern uint32_t pLcd[];
void _ServiceLenOffsWin__(int k, int selBlockPress, INIT_KEYBOARD_PARAM, int touchRelease, int touchAction, int touchAction2, int touchTimer, char* txtDescr, char* txtDescr2, uint32_t colorDescr)
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
			LCDSHAPE_Window(LCDSHAPE_Arrow,0,LCD_Arrow(ToStructAndReturn,width,height, xPosU,yPosUD, SetLineBold2Width(widthtUpDown,7), SetTriangHeightCoeff2Height(heightUpDown,3), colorDescr, colorDescr, 0/*v.COLOR_BkScreen*/, Up));
			LCD_TOUCH_RestoreSusspendedTouch(touchAction2);
		}
		if(0 < posTxt_temp){
			LCD_Arrow(0,width,height, xPosD,yPosUD, SetLineBold2Width(widthtUpDown,7), SetTriangHeightCoeff2Height(heightUpDown-1,3), colorDescr, colorDescr, 0/*v.COLOR_BkScreen*/, Down);
			LCD_TOUCH_RestoreSusspendedTouch(touchAction2+1);
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
		/*FILE_NAME(main)(LoadNoDispScreen,(char**)ppMain);*/	_RstFlagWin();  LCD_DisplayPart(0,win.pos.x ,win.pos.y, win.size.w, win.size.h); /*SELECT_CURRENT_FONT(LenWin,Press, TXT_LENOFFS_WIN,255)*/;
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
				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );		/* s[k].shape(0,widthAll,heightAll, 0,0, widthAll,heightAll, SetColorBoldFrame(frameColor,s[k].bold), bkColor,bkColor); */
			BKCOPY(frameColor,frameColor_c[0]);
		/*	posHead.y = s[k].interSpace/2; */											_StrDescr_Xmidd_Yoffs(posHead, 0, "a"/*SL(LANG_LenOffsWin1)*/, colorDescr);		/* _StrDescr(k,posHead, SL(LANG_LenOffsWin), v.FONT_COLOR_Descr); */
			posHead.y += LCD_GetFontHeight(fontID_descr)+s[k].interSpace/3;	_StrDescr_Xmidd_Yoffs(posHead, 0, "a"/*SL(LANG_LenOffsWin2)*/, colorDescr);

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

	else if(touchTimer == selBlockPress){ /*FILE_NAME(main)(LoadNoDispScreen,(char**)ppMain);*/  LCD_DisplayPart(0, MIDDLE(0,LCD_X,win2.size.w)/* win2.pos.x */, win2.pos.y, win2.size.w, win2.size.h); }




	if(startTouchIdx){
		for(int i=0; i<_NMB2KEY; ++i)
			_SetTouch(k,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);

		BKCOPY_VAL(c.widthKey,s[k].widthKey,s[k].widthKey2);
		 for(int i=_NMB2KEY; i<countKey; ++i)
			 _SetTouch(k,ID_TOUCH_POINT, s[k].startTouchIdx + i, press, posKey[i]);
		BKCOPY(s[k].widthKey,c.widthKey);
	}
	#undef _NMB2KEY
}

