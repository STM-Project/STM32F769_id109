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
		s[k].forTouchIdx = forTouchIdx;
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

