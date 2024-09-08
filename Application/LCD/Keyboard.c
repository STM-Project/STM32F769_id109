/*
 * Keyboard.c
 *
 *  Created on: Sep 8, 2024
 *      Author: maraf
 */

#include "stm32f7xx_hal.h"

#include "Keyboard.h"
#include "common.h"
#include "LCD_fonts_images.h"
#include "LCD_BasicGaphics.h"
#include "touch.h"

#define MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY		20

void _ServiceRGB__(void)
{
//		const char *txtKey[]								= {"R+",	  "G+", 	  "B+",		 "R-", 	    "G-",	  "B-"};
//		const COLORS_DEFINITION colorTxtKey[]		= {RED,	  GREEN, 	BLUE,		 RED, 	    GREEN,	 BLUE};
//		const COLORS_DEFINITION colorTxtPressKey[]= {DARKRED,DARKRED, LIGHTGREEN,LIGHTGREEN, DARKBLUE,DARKBLUE};
//		const uint16_t dimKeys[] = {3,2};
//
//		if(shape!=0){
//			if(KeysAutoSize == widthKey){
//				s[k].widthKey =  heightKey + LCD_GetWholeStrPxlWidth(fontID,(char*)txtKey[0],0,NoConstWidth) + heightKey;		/*	space + text + space */
//				s[k].heightKey = heightKey + LCD_GetFontHeight(fontID) + heightKey;
//			}
//		}
//		int head = s[k].interSpace + LCD_GetFontHeight(fontID_descr) + s[k].interSpace;
//
//		XY_Touch_Struct posHead={0,0};
//		XY_Touch_Struct posKey[]=
//		  {{1*s[k].interSpace + 0*s[k].widthKey,	1*s[k].interSpace + 0*s[k].heightKey + head},
//			{2*s[k].interSpace + 1*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
//			{3*s[k].interSpace + 2*s[k].widthKey, 	1*s[k].interSpace + 0*s[k].heightKey + head},
//			\
//			{1*s[k].interSpace + 0*s[k].widthKey,	2*s[k].interSpace + 1*s[k].heightKey + head},
//			{2*s[k].interSpace + 1*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head},
//			{3*s[k].interSpace + 2*s[k].widthKey, 	2*s[k].interSpace + 1*s[k].heightKey + head}};
//
//		int countKey = dimKeys[0]*dimKeys[1]; 		/* = STRUCT_TAB_SIZE(txtKey); */
//
//		widthAll =  dimKeys[0]*s[k].widthKey  + (dimKeys[0]+1)*s[k].interSpace;
//		heightAll = dimKeys[1]*s[k].heightKey + (dimKeys[1]+1)*s[k].interSpace + head;
//
//		switch((int)selBlockPress)
//		{
//			case KEY_All_release:
//				LCD_ShapeWindow( s[k].shape,0,widthAll,heightAll, 0,0, widthAll,heightAll, SetBold2Color(frameColor,s[k].bold), bkColor,bkColor );
//				_StrDescr(posHead, SL(LANG_nazwa_8), v.FONT_COLOR_Descr);
//
//				for(int i=0; i<countKey; ++i){
//					i<countKey-1 ? _KeyStr(posKey[i],txtKey[i],colorTxtKey[i]) : _KeyStrDisp(posKey[i],txtKey[i],colorTxtKey[i]);
//				/*	_Key(posKey[i]);
//					_TxtPos(posKey[i]);		i<countKey-1 ? _Str(txtKey[i],colorTxtKey[i]) : _StrDisp(txtKey[i],colorTxtKey[i]); */		/* This is the same as up */
//				}
//				break;
//
//			case KEY_Red_plus:	 _KeyStrPressDisp_oneBlock(posKey[0], txtKey[0], colorTxtPressKey[0]);	break;	/* Wykorzystac to dla przesuwu dotyku i pozostawienie sladu :) */
//			case KEY_Green_plus:	 _KeyStrPressDisp_oneBlock(posKey[1], txtKey[1], colorTxtPressKey[1]);	break;
//			case KEY_Blue_plus:	 _KeyStrPressDisp_oneBlock(posKey[2], txtKey[2], colorTxtPressKey[2]);	break;
//			case KEY_Red_minus: 	 _KeyStrPressDisp_oneBlock(posKey[3], txtKey[3], colorTxtPressKey[3]);	break;
//			case KEY_Green_minus: _KeyStrPressDisp_oneBlock(posKey[4], txtKey[4], colorTxtPressKey[4]);	break;
//			case KEY_Blue_minus:  _KeyStrPressDisp_oneBlock(posKey[5], txtKey[5], colorTxtPressKey[5]);	break;
//		}
//
//		if(startTouchIdx){
//			for(int i=0; i<countKey; ++i)
//				_SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT, s[k].startTouchIdx + i, TOUCH_GET_PER_X_PROBE, posKey[i]);
//		}
}

void Funkcje_nie_tu(int type){



	/* ----- User Function Definitions ----- */






	/* ----- End User Function Definitions ----- */

	switch(type)
	{
		case 1:   //KEYBOARD_fontRGB
		case 2:   //KEYBOARD_bkRGB
			_ServiceRGB__();
			break;

		default:
			break;
	}
}





int KeyboardTypeDisplay__(int type, int key, figureShape shape, uint8_t bold, uint16_t x, uint16_t y, uint16_t widthKey, uint16_t heightKey, uint8_t interSpace, uint16_t forTouchIdx, uint16_t startTouchIdx, uint8_t eraseOther)
{
	int fontID 			= 0;//v.FONT_ID_Press;
	int fontID_descr	= 0;// v.FONT_ID_Descr;
	int frameColor 	= 0;// v.COLOR_Frame;
	int fillColor 		= 0;// v.COLOR_FillFrame;
	int framePressColor 	=  0;//v.COLOR_FramePress;
	int fillPressColor 	= 0;// v.COLOR_FillFramePress;
	int bkColor 		= 0;// v.COLOR_BkScreen;

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
	void _StrDescr_XYoffs(structPosition pos,int offsX,int offsY, const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr, pos.x+offsX, pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrDescrWin_XYoffs(int offsX,int offsY, const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,s[k].widthKey,s[k].heightKey,fontID_descr, offsX,offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _Str_Xmidd_Yoffs(structPosition pos,int offsY, const char *txt, uint32_t color){
		LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[k].widthKey),NULL,0,NoConstWidth);
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt), pos.y+offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrWin_Xmidd_Yoffs(int offsY, const char *txt, uint32_t color){
		LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(0,s[k].widthKey),NULL,0,NoConstWidth);
		LCD_StrDependOnColorsWindow(0,s[k].widthKey, s[k].heightKey,fontID, GET_X((char*)txt),offsY,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrLeft(const char *txt, structPosition pos, uint32_t color){
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[k].heightKey));
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+LCD_GetFontHeight(fontID)/2, GET_Y, (char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrDescr_Xmidd_Yoffs(structPosition pos,int offsY, const char *txt, uint32_t color){
		LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(0,widthAll),NULL,0,NoConstWidth);
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),	pos.y+offsY, 	(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
	}
	void _StrDescr(structPosition pos, const char *txt, uint32_t color){
		LCD_Xmiddle(MIDDLE_NR+1,SetPos,SetPosAndWidth(pos.x,widthAll),NULL,0,NoConstWidth);
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID_descr,		LCD_Xmiddle(MIDDLE_NR+1,GetPos,fontID_descr,(char*)txt,0,NoConstWidth),		s[k].interSpace,		(char*)txt, fullHight, 0, bkColor, color,250, NoConstWidth);
	}
	void _StrPress(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _StrPressLeft(const char *txt, structPosition pos, uint32_t color){
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[k].heightKey));
		LCD_StrDependOnColorsWindow(0,widthAll,heightAll,fontID, pos.x+10,GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _StrDisp(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindowIndirect(0, s[k].x, s[k].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillColor, color,250, NoConstWidth);
	}
	void _StrPressDisp(const char *txt, uint32_t color){
		LCD_StrDependOnColorsWindowIndirect(0, s[k].x, s[k].y, widthAll,heightAll,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, color,250, NoConstWidth);
	}
	void _TxtPos(structPosition pos){
		LCD_Xmiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.x,s[k].widthKey),NULL,0,NoConstWidth);
		LCD_Ymiddle(MIDDLE_NR,SetPos,SetPosAndWidth(pos.y,s[k].heightKey));
	}
	void _Key(structPosition pos){
		LCD_ShapeWindow( s[k].shape, 0, widthAll,heightAll, pos.x,pos.y, s[k].widthKey,s[k].heightKey, SetBold2Color(frameColor,s[k].bold),fillColor,bkColor);
	}
	void _KeyPressWin(void){
		LCD_ShapeWindow( s[k].shape, 0, s[k].widthKey,s[k].heightKey, 0,0, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
	}
	void _KeyStr(structPosition pos,const char *txt, uint32_t color){
		_Key(pos);
		_TxtPos(pos);
		_Str(txt,color);
	}
	void _KeyStrleft(structPosition pos,const char *txt, uint32_t color){
		_Key(pos);
		_StrLeft(txt,pos,color);
	}
	void _KeyStrDisp(structPosition pos,const char *txt, uint32_t color){
		_Key(pos);
		_TxtPos(pos);
		_StrDisp(txt,color);
	}
	void _KeyPress(structPosition pos){
		LCD_ShapeWindow( s[k].shape, 0, widthAll,heightAll, pos.x,pos.y, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
	}
	void _KeyStrPress(structPosition pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_TxtPos(pos);
		_StrPress(txt,colorTxt);
	}
	void _KeyStrPressLeft(structPosition pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_StrPressLeft(txt,pos,colorTxt);
	}
	void _KeyStrPressDisp(structPosition pos, const char *txt, uint32_t colorTxt){
		_KeyPress(pos);
		_TxtPos(pos);
		_StrPressDisp(txt,colorTxt);
	}
	void _KeyStrPressDisp_oneBlock(structPosition pos, const char *txt, uint32_t colorTxt){
		LCD_ShapeWindow( s[k].shape, 0, s[k].widthKey,s[k].heightKey, 0,0, s[k].widthKey,s[k].heightKey, SetBold2Color(framePressColor,s[k].bold),fillPressColor,bkColor);
		_TxtPos((structPosition){0});
		LCD_StrDependOnColorsWindowIndirect(0, s[k].x+pos.x, s[k].y+pos.y, s[k].widthKey, s[k].heightKey,fontID, GET_X((char*)txt),GET_Y,(char*)txt, fullHight, 0, fillPressColor, colorTxt,255, NoConstWidth);
	}
	void _KeyShapePressDisp_oneBlock(structPosition pos, ShapeFunc pShape, SHAPE_PARAMS param){
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

	void _ElemSliderPressDisp_oneBlock(structPosition posSlider, uint32_t spaceTringLine, int selElem, uint32_t lineColor, uint32_t lineSelColor, int *pVal, int valType, int maxSlidVal, VOID_FUNCTION *pfunc){
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
	void _SetTouch(uint16_t ID, uint16_t idx, uint8_t paramTouch, structPosition pos){
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
			s[j].forTouchIdx = 0;  //NoTouch
			s[j].nmbTouch = 0;
		}
	}
	void _deleteTouchs(void){
		for(int i=0; i<s[k].nmbTouch; ++i)
			LCD_TOUCH_DeleteSelectTouch(s[k].startTouchIdx+i);
	}
	void _deleteTouchParams(void){
		s[k].forTouchIdx = 0;  //NoTouch
		s[k].nmbTouch = 0;
	}
	int _startUp(void){
		if(0 == type){   //KEYBOARD_none
			_deleteAllTouchs();
			_deleteAllTouchParams();
			return 1;
		}
		if(shape!=0){
			if(2 == eraseOther){  //KeysDel
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


	Funkcje_nie_tu(1);

	#undef MIDDLE_NR
	#undef GET_X
	#undef GET_Y
	return 0;
}

