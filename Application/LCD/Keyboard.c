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





void _ServiceRGB__(int k, int selBlockPress, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther, \
						int touchRelease, int touchAction, char* txtDescr, uint32_t colorDescr)
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


	if(touchRelease == selBlockPress){
		LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
		_StrDescr(k,posHead, txtDescr/*SL(LANG_nazwa_8)*/, colorDescr/*v.FONT_COLOR_Descr*/);

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




//	switch((int)selBlockPress)
//	{
//		case touchRelease://KEY_All_release:
//			LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
//			_StrDescr(k,posHead, txtDescr/*SL(LANG_nazwa_8)*/, colorDescr/*v.FONT_COLOR_Descr*/);
//
//			for(int i=0; i<countKey; ++i){
//				i<countKey-1 ? _KeyStr(k,posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(k,posKey[i],txtKey[i],colorTxtKey[i]);
//			/*	_Key(posKey[i]);
//				_TxtPos(k,posKey[i]);		i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(k,txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
//			}
//			break;
//
//		case touchAction:/*KEY_Red_plus:*/	 _KeyStrPressDisp_oneBlock(k,posKey[0], txtKey[0], colorTxtPressKey[0]);	break;	/* Wykorzystac to dla przesuwu dotyku i pozostawienie sladu :) */
//		case touchAction+1:/*KEY_Green_plus:*/	 _KeyStrPressDisp_oneBlock(k,posKey[1], txtKey[1], colorTxtPressKey[1]);	break;
//		case touchAction+2:/*KEY_Blue_plus:*/	 _KeyStrPressDisp_oneBlock(k,posKey[2], txtKey[2], colorTxtPressKey[2]);	break;
//		case touchAction+3:/*KEY_Red_minus:*/ 	 _KeyStrPressDisp_oneBlock(k,posKey[3], txtKey[3], colorTxtPressKey[3]);	break;
//		case touchAction+4:/*KEY_Green_minus:*/ _KeyStrPressDisp_oneBlock(k,posKey[4], txtKey[4], colorTxtPressKey[4]);	break;
//		case touchAction+5:/*KEY_Blue_minus:*/  _KeyStrPressDisp_oneBlock(k,posKey[5], txtKey[5], colorTxtPressKey[5]);	break;
//	}

	if(startTouchIdx){
		for(int i=0; i<countKey; ++i)
			_SetTouch(k,ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
	}
}


