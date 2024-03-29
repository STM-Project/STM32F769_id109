/*
 * LCD_BasicGaphics.c
 *
 *  Created on: 11.04.2021
 *      Author: Elektronika RM
 */

#include "LCD_BasicGaphics.h"
#include <stdarg.h>
#include <math.h>
#include "LCD_Hardware.h"
#include "LCD_Common.h"
#include <stdbool.h>

#define MAX_SIZE_TAB_AA		500   //!!!!!!!!!!  tREBA if >MAX_SIZE_TAB_AA to break; !!!! bo inaczej sa czarne kropki !!!!!
#define MAX_LINE_BUFF_CIRCLE_SIZE  500
#define MAX_DEGREE_CIRCLE  10

ALIGN_32BYTES(SDRAM uint32_t pLcd[LCD_BUFF_XSIZE*LCD_BUFF_YSIZE]);

static uint32_t k, kCopy;
static uint32_t buff_AA[MAX_SIZE_TAB_AA];
static structPosition pos;

typedef struct
{	float c1;
	float c2;
}AACoeff_RoundFrameRectangle;
static AACoeff_RoundFrameRectangle AA;

static uint8_t correctLine_AA=0;

typedef struct
{	uint8_t lineBuff[MAX_LINE_BUFF_CIRCLE_SIZE];
	float outRatioStart;
	float inRatioStart;
	float outRatioStart_prev;
	float inRatioStart_prev;
	uint16_t width;
	uint16_t width_prev;
	uint16_t x0;
	uint16_t y0;
	uint16_t degree[MAX_DEGREE_CIRCLE+1];
	uint32_t degColor[MAX_DEGREE_CIRCLE];
	float tang[MAX_DEGREE_CIRCLE];
	float coeff[MAX_DEGREE_CIRCLE];
	uint8_t rot[MAX_DEGREE_CIRCLE];
}Circle_Param;
static Circle_Param Circle;

static void _Middle_RoundRectangleFrame(int rectangleFrame, int fillHeight, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpSizeX, uint32_t width, uint32_t height){
	int _height = height-fillHeight;
	int _width = width-2;
	if(rectangleFrame)
	{
		for (int j=0; j<_height; j++)
		{
			_FillBuff(1, FrameColor);
			_FillBuff(_width, FillColor);
			_FillBuff(1, FrameColor);
			_NextDrawLine(BkpSizeX,width);
		}
	}
	else
	{
		for (int j=0; j<_height; j++)
		{
			_FillBuff(1, FrameColor);
			k+=_width;
			_FillBuff(1, FrameColor);
			_NextDrawLine(BkpSizeX,width);
		}
	}
}

static void LCD_DrawRoundRectangleFrame(int rectangleFrame, uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor)
{
	#define A(a,b) 	_FillBuff(a,b)

	uint8_t thickness = BkpColor>>24;
	uint32_t o1,o2;
	uint32_t i1 = GetTransitionColor(FrameColor,FillColor,AA.c1);
	uint32_t i2 = GetTransitionColor(FrameColor,FillColor,AA.c2);

	if((thickness==0)||(thickness==255)){
		o1 = GetTransitionColor(FrameColor,BkpColor,AA.c1);
		o2 = GetTransitionColor(FrameColor,BkpColor,AA.c2);
	}

	void _Fill(int x)
	{
		if(rectangleFrame)
			A(x,FillColor);
		else
			k+=x;
	}

	void _Out_AA_left(int stage)
	{
		if((thickness==0)||(thickness==255))
		{	switch(stage)
			{
			case 0:	A(3,BkpColor); A(1,o2); A(1,o1);  break;
			case 1:	A(2,BkpColor); A(1,o1);  break;
			case 2:	A(1,BkpColor); A(1,o1);  break;
			case 3:	A(1,o2); break;
			case 4:	A(1,o1); break;
			}
		}
		else
		{  switch(stage)
			{
			case 0:	k+=5; break;
			case 1:	k+=3; break;
			case 2:	k+=2; break;
			case 3:	k+=1; break;
			case 4:	k+=1; break;
			}
		}
	}

	void _Out_AA_right(int stage)
	{
		if((thickness==0)||(thickness==255))
		{	switch(stage)
			{
			case 0:	A(1,o1); A(1,o2); A(3,BkpColor);  break;
			case 1:	A(1,o1); A(2,BkpColor);  break;
			case 2:	A(1,o1); A(1,BkpColor);  break;
			case 3:	A(1,o2); break;
			case 4:	A(1,o1); break;
			}
		}
		else
		{	switch(stage)
			{
			case 0:	k+=5; break;
			case 1:	k+=3; break;
			case 2:	k+=2; break;
			case 3:	k+=1; break;
			case 4:	k+=1; break;
			}
		}
	}

	_StartDrawLine(posBuff,BkpSizeX,x,y);
	_Out_AA_left(0); A(width-10,FrameColor); _Out_AA_right(0);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(1); A(2,FrameColor); A(1,i1);A(1,i2); _Fill(width-14); A(1,i2);A(1,i1);A(2,FrameColor); _Out_AA_right(1);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(2); A(1,FrameColor); A(1,i1); _Fill(width-8); A(1,i1); A(1,FrameColor); _Out_AA_right(2);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(3); A(1,FrameColor); A(1,i1); _Fill(width-6); A(1,i1); A(1,FrameColor); _Out_AA_right(3);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(4); A(1,FrameColor); _Fill(width-4); A(1,FrameColor); _Out_AA_right(4);
	_NextDrawLine(BkpSizeX,width);

	A(1,FrameColor);  A(1,i1); _Fill(width-4); A(1,i1); A(1,FrameColor);
	_NextDrawLine(BkpSizeX,width);
	A(1,FrameColor);  A(1,i2); _Fill(width-4); A(1,i2); A(1,FrameColor);
	_NextDrawLine(BkpSizeX,width);

	_Middle_RoundRectangleFrame(rectangleFrame,14,FrameColor,FillColor,BkpSizeX,width,height);

	A(1,FrameColor);  A(1,i2); _Fill(width-4); A(1,i2); A(1,FrameColor);
	_NextDrawLine(BkpSizeX,width);
	A(1,FrameColor);  A(1,i1); _Fill(width-4); A(1,i1); A(1,FrameColor);
	_NextDrawLine(BkpSizeX,width);

	_Out_AA_left(4); A(1,FrameColor); _Fill(width-4); A(1,FrameColor); _Out_AA_right(4);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(3); A(1,FrameColor); A(1,i1); _Fill(width-6); A(1,i1); A(1,FrameColor); _Out_AA_right(3);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(2); A(1,FrameColor); A(1,i1); _Fill(width-8); A(1,i1); A(1,FrameColor); _Out_AA_right(2);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(1); A(2,FrameColor); A(1,i1);A(1,i2); _Fill(width-14); A(1,i2);A(1,i1);A(2,FrameColor); _Out_AA_right(1);
	_NextDrawLine(BkpSizeX,width);
	_Out_AA_left(0); A(width-10,FrameColor); _Out_AA_right(0);

	#undef  A
}

static void PixelCorrect(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint16_t pxlLen, int multiplier){
	Set_AACoeff_Draw(pxlLen,drawColor,bkColor,ratioStart);
	for(int a=0;a<pxlLen;++a)
		pLcd[k+multiplier*a]=buff_AA[1+a];
}

static void InverseAndCopyBuff(uint8_t *buf_inv, uint8_t *buf){
	buf_inv[0]=buf[0];
	for(int i=0;i<buf[0];i++)
		buf_inv[1+i]=buf[buf[0]-i];
}

static void _OffsetRightDown(uint32_t BkpSizeX, int direction, uint8_t *buf){
	int j=buf[0], i=buf[1], p=2;
	if(0==direction)
	{
		while(j--){
			k+=i;
			if(j) i=buf[p++];
			k+=BkpSizeX;
		}
	}
	else
	{
		while(j--){
			k+=i*BkpSizeX;
			if(j) i=buf[p++];
			k++;
		}
	}
}

static void _OffsetLeftDown(uint32_t BkpSizeX, int direction, uint8_t *buf){
	int j=buf[0], i=buf[1], p=2;
	if(0==direction)
	{
		while(j--){
			k-=i;
			if(j) i=buf[p++];
			k+=BkpSizeX;
		}
	}
	else
	{
		while(j--){
			k+=i*BkpSizeX;
			if(j) i=buf[p++];
			k--;
		}
	}
}

static void _OffsetLeftUp(uint32_t BkpSizeX, int direction, uint8_t *buf){
	int j=buf[0], i=buf[1], p=2;
	if(0==direction)
	{
		while(j--){
			k-=i;
			if(j) i=buf[p++];
			k-=BkpSizeX;
		}
	}
	else
	{
		while(j--){
			k-=i*BkpSizeX;
			if(j) i=buf[p++];
			k--;
		}
	}
}

static void LCD_CircleCorrect(void)
{
	//LCD_OffsCircleLine(1,1);  LCD_OffsCircleLine(2,-1);
	//LCD_OffsCircleLine(1,2);  LCD_OffsCircleLine(2,-1); LCD_OffsCircleLine(3,-1);
	//LCD_OffsCircleLine(1,4);  LCD_OffsCircleLine(2,3); LCD_OffsCircleLine(3,2);  LCD_OffsCircleLine(3,1);
}

static uint16_t GetDegFromPosK( uint32_t posBuff, uint16_t x0, uint16_t y0, uint32_t BkpSizeX)
{
	pos = _GetPosXY(posBuff,BkpSizeX);
	float deg = DEG(atan2(pos.y-y0, pos.x-x0));
	return deg<0 ? 360+deg : deg;
}

static void OffsetKfromLineBuff(int itBuff, uint32_t BkpSizeX){
	uint32_t offs= (itBuff-1)*BkpSizeX;
	for(int i=1;i<itBuff;++i){
		if(i>=MAX_LINE_BUFF_CIRCLE_SIZE) break;
		offs += Circle.lineBuff[i];
	}
	k+=offs;
}

static uint8_t LCD_SearchLinePoints(int startMeasure, int posBuff, int x0,int y0, int deg, uint32_t BkpSizeX)
{
	static float tang, coeff;

	if(startMeasure)
	{
		tang= tan(TANG_ARG(deg));
		coeff = 1/ABS(tang);

		if(deg>0 && deg<=45)
			return 1;
		else if(deg>45 && deg<=90)
			return 2;
		else if(deg>90 && deg<=135)
			return 3;
		else if(deg>135 && deg<=180)
			return 4;
		else if(deg>180 && deg<=225)
			return 5;
		else if(deg>225 && deg<=270)
			return 6;
		else if(deg>270 && deg<=315)
			return 7;
		else if(deg>315 && deg<=360)
			return 8;
	}
	else
	{
   	pos = _GetPosXY(posBuff,BkpSizeX);
   	float xxx = pos.x - x0;
		float yyy = pos.y - y0;
		float _yyy = tang*xxx;

		if(deg>0 && deg<=45)
		{
				  if(yyy<_yyy)	                                return 0;
			else if( (yyy>=_yyy) && (yyy<tang*(xxx+coeff)) )  return 1;
			else															  return 2;
		}
		else if(deg>45 && deg<=90)
		{
			  if(yyy<_yyy)	                                return 0;
		else if( (yyy>=_yyy) && (yyy<tang*(xxx+1)) )      return 1;
		else															  return 2;
		}

		else if(deg>90 && deg<=135)
		{
			  if(yyy>_yyy)	                                return 0;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+1)) )      return 1;
		else															  return 2;
		}
		else if(deg>135 && deg<=180)
		{
			  if(yyy>_yyy)	                                return 0;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+coeff)) )  return 1;
		else															  return 2;
		}

		else if(deg>180 && deg<=225)
		{
				  if(yyy<_yyy)	                                return 2;
			else if( (yyy>=_yyy) && (yyy<tang*(xxx+coeff)) )  return 1;
			else															  return 0;
		}
		else if(deg>225 && deg<=270)
		{
			  if(yyy<_yyy)	                                return 2;
		else if( (yyy>=_yyy) && (yyy<tang*(xxx+1)) )      return 1;
		else															  return 0;
		}

		else if(deg>270 && deg<=315)
		{
			  if(yyy>_yyy)	                                return 2;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+1)) )      return 1;
		else															  return 0;
		}
		else if(deg>315 && deg<=360)
		{
			  if(yyy>_yyy)	                                return 2;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+coeff)) )  return 1;
		else															  return 0;
		}

		else
			return 3;
	}
	return 0;
}

static uint8_t LCD_SearchRadiusPoints(int posBuff, int nrDeg, uint32_t BkpSizeX)
{
	float tang= Circle.tang[nrDeg];
	float coeff=Circle.coeff[nrDeg];
	uint16_t deg = Circle.degree[1+nrDeg];

   	pos = _GetPosXY(posBuff,BkpSizeX);
   	float xxx = pos.x - Circle.x0;
		float yyy = pos.y - Circle.y0;
		float _yyy = tang*xxx;

		if(deg>0 && deg<=45)
		{
				  if(yyy<_yyy)	                                return 0;
			else if( (yyy>=_yyy) && (yyy<tang*(xxx+coeff)) )  return 1;
			else															  return 2;
		}
		else if(deg>45 && deg<=90)
		{
			  if(yyy<_yyy)	                                return 0;
		else if( (yyy>=_yyy) && (yyy<tang*(xxx+1)) )      return 1;
		else															  return 2;
		}

		else if(deg>90 && deg<=135)
		{
			  if(yyy>_yyy)	                                return 0;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+1)) )      return 1;
		else															  return 2;
		}
		else if(deg>135 && deg<=180)
		{
			  if(yyy>_yyy)	                                return 0;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+coeff)) )  return 1;
		else															  return 2;
		}

		else if(deg>180 && deg<=225)
		{
				  if(yyy<_yyy)	                                return 2;
			else if( (yyy>=_yyy) && (yyy<tang*(xxx+coeff)) )  return 1;
			else															  return 0;
		}
		else if(deg>225 && deg<=270)
		{
			  if(yyy<_yyy)	                                return 2;
		else if( (yyy>=_yyy) && (yyy<tang*(xxx+1)) )      return 1;
		else															  return 0;
		}

		else if(deg>270 && deg<=315)
		{
			  if(yyy>_yyy)	                                return 2;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+1)) )      return 1;
		else															  return 0;
		}
		else if(deg>315 && deg<=360)
		{
			  if(yyy>_yyy)	                                return 2;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+coeff)) )  return 1;
		else															  return 0;
		}

		else
			return 3;
}
structPosition _GetPosXY___(uint32_t posBuff, int k, uint32_t BkpSizeX);
static uint8_t LCD_SearchRadiusPoints___(int posBuff, int k, int nrDeg, uint32_t BkpSizeX)
{
	float tang= Circle.tang[nrDeg];
	float coeff=Circle.coeff[nrDeg];
	uint16_t deg = Circle.degree[1+nrDeg];

   	pos = _GetPosXY___(posBuff,k,BkpSizeX);
   	float xxx = pos.x - Circle.x0;
		float yyy = pos.y - Circle.y0;
		float _yyy = tang*xxx;

		if(deg>0 && deg<=45)
		{
				  if(yyy<_yyy)	                                return 0;
			else if( (yyy>=_yyy) && (yyy<tang*(xxx+coeff)) )  return 1;
			else															  return 2;
		}
		else if(deg>45 && deg<=90)
		{
			  if(yyy<_yyy)	                                return 0;
		else if( (yyy>=_yyy) && (yyy<tang*(xxx+1)) )      return 1;
		else															  return 2;
		}

		else if(deg>90 && deg<=135)
		{
			  if(yyy>_yyy)	                                return 0;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+1)) )      return 1;
		else															  return 2;
		}
		else if(deg>135 && deg<=180)
		{
			  if(yyy>_yyy)	                                return 0;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+coeff)) )  return 1;
		else															  return 2;
		}

		else if(deg>180 && deg<=225)
		{
				  if(yyy<_yyy)	                                return 2;
			else if( (yyy>=_yyy) && (yyy<tang*(xxx+coeff)) )  return 1;
			else															  return 0;
		}
		else if(deg>225 && deg<=270)
		{
			  if(yyy<_yyy)	                                return 2;
		else if( (yyy>=_yyy) && (yyy<tang*(xxx+1)) )      return 1;
		else															  return 0;
		}

		else if(deg>270 && deg<=315)
		{
			  if(yyy>_yyy)	                                return 2;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+1)) )      return 1;
		else															  return 0;
		}
		else if(deg>315 && deg<=360)
		{
			  if(yyy>_yyy)	                                return 2;
		else if( (yyy<=_yyy) && (yyy>tang*(xxx+coeff)) )  return 1;
		else															  return 0;
		}

		else
			return 3;
}

static uint8_t _DrawArrayBuffRightDown_AA___(uint32_t posBuff, uint8_t *pFill, uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;
	uint8_t thershold;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				thershold=LCD_SearchRadiusPoints(posBuff,*pFill,BkpSizeX);
				if((0==thershold)||(1==thershold)){
					if(*pFill==0) *pFill=1;
					else return 1;
					pLcd[k++]=drawColor;
				}
				else{
					if(*pFill==0)
						k++;
					else
						pLcd[k++]=drawColor;
				}
			}

			if(j){
				i=buf[p++];
				if(*pFill==1){
					Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
					for(int a=0;a<buff_AA[0];++a){
						thershold=LCD_SearchRadiusPoints___(posBuff,k+a,*pFill,BkpSizeX);
						if(2==thershold)
							pLcd[k+a]=buff_AA[1+a];
						else
							break;
					}
			}}
			k+=BkpSizeX;

			if(*pFill==1){
				Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a){
					thershold=LCD_SearchRadiusPoints___(posBuff,k-1-a,*pFill,BkpSizeX);
					if(2==thershold)
						pLcd[k-1-a]=buff_AA[1+a];
					else break;
				}
			}
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				thershold=LCD_SearchRadiusPoints(posBuff,*pFill,BkpSizeX);
				if((0==thershold)||(1==thershold)){
					if(*pFill==0) *pFill=1;
					else return 1;
					pLcd[k]=drawColor;
					k+=BkpSizeX;
				}
				else{
					if(*pFill==0)
						k+=BkpSizeX;
					else{
						pLcd[k]=drawColor;
						k+=BkpSizeX;
					}
				}
			}

			if(j){
				i=buf[p++];
				if(*pFill==1){
					Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
					for(int a=0;a<buff_AA[0];++a){
						thershold=LCD_SearchRadiusPoints___(posBuff,k+a*BkpSizeX,*pFill,BkpSizeX);
						if(2==thershold)
							pLcd[k+a*BkpSizeX]=buff_AA[1+a];
						else break;
					}
			}}
			k++;

			if(*pFill==1){
				Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a){
					thershold=LCD_SearchRadiusPoints___(posBuff,k-(a+1)*BkpSizeX,*pFill,BkpSizeX);
					if(0==thershold)
						pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
					else break;
				}
			}
		}
	}
	return 0;
}

static uint8_t _DrawArrayBuffLeftDown_AA___(uint32_t posBuff, uint8_t *pFill, uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;
	uint8_t thershold;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				thershold=LCD_SearchRadiusPoints(posBuff,*pFill,BkpSizeX);
				if((0==thershold)||(1==thershold)){
					if(*pFill==0) *pFill=1;
					else return 1;
					pLcd[k--]=drawColor;
				}
				else{
					if(*pFill==0)
						k--;
					else
						pLcd[k--]=drawColor;
				}
			}

			if(j){
				i=buf[p++];
				if(*pFill==1){
					Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
					for(int a=0;a<buff_AA[0];++a)
						pLcd[k-a]=buff_AA[1+a];
			}}
			k+=BkpSizeX;

			if(*pFill==1){
				Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+1+a]=buff_AA[1+a];
			}
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				thershold=LCD_SearchRadiusPoints(posBuff,*pFill,BkpSizeX);
				if((0==thershold)||(1==thershold)){
					if(*pFill==0) *pFill=1;
					else return 1;
					pLcd[k]=drawColor;
					k+=BkpSizeX;
				}
				else{
					if(*pFill==0)
						k+=BkpSizeX;
					else{
						pLcd[k]=drawColor;
						k+=BkpSizeX;
					}
				}
			}

			if(j){
				i=buf[p++];
				if(*pFill==1){
					Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
					for(int a=0;a<buff_AA[0];++a)
						pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}}
			k--;

			if(*pFill==1){
				Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
			}
		}
	}
	return 0;
}

static uint8_t _DrawArrayBuffLeftUp_AA___(uint32_t posBuff, uint8_t *pFill, uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;
	uint8_t thershold;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				if(0==LCD_SearchRadiusPoints(0,0,BkpSizeX))  //posBuff !!!!!!
					return 1;
				else
					pLcd[k--]=drawColor;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k-=BkpSizeX;

			Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				if(0==LCD_SearchRadiusPoints(0,0,BkpSizeX))  //posBuff !!!!!!
					return 1;
				else
					pLcd[k]=drawColor;
				k-=BkpSizeX;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
	return 0;
}

static uint8_t _DrawArrayBuffRightUp_AA___(uint32_t posBuff, uint8_t *pFill, uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;
	uint8_t thershold;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				if(0==LCD_SearchRadiusPoints(0,0,BkpSizeX))  //posBuff !!!!!!
					return 1;
				else
					pLcd[k++]=drawColor;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k-=BkpSizeX;
			Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				if(0==LCD_SearchRadiusPoints(0,0,BkpSizeX))  //posBuff !!!!!!
					return 1;
				else
					pLcd[k]=drawColor;
				k-=BkpSizeX;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
	return 0;
}

//Rozgraniczyc LCD_DrawCircle() od LCD_DrawCircle z podzialem procentowym jako inne LCD_DrawCircle() !!!!
static void LCD_DrawCircle(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t __x, uint32_t __y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor)
{
	int matchWidth=0, circleFlag=0;
	uint16_t param =width>>16;
	uint32_t _FillColor, _width=(width&0x0000FFFF)-matchWidth;

	if(FillColor!=TRANSPARENT) _FillColor=FillColor;
	else								_FillColor=BkpColor;

	#define PARAM 		FrameColor, BkpColor, Circle.outRatioStart
	#define PARAM_AA 	FrameColor, BkpColor, _FillColor, Circle.outRatioStart, Circle.inRatioStart, BkpSizeX
	#define buf			Circle.lineBuff

	uint32_t bkX = BkpSizeX;
	uint32_t x=__x, y=__y;

	if((_width==height)&&(_width>0)&&(height>0))
	{
		GOTO_ToCalculateRadius:
	   _width=(width&0x0000FFFF)-matchWidth;

		uint32_t R=_width/2, err=0.5;
		uint32_t pxl_width = R/3;

		x=__x+_width/2 + matchWidth/2;
		if(_width%2) x++;
		y=__y;

		float _x=(float)x, _y=(float)y;
		float x0=(float)x, y0=(float)y+R;

		float param_y = pow(y0-_y,2);
		float param_x = pow(_x-x0,2);
		float decision = pow(R+err,2);

		int pxl_line=0,i=0;

		buf[0]=pxl_width;
		do{
			_x++;  pxl_line++;
			param_x = pow(_x-x0,2);
			if((param_x+param_y) > decision){
				_y++;
				param_y = pow(y0-_y,2);
				buf[1+i++]=pxl_line-1;
				pxl_line=1;
			}
		}while(i<pxl_width);

		LCD_CircleCorrect();

  	 uint16_t _height=buf[0];
  	 for(int i=0;i<buf[0];++i) _height+=buf[buf[0]-i];
  	 _height = 2*_height;

		if(height>=_height)
			y= y+(height-_height)/2;
		else{
			matchWidth+=1;
			goto GOTO_ToCalculateRadius;
		}

		x-=(x-__x-_height/2);
		y-=(y-__y);

		Circle.width=_height;
		Circle.x0=x;
		Circle.y0=y+Circle.width/2;
		circleFlag=1;
	}
	else if((_width==0)&&(height==0));
	else return;

	uint8_t buf_Inv[buf[0]+1];
	InverseAndCopyBuff(buf_Inv,buf);

	_StartDrawLine(posBuff,bkX,x,y);
	if(param==Degree_Circle && circleFlag)
	{
		uint8_t pFill=0;
		if(_DrawArrayBuffRightDown_AA___(posBuff,&pFill,PARAM_AA,0,buf)){ return; 	}   _CopyDrawPos(); _IncDrawPos(-bkX); PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		if(_DrawArrayBuffRightDown_AA___(posBuff,&pFill,PARAM_AA,1,buf_Inv)){ return; }                      				   PixelCorrect(PARAM, buf[1],bkX);   _IncDrawPos(-1);

		if(_DrawArrayBuffLeftDown_AA___ (posBuff,&pFill,PARAM_AA,1,buf)){ return; 	} 		 _CopyDrawPos(); _IncDrawPos(1);    PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		if(_DrawArrayBuffLeftDown_AA___ (posBuff,&pFill,PARAM_AA,0,buf_Inv)){ return; 	}                      					PixelCorrect(PARAM, buf[1],-1);    _IncDrawPos(-bkX);

//		_DrawArrayBuffLeftUp_AA___  (posBuff,&pFill,PARAM_AA,0,buf);       _CopyDrawPos(); _IncDrawPos(bkX);  PixelCorrect(PARAM, 1,     -bkX);  _SetCopyDrawPos();
//		_DrawArrayBuffLeftUp_AA___  (posBuff,&pFill,PARAM_AA,1,buf_Inv);  						    					PixelCorrect(PARAM, buf[1],-bkX);  _IncDrawPos(1);
//
//		_DrawArrayBuffRightUp_AA___ (posBuff,&pFill,PARAM_AA,1,buf);       _CopyDrawPos(); _IncDrawPos(-1);  	PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
//		_DrawArrayBuffRightUp_AA___ (posBuff,&pFill,PARAM_AA,0,buf_Inv);                       					PixelCorrect(PARAM, buf[1], 1);

		return;
//		if(Circle.rot[0]==3){ _DrawArrayBuffRightDown_AA___(posBuff,PARAM_AA,0,buf); return; }     else _DrawArrayBuffRightDown_AA(PARAM_AA,0,buf); 	  _CopyDrawPos(); _IncDrawPos(-bkX); PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
//		if(Circle.rot[0]==4){ _DrawArrayBuffRightDown_AA___(posBuff,PARAM_AA,1,buf_Inv); return; } else _DrawArrayBuffRightDown_AA(PARAM_AA,1,buf_Inv);                     				    PixelCorrect(PARAM, buf[1],bkX);   _IncDrawPos(-1);
//
//		if(Circle.rot[0]==5){ _DrawArrayBuffLeftDown_AA___ (PARAM_AA,1,buf); return; }     else _DrawArrayBuffLeftDown_AA(PARAM_AA,1,buf); 		 _CopyDrawPos(); _IncDrawPos(1);    PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
//		if(Circle.rot[0]==6){ _DrawArrayBuffLeftDown_AA___ (PARAM_AA,0,buf_Inv); return; } else _DrawArrayBuffLeftDown_AA(PARAM_AA,0,buf_Inv);                     					   PixelCorrect(PARAM, buf[1],-1);    _IncDrawPos(-bkX);
//
//		if(Circle.rot[0]==7){ _DrawArrayBuffLeftUp_AA___   (PARAM_AA,0,buf); return; }     else _DrawArrayBuffLeftUp_AA(PARAM_AA,0,buf);       _CopyDrawPos(); _IncDrawPos(bkX);  PixelCorrect(PARAM, 1,     -bkX);  _SetCopyDrawPos();
//		if(Circle.rot[0]==8){ _DrawArrayBuffLeftUp_AA___   (PARAM_AA,1,buf_Inv); return; } else _DrawArrayBuffLeftUp_AA(PARAM_AA,1,buf_Inv); 						    					  PixelCorrect(PARAM, buf[1],-bkX);  _IncDrawPos(1);
//
//		if(Circle.rot[0]==1){ _DrawArrayBuffRightUp_AA___  (PARAM_AA,1,buf); return; }     else _DrawArrayBuffRightUp_AA(PARAM_AA,1,buf);       _CopyDrawPos(); _IncDrawPos(-1);  	PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
//		if(Circle.rot[0]==2){ _DrawArrayBuffRightUp_AA___  (PARAM_AA,0,buf_Inv); return; } else _DrawArrayBuffRightUp_AA(PARAM_AA,0,buf_Inv);                       					PixelCorrect(PARAM, buf[1], 1);

	}
	else
	{
		_DrawArrayBuffRightDown_AA(PARAM_AA,0,buf); 	    _CopyDrawPos(); _IncDrawPos(-bkX); PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffRightDown_AA(PARAM_AA,1,buf_Inv);                      				   PixelCorrect(PARAM, buf[1],bkX);   _IncDrawPos(-1);

		_DrawArrayBuffLeftDown_AA (PARAM_AA,1,buf); 		 _CopyDrawPos(); _IncDrawPos(1);    PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffLeftDown_AA (PARAM_AA,0,buf_Inv);                      					PixelCorrect(PARAM, buf[1],-1);    _IncDrawPos(-bkX);

		_DrawArrayBuffLeftUp_AA  (PARAM_AA,0,buf);       _CopyDrawPos(); _IncDrawPos(bkX);  PixelCorrect(PARAM, 1,     -bkX);  _SetCopyDrawPos();
		_DrawArrayBuffLeftUp_AA  (PARAM_AA,1,buf_Inv);  						    					PixelCorrect(PARAM, buf[1],-bkX);  _IncDrawPos(1);

		_DrawArrayBuffRightUp_AA (PARAM_AA,1,buf);       _CopyDrawPos(); _IncDrawPos(-1);  	PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffRightUp_AA (PARAM_AA,0,buf_Inv);                       					PixelCorrect(PARAM, buf[1], 1);
	}

	if(FillColor!=TRANSPARENT)
	{
   	 uint16_t fillWidth, fillHeight=0;

		_StartDrawLine(posBuff,bkX,x,y);
		fillWidth=0;
		for(int i=0;i<buf[0];++i){
			_IncDrawPos(bkX-buf[i+1]);	_CopyDrawPos(); k+=buf[i+1];	if(i>0) _DrawRight(2*fillWidth,FillColor);	_SetCopyDrawPos();	fillHeight++;
			fillWidth += buf[i+1];
		}

		for(int i=0;i<buf[0];++i){
			fillWidth++;
			_IncDrawPos(-1);
			for(int j=0; j<buf[buf[0]-i]; ++j){
				_IncDrawPos(bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){k+=1;_DrawRight(2*(fillWidth-1),FillColor);}else{k+=2;_DrawRight(2*(fillWidth-2),FillColor);}	_SetCopyDrawPos();	fillHeight++;
			}
		}

		fillHeight = 2*fillHeight-1;

		_StartDrawLine(posBuff,bkX,x,y+fillHeight);
		fillWidth=0;
		for(int i=0;i<buf[0];++i){
			_IncDrawPos(-bkX-buf[i+1]);	_CopyDrawPos();	 k+=buf[i+1];	if(i>0) _DrawRight(2*fillWidth,FillColor);	_SetCopyDrawPos();
			fillWidth += buf[i+1];
		}

		for(int i=0;i<buf[0];++i){
			fillWidth++;
			_IncDrawPos(-1);
			for(int j=0; j<buf[buf[0]-i]; ++j){
				_IncDrawPos(-bkX);	_CopyDrawPos();  if(j==buf[buf[0]-i]-1){k+=1;_DrawRight(2*(fillWidth-1),FillColor);}else{k+=2;_DrawRight(2*(fillWidth-2),FillColor);}	 _SetCopyDrawPos();
			}
		}
	}

	if(param==Percent_Circle && circleFlag)
	{
		int circleLinesLenCorrect=0;
		uint16_t fillWidth=0, fillHeight=0;

		for(int i=0;i<Circle.degree[0];++i)
		{
			if(IS_RANGE(Circle.degree[1+i],46,89)){
				if(Circle.width<150) circleLinesLenCorrect=-1;
			}
			else if(IS_RANGE(Circle.degree[1+i],113,157)) circleLinesLenCorrect=2;  //to w fazie koncowej trzeba uregulowac ostatecznie
			else if(IS_RANGE(Circle.degree[1+i],201,247)) circleLinesLenCorrect=3;
			else if(IS_RANGE(Circle.degree[1+i],270,292)) circleLinesLenCorrect=-1;
			else if(IS_RANGE(Circle.degree[1+i],293,338)){
				  	  if((Circle.width>100)&&(Circle.width<260)) circleLinesLenCorrect=1;
				  	  else if(Circle.width>260) 						circleLinesLenCorrect=2;
				  	  else													circleLinesLenCorrect=-1;
			}

			if(i==0)
				DrawLine(0,Circle.x0,Circle.y0,(Circle.width-4-circleLinesLenCorrect)/2-1,Circle.degree[1+i],FrameColor,BkpSizeX, Circle.outRatioStart,Circle.inRatioStart,_FillColor,Circle.degColor[i+1]);
			else if(i==Circle.degree[0]-1)
				DrawLine(0,Circle.x0,Circle.y0,(Circle.width-4-circleLinesLenCorrect)/2-1,Circle.degree[1+i],FrameColor,BkpSizeX, Circle.outRatioStart,Circle.inRatioStart,Circle.degColor[i],_FillColor);
			else
				DrawLine(0,Circle.x0,Circle.y0,(Circle.width-4-circleLinesLenCorrect)/2-1,Circle.degree[1+i],FrameColor,BkpSizeX, Circle.outRatioStart,Circle.inRatioStart,Circle.degColor[i],Circle.degColor[i+1]);
		}

		void _Fill(int width)
		{
			int j=width;
			uint8_t threshold[Circle.degree[0]];
			int subj=0, deltaDeg;

			while(j--)
			{
				if(pLcd[k]==FillColor){
					for(int i=0;i<Circle.degree[0];++i)
						threshold[i]=LCD_SearchRadiusPoints(posBuff,i,bkX);
				}

				GOTO_ToFillCircle:
				if(pLcd[k]==FillColor)
				{
					for(int i=0;i<Circle.degree[0]-1;++i)
					{
						deltaDeg=Circle.degree[1+i+1]-Circle.degree[1+i];
						if( ((deltaDeg>0)&&(deltaDeg<=180)) || (deltaDeg<-180) ){
							if((threshold[i]==0)&&(threshold[i+1]==2)){
								pLcd[k]=Circle.degColor[1+i];
								break;
							}
						}
						else{
							if((threshold[i]==0)||(threshold[i+1]==2)){
								pLcd[k]=Circle.degColor[1+i];
								break;
							}
						}
					}
					k++; subj=1;
				}
				if(pLcd[k]==FillColor){ if(j==0){ k--; break; } j--; subj=0; goto GOTO_ToFillCircle; }
				k++;
				if(subj==1){ subj=0; if(j>0) j--; else{ k--; break; }}
			}
		}

		void _Change_AA(int width)
		{
			int j=width;
			int threshold[Circle.degree[0]];
			int deltaDeg;

			while(j--)
			{
				for(int i=0;i<Circle.degree[0];++i)
					threshold[i]=LCD_SearchRadiusPoints(posBuff,i,bkX);

				for(int i=0;i<Circle.degree[0]-1;++i)
				{
					deltaDeg=Circle.degree[1+i+1]-Circle.degree[1+i];
					if( ((deltaDeg>0)&&(deltaDeg<=180)) || (deltaDeg<-180) )
					{
						if((threshold[i]==0)&&(threshold[i+1]==2)){
							pLcd[k]=GetTransitionColor(FrameColor, Circle.degColor[1+i], GetTransitionCoeff(FrameColor,FillColor,pLcd[k]));
							break;
						}
					}
					else
					{
						if((threshold[i]==0)||(threshold[i+1]==2)){
							pLcd[k]=GetTransitionColor(FrameColor, Circle.degColor[1+i], GetTransitionCoeff(FrameColor,FillColor,pLcd[k]));
							break;
						}
					}
				}
				k++;
			}
		}

		_StartDrawLine(posBuff,bkX,x,y);
		for(int i=0;i<buf[0];++i){
			_IncDrawPos(bkX-buf[i+1]);	  _CopyDrawPos();   _Change_AA(buf[i+1]); 	if(i>0) _Fill(2*fillWidth); _Change_AA(buf[i+1]); _SetCopyDrawPos();	fillHeight++;
			fillWidth += buf[i+1];
		}

		for(int i=0;i<buf[0];++i){
			fillWidth++;
			_IncDrawPos(-1);
			for(int j=0; j<buf[buf[0]-i]; ++j){
				_IncDrawPos(bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){ _Change_AA(1); _Fill(2*(fillWidth-1)); _Change_AA(1); }else{ _Change_AA(2); _Fill(2*(fillWidth-2)); _Change_AA(2); }	_SetCopyDrawPos();	fillHeight++;
			}
		}

		fillHeight = 2*fillHeight-1;

		_StartDrawLine(posBuff,bkX,x,y+fillHeight);
		fillWidth=0;
		for(int i=0;i<buf[0];++i){
			_IncDrawPos(-bkX-buf[i+1]);	_CopyDrawPos();	_Change_AA(buf[i+1]); if(i>0) _Fill(2*fillWidth); _Change_AA(buf[i+1]);	_SetCopyDrawPos();
			fillWidth += buf[i+1];
		}

		for(int i=0;i<buf[0];++i){
			fillWidth++;
			_IncDrawPos(-1);
			for(int j=0; j<buf[buf[0]-i]; ++j){
				_IncDrawPos(-bkX);	_CopyDrawPos();  if(j==buf[buf[0]-i]-1){ _Change_AA(1); _Fill(2*(fillWidth-1)); _Change_AA(1);}else{ _Change_AA(2); _Fill(2*(fillWidth-2)); _Change_AA(2);}	 _SetCopyDrawPos();
			}
		}
	}

	#undef PARAM
	#undef PARAM_AA
	#undef buf
}

// LCD_QuarterCircle
static void LCD_DrawHalfCircle(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t __x, uint32_t __y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor)
{
	int matchWidth=0;
	uint32_t _FillColor, _width=(width&0x0000FFFF)-matchWidth;

	if(FillColor!=TRANSPARENT) _FillColor=FillColor;
	else								_FillColor=BkpColor;

	#define PARAM 		FrameColor, BkpColor, Circle.outRatioStart
	#define PARAM_AA 	FrameColor, BkpColor, _FillColor, Circle.outRatioStart, Circle.inRatioStart, BkpSizeX
	#define buf			Circle.lineBuff

	uint32_t bkX = BkpSizeX;
	uint32_t x=__x, y=__y;

	if((_width==height)&&(_width>0)&&(height>0))
	{
		GOTO_ToCalculateRadius:
	   _width=(width&0x0000FFFF)-matchWidth;

		uint32_t R=_width/2, err=0.5;
		uint32_t pxl_width = R/3;

		x=__x+_width/2 + matchWidth/2;
		if(_width%2) x++;
		y=__y;

		float _x=(float)x, _y=(float)y;
		float x0=(float)x, y0=(float)y+R;

		float param_y = pow(y0-_y,2);
		float param_x = pow(_x-x0,2);
		float decision = pow(R+err,2);

		int pxl_line=0,i=0;

		buf[0]=pxl_width;
		do{
			_x++;  pxl_line++;
			param_x = pow(_x-x0,2);
			if((param_x+param_y) > decision){
				_y++;
				param_y = pow(y0-_y,2);
				buf[1+i++]=pxl_line-1;
				pxl_line=1;
			}
		}while(i<pxl_width);

		LCD_CircleCorrect();

  	 uint16_t _height=buf[0];
  	 for(int i=0;i<buf[0];++i) _height+=buf[buf[0]-i];
  	 _height = 2*_height;

		if(height>=_height)
			y= y+(height-_height)/2;
		else{
			matchWidth+=1;
			goto GOTO_ToCalculateRadius;
		}

		x-=(x-__x-_height/2);
		y-=(y-__y);

		Circle.width=_height;

	}
	else if((_width==0)&&(height==0));
	else return;

	uint8_t buf_Inv[buf[0]+1];
	InverseAndCopyBuff(buf_Inv,buf);

	_StartDrawLine(0,bkX,x,y);

	switch(width>>16)
	{
	case Half_Circle_0:
		_DrawArrayBuffRightDown_AA(PARAM_AA,0,buf); 		 _CopyDrawPos(); _IncDrawPos(-bkX); PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffRightDown_AA(PARAM_AA,1,buf_Inv);                      				   PixelCorrect(PARAM, buf[1],bkX);   _IncDrawPos(-1);

		_OffsetLeftDown(BkpSizeX,1,buf);
		_OffsetLeftDown(BkpSizeX,0,buf_Inv);  _IncDrawPos(-bkX);

		_OffsetLeftUp(BkpSizeX,0,buf);
		_OffsetLeftUp(BkpSizeX,1,buf_Inv);	_IncDrawPos(1);

		_DrawArrayBuffRightUp_AA (PARAM_AA,1,buf);       _CopyDrawPos(); _IncDrawPos(-1);  	PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffRightUp_AA (PARAM_AA,0,buf_Inv);                       					PixelCorrect(PARAM, buf[1], 1);
		break;

	case Half_Circle_90:
		_DrawArrayBuffRightDown_AA(PARAM_AA,0,buf); 		 _CopyDrawPos(); _IncDrawPos(-bkX); PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffRightDown_AA(PARAM_AA,1,buf_Inv);                      				   PixelCorrect(PARAM, buf[1],bkX);   _IncDrawPos(-1);

		_DrawArrayBuffLeftDown_AA (PARAM_AA,1,buf); 		 _CopyDrawPos(); _IncDrawPos(1);    PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffLeftDown_AA (PARAM_AA,0,buf_Inv);                      					PixelCorrect(PARAM, buf[1],-1);    _IncDrawPos(-bkX);
		break;

	case Half_Circle_180:
		_OffsetRightDown(BkpSizeX,0,buf);
		_OffsetRightDown(BkpSizeX,1,buf_Inv);    _IncDrawPos(-1);

		_DrawArrayBuffLeftDown_AA (PARAM_AA,1,buf); 		 _CopyDrawPos(); _IncDrawPos(1);    PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffLeftDown_AA (PARAM_AA,0,buf_Inv);                      					PixelCorrect(PARAM, buf[1],-1);    _IncDrawPos(-bkX);

		_DrawArrayBuffLeftUp_AA  (PARAM_AA,0,buf);       _CopyDrawPos(); _IncDrawPos(bkX);  PixelCorrect(PARAM, 1,     -bkX);  _SetCopyDrawPos();
		_DrawArrayBuffLeftUp_AA  (PARAM_AA,1,buf_Inv);  						    					PixelCorrect(PARAM, buf[1],-bkX);  _IncDrawPos(1);
		break;

	case Half_Circle_270:
		_OffsetRightDown(BkpSizeX,0,buf);
		_OffsetRightDown(BkpSizeX,1,buf_Inv);    _IncDrawPos(-1);

		_OffsetLeftDown(BkpSizeX,1,buf);
		_OffsetLeftDown(BkpSizeX,0,buf_Inv);    _IncDrawPos(-bkX);

		_DrawArrayBuffLeftUp_AA  (PARAM_AA,0,buf);       _CopyDrawPos(); _IncDrawPos(bkX);  PixelCorrect(PARAM, 1,     -bkX);  _SetCopyDrawPos();
		_DrawArrayBuffLeftUp_AA  (PARAM_AA,1,buf_Inv);  						    					PixelCorrect(PARAM, buf[1],-bkX);  _IncDrawPos(1);

		_DrawArrayBuffRightUp_AA (PARAM_AA,1,buf);       _CopyDrawPos(); _IncDrawPos(-1);  	PixelCorrect(PARAM, 1,      1);    _SetCopyDrawPos();
		_DrawArrayBuffRightUp_AA (PARAM_AA,0,buf_Inv);                       					PixelCorrect(PARAM, buf[1], 1);
		break;
	}

	if(FillColor!=TRANSPARENT)
	{
   	 uint16_t fillWidth, fillHeight=0;

   		switch(width>>16)
   		{
   		case Half_Circle_0:
   			_StartDrawLine(0,bkX,x,y);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(bkX-buf[i+1]);	_CopyDrawPos(); k+=buf[i+1]; 	if(i>0) _DrawRight(2*fillWidth,FillColor);	_SetCopyDrawPos(); fillHeight++;
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){if(i<buf[0]-2){k+=1;_DrawRight(2*(fillWidth-1),FillColor);}}else{k+=2;_DrawRight(2*(fillWidth-2),FillColor);}	_SetCopyDrawPos();	fillHeight++;
   				}
   			}
   			break;

   		case Half_Circle_90:
   			_StartDrawLine(0,bkX,x,y);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(bkX-buf[i+1]);	_CopyDrawPos(); k+=buf[i+1]+fillWidth; 	if(i>0) _DrawRight(1*fillWidth,FillColor);	_SetCopyDrawPos(); fillHeight++;
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){k+=1+fillWidth-1;_DrawRight(1*(fillWidth-1),FillColor);}else{k+=fillWidth;_DrawRight(1*(fillWidth-2),FillColor);}	_SetCopyDrawPos();	fillHeight++;
   				}
   			}

   			fillHeight = 2*fillHeight-1;

   			_StartDrawLine(0,bkX,x,y+fillHeight);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(-bkX-buf[i+1]);_CopyDrawPos(); k+=buf[i+1]+fillWidth; 	if(i>0) _DrawRight(1*fillWidth,FillColor);  _SetCopyDrawPos();
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(-bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){k+=1+fillWidth-1;_DrawRight(1*(fillWidth-1),FillColor);}else{k+=fillWidth;_DrawRight(1*(fillWidth-2),FillColor);}	_SetCopyDrawPos();	fillHeight++;
   				}
   			}
   			break;

   			case Half_Circle_180:
   			_StartDrawLine(0,bkX,x,y);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(bkX-buf[i+1]);	 fillHeight++;
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(bkX);	 fillHeight++;
   				}
   			}

   			fillHeight = 2*fillHeight-1;

   			_StartDrawLine(0,bkX,x,y+fillHeight);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(-bkX-buf[i+1]);	_CopyDrawPos(); k+=buf[i+1]; 	if(i>0) _DrawRight(2*fillWidth,FillColor);	_SetCopyDrawPos();
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(-bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){if(i<buf[0]-2){k+=1;_DrawRight(2*(fillWidth-1),FillColor);}}else{k+=2;_DrawRight(2*(fillWidth-2),FillColor);}	_SetCopyDrawPos();
   				}
   			}
   			break;

   		case Half_Circle_270:
   			_StartDrawLine(0,bkX,x,y);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(bkX-buf[i+1]);	_CopyDrawPos(); k+=buf[i+1]; 	if(i>0) _DrawRight(1*fillWidth,FillColor);	_SetCopyDrawPos(); fillHeight++;
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){k+=1;_DrawRight(1*(fillWidth-1),FillColor);}else{k+=2;_DrawRight(1*(fillWidth-2),FillColor);}	_SetCopyDrawPos();	fillHeight++;
   				}
   			}

   			fillHeight = 2*fillHeight-1;

   			_StartDrawLine(0,bkX,x,y+fillHeight);
   			fillWidth=0;
   			for(int i=0;i<buf[0];++i){
   				_IncDrawPos(-bkX-buf[i+1]);_CopyDrawPos(); k+=buf[i+1]; 	if(i>0) _DrawRight(1*fillWidth,FillColor);  _SetCopyDrawPos();
   				fillWidth += buf[i+1];
   			}

   			for(int i=0;i<buf[0];++i){
   				fillWidth++;
   				_IncDrawPos(-1);
   				for(int j=0; j<buf[buf[0]-i]; ++j){
   					_IncDrawPos(-bkX);	_CopyDrawPos(); if(j==buf[buf[0]-i]-1){k+=1;_DrawRight(1*(fillWidth-1),FillColor);}else{k+=2;_DrawRight(1*(fillWidth-2),FillColor);}	_SetCopyDrawPos();	fillHeight++;
   				}
   			}
   			break;
   		}
	}

	#undef PARAM
	#undef PARAM_AA
	#undef buf
}


//################################## -- Global Declarations -- #########################################################
//################################## -- Global Declarations -- #########################################################
//################################## -- Global Declarations -- #########################################################

void CorrectLineAA_on(void){
	correctLine_AA=1;
}
void CorrectLineAA_off(void){
	correctLine_AA=0;
}

void Set_AACoeff_RoundFrameRectangle(float coeff_1, float coeff_2){
	AA.c1 = coeff_1;
	AA.c2 = coeff_2;
}

void _FillBuff(int itCount, uint32_t color)
{
	if(itCount>10)
	{
		int j=itCount/2;
		int a=j;

		uint64_t *pLcd64=(uint64_t*) (pLcd+k);
		uint64_t color64=(((uint64_t)color)<<32)|((uint64_t)color);

		j--;
		while (j)
			pLcd64[j--]=color64;

		pLcd64[j]=color64;
		k+=a+itCount/2;

		if (itCount%2)
			pLcd[k++]=color;
	}
	else
	{
		for(int i=0;i<itCount;++i)
			pLcd[k++]=color;
	}
}

void _CopyDrawPos(void){
	kCopy=k;
}
void _SetCopyDrawPos(void){
	k=kCopy;
}
void _IncDrawPos(int pos){
	k+=pos;
}

void _StartDrawLine(uint32_t posBuff,uint32_t BkpSizeX,uint32_t x,uint32_t y){
	k=posBuff+(y*BkpSizeX+x);
}
uint32_t _GetPosK(uint32_t posBuff,uint32_t BkpSizeX,uint32_t x,uint32_t y){
	return (posBuff+(y*BkpSizeX+x));
}
uint32_t _GetPosY(uint32_t posBuff,uint32_t BkpSizeX){
	return ((k-posBuff)/BkpSizeX);
}
uint32_t _GetPosX(uint32_t posBuff,uint32_t BkpSizeX){
	return ((k-posBuff)-_GetPosY(posBuff,BkpSizeX)*BkpSizeX);
}
structPosition _GetPosXY(uint32_t posBuff, uint32_t BkpSizeX){
	structPosition temp;
	int kOffs = k-posBuff;
	temp.y = (kOffs/BkpSizeX);
	temp.x = (kOffs-temp.y*BkpSizeX);
	return temp;
}
structPosition _GetPosXY___(uint32_t posBuff, int k, uint32_t BkpSizeX){
	structPosition temp;
	int kOffs = k-posBuff;
	temp.y = (kOffs/BkpSizeX);
	temp.x = (kOffs-temp.y*BkpSizeX);
	return temp;
}
void _NextDrawLine(uint32_t BkpSizeX,uint32_t width){
	k+=(BkpSizeX-width);
}

void _DrawRight(int width, uint32_t color)
{
	int j=width;
	uint32_t *p = pLcd+k;
	while(j--)
		*(p++)=color;
	k+=width;
}

void _DrawLeft(int width, uint32_t color)
{
	int j=width;
	uint32_t *p = pLcd+k;
	while(j--)
		*(p--)=color;
	k-=width;
}

void _DrawDown(int height, uint32_t color,uint32_t BkpSizeX)
{
	int j=height;
	while(j--)
	{
		pLcd[k]=color;
		k+=BkpSizeX;
	}
}

void _DrawUp(int height, uint32_t color,uint32_t BkpSizeX)
{
	int j=height;
	while(j--)
	{
		pLcd[k]=color;
		k-=BkpSizeX;
	}
}

void Set_AACoeff_Draw(int pixelsInOneSide, uint32_t colorFrom, uint32_t colorTo, float ratioStart)
{
	//int pixelsInOneSide= pixelsInOneSide_==1?3:pixelsInOneSide_;
	//int pixelsInOneSide;
//
//	if(pixelsInOneSide_==1) pixelsInOneSide=2;
//	else                    pixelsInOneSide=pixelsInOneSide_;

	float incr= (1-ratioStart)/pixelsInOneSide;
	buff_AA[0]=pixelsInOneSide;
	for(int i=0;i<pixelsInOneSide;++i){
		if(1+i>MAX_SIZE_TAB_AA)
			break;
		buff_AA[1+i]= GetTransitionColor(colorFrom,colorTo, ratioStart+i*incr);
	}
}

void _DrawRightDown(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k++]=color;
			k+=BkpSizeX;
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--)
			{
				pLcd[k]=color;
				k+=BkpSizeX;
			}
			k++;
		}
	}
}

void _DrawRightDown_AA(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k++]=color;

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k+=BkpSizeX;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--){
				pLcd[k]=color;
				k+=BkpSizeX;
			}

			if(j){
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawArrayRightDown(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...)
{
	int j=len,i;
	va_list va;
	va_start(va,0);

	if(0==direction)
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--)
				pLcd[k++]=color;
			k+=BkpSizeX;
		}
	}
	else
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--){
				pLcd[k]=color;
				k+=BkpSizeX;
			}
			k++;
		}
	}
	va_end(va);
}

void _DrawArrayRightDown_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...)
{
	va_list va;
	va_start(va,0);
	int j=len, i=va_arg(va,int), i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k++]=drawColor;

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<i;++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k+=BkpSizeX;
			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k+=BkpSizeX;
			}

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
	va_end(va);
}

void _DrawArrayBuffRightDown_AA_new(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], i_prev, p=2;  int m=0;  int s=0;   int kk=0;  int yy=0;  int licz2=0;

	if(0==direction)
	{
		while(j--)
		{
			if(i==1)
			{
				if(s==0)
				{
					if(yy==0)
					{
						for(int x=m+1;x<buf[0];++x){
							if(buf[1+x]==1)
								s++;
							else
								break;
						}
						kk=1;
						s++;
						Set_AACoeff_Draw(s+1,drawColor,outColor,outRatioStart);
					}
				}
			}
			else
			{
				Set_AACoeff_Draw(i+2,drawColor,outColor,outRatioStart); s=0;  kk=0; yy=0;
			}



			while(i--)
			{

				if(kk)
					pLcd[k-1]=buff_AA[1+buff_AA[0]-s];
				else
				{
					if(i_prev>2)
					{
						k-=BkpSizeX;
						pLcd[k]=buff_AA[buff_AA[0]-i];
						k+=BkpSizeX;
						licz2=0;
					}
					else if(i_prev==2)
					{
						if(licz2==0){
							licz2=1;
							pLcd[k-1]=WHITE;//buff_AA[1+buff_AA[0]];
						}
					}
					else
						licz2=0;
				}

				pLcd[k]=drawColor;  //kkkkkkkkkkkkkkkkkkkkkkkkkk

				if(kk)
					pLcd[k+1]=buff_AA[1+s];
				else
				{
					if(i_prev>2)
					{
						k+=BkpSizeX;
						pLcd[k]=buff_AA[1+i+2];
						k-=BkpSizeX;
						licz2=0;
					}
					else if(i_prev==2)
					{
						if(licz2==1){
							licz2=3;
							pLcd[k+1]=WHITE;//buff_AA[1+buff_AA[0]];
						}
					}
					else
						licz2=0;
				}
				k++;

				if(s)
					s--;

				if(s==0){
					if(i_prev==1)
						yy=1;
				}

			}
			if(j)
				i=buf[p++];
			i_prev=i;
			k+=BkpSizeX;
			m++;
		}
	}
	else
	{
		while(j--)
		{
			if(i==1) Set_AACoeff_Draw(4,drawColor,outColor,outRatioStart);
			else 		Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);

			while(i--)
			{
				pLcd[k-1]=buff_AA[buff_AA[0]-i];
				pLcd[k]=drawColor;
				pLcd[k+1]=buff_AA[1+i];
				k+=BkpSizeX;
			}
			if(j) i=buf[p++];
			k++;
		}
	}
}

void _DrawArrayBuffRightDown_AA(uint32_t _drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;
	uint32_t drawColor=_drawColor;

	void XXX(void)
	{
		uint16_t aaaa=GetDegFromPosK(0, Circle.x0, Circle.y0, BkpSizeX);
		if(aaaa>145 && aaaa<170)
			drawColor=MYRED;
		else
			drawColor=_drawColor;
	}



	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k++]=drawColor;  XXX();
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k+=BkpSizeX;   XXX();

			Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k+=BkpSizeX;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawRightUp(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k++]=color;
			k-=BkpSizeX;
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--)
			{
				pLcd[k]=color;
				k-=BkpSizeX;
			}
			k++;
		}
	}
}

void _DrawRightUp_AA(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k++]=color;

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k-=BkpSizeX;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--){
				pLcd[k]=color;
				k-=BkpSizeX;
			}

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawArrayRightUp(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...)
{
	int j=len,i;
	va_list va;
	va_start(va,0);

	if(0==direction)
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--)
				pLcd[k++]=color;
			k-=BkpSizeX;
		}
	}
	else
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--){
				pLcd[k]=color;
				k-=BkpSizeX;
			}
			k++;
		}
	}
	va_end(va);
}

void _DrawArrayRightUp_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...)
{
	va_list va;
	va_start(va,0);
	int j=len, i=va_arg(va,int), i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k++]=drawColor;

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<i;++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k-=BkpSizeX;
			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k-=BkpSizeX;
			}

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
	va_end(va);
}

void _DrawArrayBuffRightUp_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k++]=drawColor;

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a]=buff_AA[1+a];
			}
			k-=BkpSizeX;
			Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-1-a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k-=BkpSizeX;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k++;

			Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawLeftDown(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k--]=color;
			k+=BkpSizeX;
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--)
			{
				pLcd[k]=color;
				k+=BkpSizeX;
			}
			k--;
		}
	}
}

void _DrawLeftDown_AA(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k--]=color;

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k+=BkpSizeX;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--){
				pLcd[k]=color;
				k+=BkpSizeX;
			}

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawArrayLeftDown(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...)
{
	int j=len,i;
	va_list va;
	va_start(va,0);

	if(0==direction)
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--)
				pLcd[k--]=color;
			k+=BkpSizeX;
		}
	}
	else
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--){
				pLcd[k]=color;
				k+=BkpSizeX;
			}
			k--;
		}
	}
	va_end(va);
}

void _DrawArrayLeftDown_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...)
{
	va_list va;
	va_start(va,0);
	int j=len, i=va_arg(va,int), i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k--]=drawColor;

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<i;++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k+=BkpSizeX;

			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k+=BkpSizeX;
			}

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
	va_end(va);
}

void _DrawArrayBuffLeftDown_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k--]=drawColor;

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k+=BkpSizeX;

			Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k+=BkpSizeX;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k+a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k-(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawLeftUp(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k--]=color;
			k-=BkpSizeX;
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--)
			{
				pLcd[k]=color;
				k-=BkpSizeX;
			}
			k--;
		}
	}
}

void _DrawLeftUp_AA(int width,int height, uint32_t color,uint32_t BkpSizeX)
{
	int j,i,x;

	if(width >= height)
	{
		x=width/height;
		j=height;
		while(j--)
		{
			i=x;
			while(i--)
				pLcd[k--]=color;

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k-=BkpSizeX;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		x=height/width;
		j=width;
		while(j--)
		{
			i=x;
			while(i--){
				pLcd[k]=color;
				k-=BkpSizeX;
			}

			if(j){
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void _DrawArrayLeftUp(uint32_t color,uint32_t BkpSizeX, int direction, int len, ...)
{
	int j=len,i;
	va_list va;
	va_start(va,0);

	if(0==direction)
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--)
				pLcd[k--]=color;
			k-=BkpSizeX;
		}
	}
	else
	{
		while(j--)
		{
			i=va_arg(va,int);
			while(i--){
				pLcd[k]=color;
				k-=BkpSizeX;
			}
			k--;
		}
	}
	va_end(va);
}

void _DrawArrayLeftUp_AA(uint32_t drawColor, uint32_t bkColor, float ratioStart, uint32_t BkpSizeX, int direction, int len, ...)
{
	va_list va;
	va_start(va,0);
	int j=len, i=va_arg(va,int), i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k--]=drawColor;

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<i;++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k-=BkpSizeX;

			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k-=BkpSizeX;
			}

			i=va_arg(va,int);
			if(j){
				Set_AACoeff_Draw(i,drawColor,bkColor,ratioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			Set_AACoeff_Draw(i_prev,drawColor,bkColor,ratioStart);
			for(int a=0;a<i_prev;++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
	va_end(va);
}

void _DrawArrayBuffLeftUp_AA(uint32_t drawColor, uint32_t outColor, uint32_t inColor, float outRatioStart, float inRatioStart, uint32_t BkpSizeX, int direction, uint8_t *buf)
{
	int j=buf[0], i=buf[1], p=2, i_prev;

	if(0==direction)
	{
		while(j--)
		{
			i_prev=i;
			while(i--)
				pLcd[k--]=drawColor;

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,outColor,outRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a]=buff_AA[1+a];
			}
			k-=BkpSizeX;

			Set_AACoeff_Draw(i_prev,drawColor,inColor,inRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+1+a]=buff_AA[1+a];
		}
	}
	else
	{
		while(j--)
		{
			i_prev=i;
			while(i--){
				pLcd[k]=drawColor;
				k-=BkpSizeX;
			}

			if(j){
				i=buf[p++];
				Set_AACoeff_Draw(i,drawColor,inColor,inRatioStart);
				for(int a=0;a<buff_AA[0];++a)
					pLcd[k-a*BkpSizeX]=buff_AA[1+a];
			}
			k--;

			Set_AACoeff_Draw(i_prev,drawColor,outColor,outRatioStart);
			for(int a=0;a<buff_AA[0];++a)
				pLcd[k+(a+1)*BkpSizeX]=buff_AA[1+a];
		}
	}
}

void LCD_Display(uint32_t posBuff, uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height){
	LCD_DisplayBuff(Xpos,Ypos,width,height,  pLcd+posBuff);
}
void LCD_Show(void){
	LCD_Display(0,0,0,LCD_X,LCD_Y);
}

void LCD_Shape(uint32_t x,uint32_t y,figureShape pShape,uint32_t width,uint32_t height,uint32_t FrameColor,uint32_t FillColor,uint32_t BkpColor){
	LCD_ShapeWindow(pShape,0,LCD_X,LCD_Y,x,y,width,height,FrameColor,FillColor,BkpColor);
}
void LCD_ShapeWindow(figureShape pShape,uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	pShape(posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FillColor,BkpColor);
}
void LCD_ShapeIndirect(uint32_t xPos,uint32_t yPos,figureShape pShape, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	pShape(0,width,height,0,0,width,height,FrameColor,FillColor,BkpColor);
	LCD_Display(0,xPos,yPos,width,height);
}
void LCD_ShapeWindowIndirect(uint32_t xPos,uint32_t yPos,figureShape pShape,uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	pShape(posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FillColor,BkpColor);
	LCD_Display(posBuff,xPos,yPos,BkpSizeX,BkpSizeY);
}

void LCD_Clear(uint32_t color){
	LCD_ShapeWindow(LCD_Rectangle,0,LCD_X,LCD_Y, 0,0, LCD_X, LCD_Y, color,color,color);
}
void LCD_ClearIndirect(uint32_t posBuff, uint32_t BkpSizeX, uint32_t BkpSizeY, uint32_t color){
	LCD_ShapeWindow(LCD_Rectangle,posBuff,BkpSizeX,BkpSizeY, 0,0, BkpSizeX, BkpSizeY, color,color,color);
}
uint32_t SetColorBoldFrame(uint32_t frameColor, uint8_t thickness){
	return (frameColor&0xFFFFFF)|thickness<<24;
}

void LCD_Rectangle(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	_StartDrawLine(posBuff,BkpSizeX,x,y);
	_FillBuff(width, FrameColor);
	if(height>1)
	{
		_NextDrawLine(BkpSizeX,width);
		for (int j=0; j<height-2; j++)
		{
			_FillBuff(1, FrameColor);
			_FillBuff(width-2, FillColor);
			_FillBuff(1, FrameColor);
			_NextDrawLine(BkpSizeX,width);
		}
		_FillBuff(width, FrameColor);
	}
}
void LCD_Frame(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	_StartDrawLine(posBuff,BkpSizeX,x,y);
	_FillBuff(width, FrameColor);
	if(height>1)
	{
		_NextDrawLine(BkpSizeX,width);
		for (int j=0; j<height-2; j++)
		{
			_FillBuff(1, FrameColor);
			k+=width-2;
			_FillBuff(1, FrameColor);
			_NextDrawLine(BkpSizeX,width);
		}
		_FillBuff(width, FrameColor);
	}
}

void LCD_BoldRectangle(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	uint8_t thickness = FrameColor>>24;
	if((thickness==0)||(thickness==1)||(thickness==2)||(thickness==255))
		thickness=2;
	int thickness2 = 2*thickness;
	int fillHeight = height-thickness2;
	int fillWidth = width-thickness2;

	_StartDrawLine(posBuff,BkpSizeX,x,y);
	_FillBuff(width, FrameColor);
	for(int i=0;i<thickness-1;++i){
		_NextDrawLine(BkpSizeX,width);
		_FillBuff(width, FrameColor);
	}
	_NextDrawLine(BkpSizeX,width);
	if(fillHeight>0)
	{
		for (int j=0; j<fillHeight; j++)
		{	_FillBuff(thickness, FrameColor);
			_FillBuff(fillWidth, FillColor);
			_FillBuff(thickness, FrameColor);
			_NextDrawLine(BkpSizeX,width);
		}
	}
	_FillBuff(width, FrameColor);
	for(int i=0;i<thickness-1;++i){
		_NextDrawLine(BkpSizeX,width);
		_FillBuff(width, FrameColor);
	}
}
void LCD_BoldFrame(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	uint8_t thickness = FrameColor>>24;
	if((thickness==0)||(thickness==1)||(thickness==2)||(thickness==255))
		thickness=2;
	int thickness2 = 2*thickness;
	int fillHeight = height-thickness2;
	int fillWidth = width-thickness2;

	_StartDrawLine(posBuff,BkpSizeX,x,y);
	_FillBuff(width, FrameColor);
	for(int i=0;i<thickness-1;++i){
		_NextDrawLine(BkpSizeX,width);
		_FillBuff(width, FrameColor);
	}
	_NextDrawLine(BkpSizeX,width);
	if(fillHeight>0)
	{
		for (int j=0; j<fillHeight; j++)
		{	_FillBuff(thickness, FrameColor);
			k+=fillWidth;
			_FillBuff(thickness, FrameColor);
			_NextDrawLine(BkpSizeX,width);
		}
	}
	_FillBuff(width, FrameColor);
	for(int i=0;i<thickness-1;++i){
		_NextDrawLine(BkpSizeX,width);
		_FillBuff(width, FrameColor);
	}
}


void LCD_RoundFrame(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	LCD_DrawRoundRectangleFrame(0,posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FillColor,BkpColor);
}
void LCD_RoundRectangle(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	LCD_DrawRoundRectangleFrame(1,posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FillColor,BkpColor);
}

void LCD_BoldRoundRectangle(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	uint8_t thickness = FrameColor>>24;
	if((thickness==0)||(thickness==1)||(thickness==2)||(thickness==255))
		thickness=1;
	else
		thickness--;
	LCD_DrawRoundRectangleFrame(1,posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FrameColor,BkpColor);
	LCD_DrawRoundRectangleFrame(1,posBuff,BkpSizeX,BkpSizeY,x+thickness,y+thickness,width-2*thickness,height-2*thickness,FrameColor,FillColor,1<<24);
}

void LCD_BoldRoundFrame(uint32_t posBuff, uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	int i,k1,k2;
	uint8_t thickness = FrameColor>>24;
	if((thickness==0)||(thickness==1)||(thickness==2)||(thickness==255))
		thickness=2;
	LCD_DrawRoundRectangleFrame(0,posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FrameColor,BkpColor);
	thickness-=2;
	for(i=0;i<thickness;++i){
		k1=1+i;
		k2=2*k1;
		LCD_DrawRoundRectangleFrame(0,posBuff,BkpSizeX,BkpSizeY,x+k1,y+k1,width-k2,height-k2,FrameColor,FrameColor,1<<24);
	}
	k1=1+i;
	k2=2*k1;
	LCD_DrawRoundRectangleFrame(0,posBuff,BkpSizeX,BkpSizeY,x+k1,y+k1,width-k2,height-k2,FrameColor,FillColor,1<<24);
}

void DrawLine(uint32_t posBuff,uint16_t x0, uint16_t y0, uint16_t len, uint16_t degree, uint32_t lineColor,uint32_t BkpSizeX, float ratioAA1, float ratioAA2 ,uint32_t bk1Color, uint32_t bk2Color)
{
	#define LINES_BUFF_SIZE		len+6

	uint16_t degree_copy=degree;
	uint8_t linesBuff[LINES_BUFF_SIZE];
	int k_iteration=0, searchDirection=0;
	int nrPointsPerLine=0, iteration=1;
	int findNoPoint=0, rot;
	float param_y;
	float param_x;
	float decision;

	void _FFFFFFFFFF(void)
	{
		switch(rot)
		{  case 1: switch(searchDirection){ case 0: k_iteration=-1;	 		 break; case 1: k_iteration=-BkpSizeX-1; break; } break;
			case 2: switch(searchDirection){ case 0: k_iteration=-BkpSizeX; break; case 1: k_iteration=-BkpSizeX-1; break; } break;
			case 3: switch(searchDirection){ case 0: k_iteration=-BkpSizeX; break; case 1: k_iteration=-BkpSizeX+1; break; } break;
			case 4: switch(searchDirection){ case 0: k_iteration=1;     	 break; case 1: k_iteration=-BkpSizeX+1; break; } break;
			case 5: switch(searchDirection){ case 0: k_iteration=1;     	 break; case 1: k_iteration= BkpSizeX+1; break; } break;
			case 6: switch(searchDirection){ case 0: k_iteration=BkpSizeX;  break; case 1: k_iteration= BkpSizeX+1; break; } break;
			case 7: switch(searchDirection){ case 0: k_iteration=BkpSizeX;  break; case 1: k_iteration= BkpSizeX-1; break; } break;
			case 8: switch(searchDirection){ case 0: k_iteration=-1;   		 break; case 1: k_iteration= BkpSizeX-1; break; } break;
		}
	}

	if(degree_copy==0)
		degree_copy=360;

	_StartDrawLine(posBuff,BkpSizeX,x0,y0);
	rot=LCD_SearchLinePoints(1,posBuff,x0,y0,degree_copy,BkpSizeX);
	_FFFFFFFFFF();

	do
	{
		if(LCD_SearchLinePoints(0,posBuff,x0,y0,degree_copy,BkpSizeX)==1)
		{
			nrPointsPerLine++;
		   if(findNoPoint)
		   {
		   	findNoPoint=0;
		   	searchDirection=1-searchDirection;
		   	_FFFFFFFFFF();
		   }
		}
		else
		{
		   k-=k_iteration;
		   searchDirection=1-searchDirection;
		   if(nrPointsPerLine)
		   {
		   	if(iteration<LINES_BUFF_SIZE-2)
		   		linesBuff[iteration++]=nrPointsPerLine;
		   	else break;
		   }
		   nrPointsPerLine=0;
			findNoPoint=1;
			_FFFFFFFFFF();
		}
		k+=k_iteration;
	   pos = _GetPosXY(posBuff,BkpSizeX);

		param_y = pow(y0-pos.y,2);
		param_x = pow(x0-pos.x,2);
		decision = pow((float)(len+1),2);

	}while((param_x+param_y) <= decision);

	if(nrPointsPerLine)
		linesBuff[iteration++]=nrPointsPerLine;
	linesBuff[0]=iteration-1;

	_StartDrawLine(posBuff,BkpSizeX,x0,y0);

	if(correctLine_AA==0)
	{
		switch(rot)
		{
			case 1:  _DrawArrayBuffLeftUp_AA   (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff); break;
			case 2:  _DrawArrayBuffLeftUp_AA   (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff); break;
			case 3:  _DrawArrayBuffRightUp_AA  (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff); break;
			case 4:  _DrawArrayBuffRightUp_AA  (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff); break;
			case 5:  _DrawArrayBuffRightDown_AA(lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff); break;
			case 6:  _DrawArrayBuffRightDown_AA(lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff); break;
			case 7:  _DrawArrayBuffLeftDown_AA (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff); break;
			case 8:  _DrawArrayBuffLeftDown_AA (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff); break;
		}
	}
	else
	{
		int  k_p=k;
		int linePointsStructSize;
		int nmbPxl,repeatPxl,repeatPxl_next;
		int direction, upDown;

		int _GetNmbPxls(void)
		{
			int j,it=0,count1=0;
			char linesBuff_prev=0;

			linesBuff_prev=linesBuff[1];

			for(j=0;j<linesBuff[0];++j){
				if(linesBuff[1+j]==linesBuff_prev) count1++;
				else{
					it++;
					linesBuff_prev=linesBuff[1+j];
					count1=1;
				}
			}
			if(count1){
				it++;
			}
			return it;
		}

		typedef struct
		{	uint16_t nmbPxl;
			uint8_t nmbPxlTheSame;
		}Struct_LinePoints;
		Struct_LinePoints linePoints[_GetNmbPxls()];

		int Pxl_V(uint8_t nmbPxl_V, uint8_t repeat, uint8_t stepAA, int offs, int direction, int upDown)
		{
			if(repeat>1)
			{
				for(int i=0;i<repeat;++i)
				{
					Set_AACoeff_Draw(stepAA,lineColor,bk1Color,0.0);
					pLcd[k_p-			upDown*BkpSizeX]=	buff_AA[offs+1+i];
					Set_AACoeff_Draw(stepAA,lineColor,bk2Color,0.0);
					pLcd[k_p+upDown*nmbPxl_V*BkpSizeX]=	buff_AA[offs+repeat-i];
					 k_p=k_p+upDown*nmbPxl_V*BkpSizeX+direction;
				}
			}
			else
			{
				Set_AACoeff_Draw(stepAA,lineColor,bk1Color,0.0);
				pLcd[k_p-			upDown*BkpSizeX]=	buff_AA[offs];
				Set_AACoeff_Draw(stepAA,lineColor,bk2Color,0.0);
				pLcd[k_p+upDown*nmbPxl_V*BkpSizeX]=	buff_AA[offs];
				 k_p=k_p+upDown*nmbPxl_V*BkpSizeX+direction;
			}
			return 0;
		}

		int Pxl_H(uint8_t nmbPxl_H, uint8_t repeat, uint8_t stepAA, int offs, int direction, int upDown)
		{
			if(repeat>1)
			{
				for(int i=0;i<repeat;++i)
				{
					Set_AACoeff_Draw(stepAA,lineColor,bk1Color,0.0);
					pLcd[k_p-direction]			=	buff_AA[offs+1+i];
					Set_AACoeff_Draw(stepAA,lineColor,bk2Color,0.0);
					pLcd[k_p+direction*nmbPxl_H]=	buff_AA[offs+repeat-i];
					k_p=k_p+upDown*BkpSizeX+direction*nmbPxl_H;
				}
			}
			else
			{
				Set_AACoeff_Draw(stepAA,lineColor,bk1Color,0.0);
				pLcd[k_p-direction]			=	buff_AA[offs];
				Set_AACoeff_Draw(stepAA,lineColor,bk2Color,0.0);
				pLcd[k_p+direction*nmbPxl_H]=	buff_AA[offs];
				k_p=k_p+upDown*BkpSizeX+direction*nmbPxl_H;
			}
			return 0;
		}

		void _GetNmbPxlsAndLoadToBuff(void)
		{
			int j,it=0,count1=0;
			char linesBuff_prev=0;

			linesBuff_prev=linesBuff[1];

			for(j=1;j<linesBuff[0]+1;++j){
				if(linesBuff[j]==linesBuff_prev) count1++;
				else{
					linePoints[it].nmbPxl=linesBuff_prev;
					linePoints[it++].nmbPxlTheSame=count1;
					linesBuff_prev=linesBuff[j];
					count1=1;
				}
			}
			if(count1){
				linePoints[it].nmbPxl=linesBuff_prev;
				linePoints[it++].nmbPxlTheSame=count1;
			}
			linePointsStructSize=it;
		}

		switch(rot)
		{
			case 1:
				if(degree>=45-11 && degree<=45)
					_DrawArrayBuffLeftUp_AA   (lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,0,linesBuff);
				else
					_DrawArrayBuffLeftUp_AA   (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff);
				break;
			case 2:
				if(degree>45 && degree<=45+11)
					_DrawArrayBuffLeftUp_AA   (lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,1,linesBuff);
				else
					_DrawArrayBuffLeftUp_AA   (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff);
			break;

			case 3:
				if(degree<=135 && degree>=135-11)
					_DrawArrayBuffRightUp_AA  (lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,1,linesBuff);
				else
					_DrawArrayBuffRightUp_AA  (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff);
				break;
			case 4:
				if(degree>135 && degree<=135+11)
					_DrawArrayBuffRightUp_AA  (lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,0,linesBuff);
				else
					_DrawArrayBuffRightUp_AA  (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff);
			break;

			case 5:
				if(degree>=225-11 && degree<=225)
					_DrawArrayBuffRightDown_AA(lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,0,linesBuff);
				else
					_DrawArrayBuffRightDown_AA(lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff);
				break;
			case 6:
				if(degree>225 && degree<=225+11)
					_DrawArrayBuffRightDown_AA(lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,1,linesBuff);
				else
					_DrawArrayBuffRightDown_AA(lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff);
				break;

			case 7:
				if(degree<315 && degree>=315-11)
					_DrawArrayBuffLeftDown_AA (lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,1,linesBuff);
				else
					_DrawArrayBuffLeftDown_AA (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,1,linesBuff);
				break;
			case 8:
				if(degree>=315 && degree<=315+11)
					_DrawArrayBuffLeftDown_AA (lineColor, bk1Color, bk2Color, 1.0, 1.0, BkpSizeX,0,linesBuff);
				else
					_DrawArrayBuffLeftDown_AA (lineColor, bk1Color, bk2Color, ratioAA1, ratioAA2, BkpSizeX,0,linesBuff);
				break;
		}

		if((degree>=225-11 && degree<=225)||
		   (degree>=315    && degree<=315+11)||
		   (degree>135     && degree<=135+11)||
		   (degree>=45-11  && degree<=45))
		{
			_GetNmbPxlsAndLoadToBuff();

			if((degree>=225-11 && degree<=225)||
				(degree>135     && degree<=135+11)) direction=1;
			else                                   direction=-1;

			if((degree>=225-11 && degree<=225)||
				(degree>=315    && degree<=315+11)) upDown=1;
			else                                   upDown=-1;

			for(int j=0; j<linePointsStructSize; ++j)
			{
				nmbPxl    = linePoints[j].nmbPxl;
				repeatPxl = linePoints[j].nmbPxlTheSame;

				if(repeatPxl>1)
					Pxl_H(nmbPxl,repeatPxl,repeatPxl+2,1,direction,upDown);
				else{
					if(nmbPxl==2)
						Pxl_H(nmbPxl,repeatPxl,4,4,direction,upDown);
					else
						Pxl_H(nmbPxl,repeatPxl,3,2,direction,upDown);
				}

			}

		}
		else if((degree>225 && degree<=225+11)||
			    (degree<315  && degree>=315-11)||
				 (degree<=135 && degree>=135-11)||
				 (degree>45   && degree<=45+11))
		{
			_GetNmbPxlsAndLoadToBuff();

			if((degree>225  && degree<=225+11)||
				(degree<=135 && degree>=135-11)) direction=1;
			else                               direction=-1;

			if((degree>225 && degree<=225+11)||
				(degree<315 && degree>=315-11)) upDown=1;
			else                               upDown=-1;

			if((degree>=225+6 && degree<=225+11)||
				(degree<=315-6 && degree>=315-11)||
				(degree<=135-6 && degree>=135-11)||
				(degree>=45+6  && degree<=45+11))
			{
				for(int j=0; j<linePointsStructSize; ++j)
				{
					nmbPxl    = linePoints[j].nmbPxl;
					repeatPxl = linePoints[j].nmbPxlTheSame;

					if(repeatPxl>1)
					{
						if(repeatPxl==2)
							Pxl_V(nmbPxl,repeatPxl,4,1,direction,upDown);
						else if(repeatPxl>=3)
							Pxl_V(nmbPxl,repeatPxl,repeatPxl+1,0,direction,upDown);
					}
					else
					{
						if(nmbPxl==1)
							Pxl_V(nmbPxl,repeatPxl,3,2,direction,upDown);
						else
							Pxl_V(nmbPxl,repeatPxl,3,3,direction,upDown);
					}
				}
			}
			else
			{
				for(int j=0; j<linePointsStructSize; ++j)
				{
					nmbPxl = linePoints[j].nmbPxl;
					repeatPxl = linePoints[j].nmbPxlTheSame;
					if(j+1<linePointsStructSize)
						repeatPxl_next = linePoints[j+1].nmbPxlTheSame;

					if(repeatPxl>1){
						if(Pxl_V(nmbPxl,repeatPxl,repeatPxl,0,direction,upDown)) break;
					}
					else{
						if(repeatPxl_next<3){
							if(repeatPxl==1 && j==linePointsStructSize-1){
								if(Pxl_V(nmbPxl,repeatPxl,2,2,direction,upDown)) break;
							}
							else{
								if(Pxl_V(nmbPxl,repeatPxl,5,5,direction,upDown)) break;
							}
						}
						else{
							if(Pxl_V(nmbPxl,repeatPxl,repeatPxl_next,repeatPxl_next,direction,upDown)) break;
						}
					}
				}
			}
		}
	}

	#undef LINES_BUFF_SIZE
}

void LCD_KeyBackspace(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor)
{
	int widthSign=width/2;
	int heightSign=height/2;
	int sizeSignX=height/4;

	if((heightSign%2)!=0) heightSign++;

	LCD_Rectangle(posBuff,BkpSizeX,BkpSizeY,x,y,width,height,FrameColor,FillColor,BkpColor);

	_StartDrawLine(posBuff,BkpSizeX,x+(width-widthSign)/2,y+height/2);
	_DrawRightUp(heightSign/2,heightSign/2, FrameColor,BkpSizeX);
	_DrawRight(widthSign-heightSign/2,FrameColor);
	_DrawDown(heightSign,FrameColor,BkpSizeX);
	_DrawLeft(widthSign-heightSign/2,FrameColor);
	_DrawLeftUp(heightSign/2,heightSign/2, FrameColor,BkpSizeX);

	_StartDrawLine(posBuff,BkpSizeX,x+(width-widthSign)/2+1,y+height/2);
	_CopyDrawPos();
	_DrawRightUp(heightSign/2,heightSign/2, FrameColor,BkpSizeX);
	_SetCopyDrawPos();
	_DrawRightDown(heightSign/2,heightSign/2, FrameColor,BkpSizeX);



	_StartDrawLine(posBuff,BkpSizeX,x+(width-sizeSignX)/2+sizeSignX/2,y+(height-sizeSignX)/2);
	_DrawRightDown(heightSign/2,heightSign/2, FrameColor,BkpSizeX);
	_StartDrawLine(posBuff,BkpSizeX,x+(width-sizeSignX)/2+sizeSignX/2-1,y+(height-sizeSignX)/2);
	_DrawRightDown(heightSign/2,heightSign/2, FrameColor,BkpSizeX);

	_StartDrawLine(posBuff,BkpSizeX,x+(width-sizeSignX)/2+sizeSignX/2+sizeSignX,y+(height-sizeSignX)/2);
	_DrawLeftDown(heightSign/2,heightSign/2, FrameColor,BkpSizeX);
	_StartDrawLine(posBuff,BkpSizeX,x+(width-sizeSignX)/2+sizeSignX/2+sizeSignX-1,y+(height-sizeSignX)/2);
	_DrawLeftDown(heightSign/2,heightSign/2, FrameColor,BkpSizeX);



}

void LCD_Znak_XX(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor)
{


//	_StartLine(0,BkpSizeX,x,y);
//	_DrawRight(30,FrameColor);
//	_DrawDown(15,FrameColor,BkpSizeX);
//	_DrawLeft(30,FrameColor);
//	_DrawUp(15,FrameColor,BkpSizeX);
	//k_p=k;



//	_SetCoeff_AA(1,WHITE,-100,-85,buff_AA);
//	_SetCoeff_AA(2,WHITE,-100,-85,buff_AA);
//	_SetCoeff_AA(3,WHITE,-100,-55,buff_AA);
//	_SetCoeff_AA(4,WHITE,-120,-30,buff_AA);
//	_SetCoeff_AA(5,WHITE,-115,-23,buff_AA);
//	_SetCoeff_AA(10,WHITE,-100,-15,buff_AA);
//	_SetCoeff_AA(20,WHITE,-100,-5,buff_AA);



	_StartDrawLine(0,BkpSizeX,x,y-100);

//	for(int i=0;i<20;++i)
//	{
//		_SetCoeff_AA(2,WHITE,-120,-30); _DrawRightDown_AA(6,2,FrameColor,BkpSizeX);
//		_SetCoeff_AA(3,WHITE,-120,-15); _DrawRightDown_AA(1,1,FrameColor,BkpSizeX);
//	}
//
//	_StartLine(0,BkpSizeX,x,y-50);
//	for(int i=0;i<20;++i)
//	{
//		_SetCoeff_AA(0,WHITE,-100,-55); _DrawRightDown_AA(6,2,FrameColor,BkpSizeX);
//		_SetCoeff_AA(0,WHITE,-70,-35); _DrawRightDown_AA(1,1,FrameColor,BkpSizeX);
//	}









//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(20,WHITE,-100,-5); _DrawRightDown_AA(100,5,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(5,WHITE,-115,-23);_DrawRightDown_AA(100,20,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85);_DrawRightDown_AA(100,50,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(1,WHITE,-100,-85);_DrawRightDown_AA(100,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85);_DrawRightDown_AA(50,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(5,WHITE,-115,-23); _DrawRightDown_AA(20,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(20,WHITE,-100,-5);_DrawRightDown_AA(5,100,FrameColor,BkpSizeX);
//
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(10,WHITE,-100,-15); _DrawRightUp_AA(100,10,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(4,WHITE,-120,-30);_DrawRightUp_AA(100,25,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85); _DrawRightUp_AA(100,50,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(1,WHITE,-100,-85);_DrawRightUp_AA(100,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85);_DrawRightUp_AA(50,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(4,WHITE,-120,-30); _DrawRightUp_AA(25,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(10,WHITE,-100,-15); _DrawRightUp_AA(10,100,FrameColor,BkpSizeX);
//
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(10,WHITE,-100,-15); _DrawLeftUp_AA(100,10,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(4,WHITE,-120,-30);_DrawLeftUp_AA(100,25,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85);_DrawLeftUp_AA(100,50,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(1,WHITE,-100,-85); _DrawLeftUp_AA(100,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85); _DrawLeftUp_AA(50,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(4,WHITE,-120,-30);_DrawLeftUp_AA(25,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(10,WHITE,-100,-15);_DrawLeftUp_AA(10,100,FrameColor,BkpSizeX);
//
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(10,WHITE,-100,-15);_DrawLeftDown_AA(100,10,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(4,WHITE,-120,-30);_DrawLeftDown_AA(100,25,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85); _DrawLeftDown_AA(100,50,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(1,WHITE,-100,-85); _DrawLeftDown_AA(100,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(2,WHITE,-100,-85);_DrawLeftDown_AA(50,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(4,WHITE,-120,-30);_DrawLeftDown_AA(25,100,FrameColor,BkpSizeX);
//	_StartLine(0,BkpSizeX,x,y);  _SetCoeff_AA(10,WHITE,-100,-15);_DrawLeftDown_AA(10,100,FrameColor,BkpSizeX);


	#define PARAM1			WHITE,MYGRAY
	#define PARAM2		FrameColor,BkpSizeX

	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(20,PARAM1,0.39); _DrawRightDown_AA(100,5,PARAM2);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(5,PARAM1, 0.45);_DrawRightDown_AA(100,20,PARAM2);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,PARAM1, 0.39);_DrawRightDown_AA(100,50,PARAM2);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(1,PARAM1, 0.39);_DrawRightDown_AA(100,100,PARAM2);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39);_DrawRightDown_AA(50,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(5,WHITE,MYGRAY, 0.45); _DrawRightDown_AA(20,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(20,WHITE,MYGRAY,0.39);_DrawRightDown_AA(5,100,FrameColor,BkpSizeX);

	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(10,WHITE,MYGRAY,0.39); _DrawRightUp_AA(100,10,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(4,WHITE,MYGRAY, 0.47);_DrawRightUp_AA(100,25,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39); _DrawRightUp_AA(100,50,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(1,WHITE,MYGRAY, 0.39);_DrawRightUp_AA(100,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39);_DrawRightUp_AA(50,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(4,WHITE,MYGRAY, 0.47); _DrawRightUp_AA(25,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(10,WHITE,MYGRAY,0.39); _DrawRightUp_AA(10,100,FrameColor,BkpSizeX);

	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(10,WHITE,MYGRAY,0.39); _DrawLeftUp_AA(100,10,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(4,WHITE,MYGRAY, 0.47);_DrawLeftUp_AA(100,25,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39);_DrawLeftUp_AA(100,50,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(1,WHITE,MYGRAY, 0.39); _DrawLeftUp_AA(100,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39); _DrawLeftUp_AA(50,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(4,WHITE,MYGRAY, 0.47);_DrawLeftUp_AA(25,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(10,WHITE,MYGRAY,0.39);_DrawLeftUp_AA(10,100,FrameColor,BkpSizeX);

	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(10,WHITE,MYGRAY,0.39);_DrawLeftDown_AA(100,10,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(4,WHITE,MYGRAY, 0.47);_DrawLeftDown_AA(100,25,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39); _DrawLeftDown_AA(100,50,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(1,WHITE,MYGRAY, 0.39); _DrawLeftDown_AA(100,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(2,WHITE,MYGRAY, 0.39);_DrawLeftDown_AA(50,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(4,WHITE,MYGRAY, 0.47);_DrawLeftDown_AA(25,100,FrameColor,BkpSizeX);
	_StartDrawLine(0,BkpSizeX,x,y);  Set_AACoeff_Draw(10,WHITE,MYGRAY,0.39);_DrawLeftDown_AA(10,100,FrameColor,BkpSizeX);




	#undef PARAM1
	#undef PARA2

}






void LCD_SetCircleParam(float outRatio, float inRatio, int len, ...){
	va_list va;
	va_start(va,0);
	if(len<MAX_LINE_BUFF_CIRCLE_SIZE){
		Circle.lineBuff[0]=len;
		for(int i=0;i<len;i++){
			if(i>=MAX_LINE_BUFF_CIRCLE_SIZE) break;
			Circle.lineBuff[1+i]=va_arg(va,int);
		}
		Circle.outRatioStart=outRatio;
		Circle.inRatioStart=inRatio;
	}
	va_end(va);
}
void LCD_SetCircleDegrees(int len, ...){
	int i,deg;
	va_list va;
	va_start(va,0);
	for(i=0;i<len;i++){
		if(i==MAX_DEGREE_CIRCLE) break;
		Circle.degree[1+i]=va_arg(va,int);
	}
	Circle.degree[0]=i;
	va_end(va);

	for(i=0;i<len;i++)
	{
		deg=Circle.degree[1+i];
		Circle.tang[i]= tan(TANG_ARG(deg));
		Circle.coeff[i] = 1/ABS(Circle.tang[i]);

		if(deg>0 && deg<=45)
			Circle.rot[i]=1;
		else if(deg>45 && deg<=90)
			Circle.rot[i]=2;
		else if(deg>90 && deg<=135)
			Circle.rot[i]=3;
		else if(deg>135 && deg<=180)
			Circle.rot[i]=4;
		else if(deg>180 && deg<=225)
			Circle.rot[i]=5;
		else if(deg>225 && deg<=270)
			Circle.rot[i]=6;
		else if(deg>270 && deg<=315)
			Circle.rot[i]=7;
		else if(deg>315 && deg<=360)
			Circle.rot[i]=8;
	}

	for(int i=0;i<Circle.degree[0];++i){
		if(Circle.degree[1+i]==0)
			Circle.degree[1+i]=360;
	}
}
void LCD_SetCircleDegreesBuff(int len, uint16_t *buf){
	int i,deg;
	for(i=0;i<len;i++){
		if(i==MAX_DEGREE_CIRCLE) break;
		Circle.degree[1+i]=buf[i];
	}
	Circle.degree[0]=i;

	for(i=0;i<len;i++)
	{
		deg=Circle.degree[1+i];
		Circle.tang[i]= tan(TANG_ARG(deg));
		Circle.coeff[i] = 1/ABS(Circle.tang[i]);

		if(deg>0 && deg<=45)
			Circle.rot[i]=1;
		else if(deg>45 && deg<=90)
			Circle.rot[i]=2;
		else if(deg>90 && deg<=135)
			Circle.rot[i]=3;
		else if(deg>135 && deg<=180)
			Circle.rot[i]=4;
		else if(deg>180 && deg<=225)
			Circle.rot[i]=5;
		else if(deg>225 && deg<=270)
			Circle.rot[i]=6;
		else if(deg>270 && deg<=315)
			Circle.rot[i]=7;
		else if(deg>315 && deg<=360)
			Circle.rot[i]=8;
	}

	for(int i=0;i<Circle.degree[0];++i){
		if(Circle.degree[1+i]==0)
			Circle.degree[1+i]=360;
	}
}
void LCD_SetCircleDegColors(int len, ...){
	int i;
	va_list va;
	va_start(va,0);
	for(i=0;i<len;i++){
		if(i==MAX_DEGREE_CIRCLE) break;
		Circle.degColor[i]=va_arg(va,int);
	}
	va_end(va);
}
void LCD_SetCircleDegColorsBuff(int len, uint32_t *buf){
	int i;
	for(i=0;i<len;i++){
		if(i==MAX_DEGREE_CIRCLE) break;
		Circle.degColor[i]=buf[i];
	}
}
void LCD_SetCirclePercentParam(int len, uint16_t *degBuff, uint32_t *degColorBuff){
	LCD_SetCircleDegreesBuff(len,degBuff);
	LCD_SetCircleDegColorsBuff(len,degColorBuff);
}
void LCD_SetCircleDegree(uint8_t degNr, uint8_t deg){
	if(degNr<MAX_DEGREE_CIRCLE+1) Circle.degree[degNr]=deg;
}
uint16_t LCD_GetCircleDegree(uint8_t degNr){
	if(degNr<MAX_DEGREE_CIRCLE+1) return Circle.degree[degNr];
	else return 0;
}
uint32_t SetParamWidthCircle(uint16_t param, uint32_t width){
	return (width&0xFFFF)|param<<16;
}
void LCD_SetCircleLine(uint8_t lineNr, uint8_t val){
	Circle.lineBuff[lineNr]=val;
}
void LCD_OffsCircleLine(uint8_t lineNr, int offs){
	Circle.lineBuff[lineNr]+=offs;
}
void LCD_SetCircleAA(float outRatio, float inRetio){
	Circle.outRatioStart=outRatio;
	Circle.inRatioStart=inRetio;
}
void LCD_CopyCircleAA(void){
	Circle.outRatioStart_prev=Circle.outRatioStart;
	Circle.inRatioStart_prev=Circle.inRatioStart;
}
void LCD_SetCopyCircleAA(void){
	Circle.outRatioStart=Circle.outRatioStart_prev;
	Circle.inRatioStart=Circle.inRatioStart_prev;
}

uint16_t LCD_GetCircleWidth(void){
	return Circle.width;
}
void LCD_CopyCircleWidth(void){
	Circle.width_prev=Circle.width;
}
void LCD_SetCopyCircleWidth(void){
	Circle.width=Circle.width_prev;
}

uint16_t LCD_CalculateCircleWidth(uint32_t width)
{
	#define buf			Circle.lineBuff

	uint32_t x=0, y=0;
	int matchWidth=0;
	uint32_t _width=(width&0x0000FFFF)-matchWidth;
	uint32_t height=_width;

	GOTO_ToCalculateRadius:
	  _width=(width&0x0000FFFF)-matchWidth;

	uint32_t R=_width/2, err=0.5;
	uint32_t pxl_width = R/3;

	x=_width/2 + matchWidth/2;
	if(_width%2) x++;
	y=0;

	float _x=(float)x, _y=(float)y;
	float x0=(float)x, y0=(float)y+R;

	float param_y = pow(y0-_y,2);
	float param_x = pow(_x-x0,2);
	float decision = pow(R+err,2);

	int pxl_line=0,i=0;

	buf[0]=pxl_width;
	do{
		_x++;  pxl_line++;
		param_x = pow(_x-x0,2);
		if((param_x+param_y) > decision){
			_y++;
			param_y = pow(y0-_y,2);
			buf[1+i++]=pxl_line-1;
			pxl_line=1;
		}
	}while(i<pxl_width);

   uint16_t _height=buf[0];
  	for(int i=0;i<buf[0];++i) _height+=buf[buf[0]-i];
  	_height = 2*_height;

	if(height>=_height)
		y= y+(height-_height)/2;
	else{
		matchWidth+=1;
		goto GOTO_ToCalculateRadius;
	}
	return _height;
}
//MOZLIWOSC rysowania polkul np dla zakonczen ramek !!!!!!!
void LCD_Circle(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x, uint32_t y, uint32_t _width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	uint32_t width = _width&0xFFFF;
	uint16_t param = _width>>16;
	uint8_t thickness = FrameColor>>24;
	if((thickness==0)||(thickness==255))
		thickness=0;
	else
		thickness=3*thickness;

	if((param==Percent_Circle)||(param==Degree_Circle))   //Degree_Circle usunac
		thickness=0;

	if(thickness){
		LCD_DrawCircle(posBuff,BkpSizeX,BkpSizeY,x,y, width,height, FrameColor, FrameColor, BkpColor);
		LCD_CopyCircleWidth();

   	uint32_t width_new = width-2*thickness;
		int offs= (Circle.width-LCD_CalculateCircleWidth(width_new))/2;

		LCD_DrawCircle(posBuff,BkpSizeX,BkpSizeY,x+offs,y+offs, width_new,width_new, FrameColor, FillColor, FrameColor);
		LCD_SetCopyCircleWidth();
	}
	else LCD_DrawCircle(posBuff,BkpSizeX,BkpSizeY,x,y, _width,height, FrameColor, FillColor, BkpColor);
}

void LCD_HalfCircle(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor){
	uint8_t thickness = FrameColor>>24;
	if((thickness==0)||(thickness==255))
		thickness=0;
	else
		thickness=3*thickness;

	if(thickness){
		LCD_DrawHalfCircle(posBuff,BkpSizeX,BkpSizeY,x,y, width,height, FrameColor, FrameColor, BkpColor);
		LCD_CopyCircleWidth();

		int whatRotate= width>>16;
   	uint32_t width_new = (width&0x0000FFFF) - 2*thickness;
   	int offs= (Circle.width-LCD_CalculateCircleWidth(width_new))/2;

		LCD_DrawHalfCircle(posBuff,BkpSizeX,BkpSizeY,x+offs,y+offs, SetParamWidthCircle(whatRotate,width_new),width_new, FrameColor, FillColor, FrameColor);
		LCD_SetCopyCircleWidth();
	}
	else LCD_DrawHalfCircle(posBuff,BkpSizeX,BkpSizeY,x,y, width,height, FrameColor, FillColor, BkpColor);
}



