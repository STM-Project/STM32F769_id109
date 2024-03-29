/*
 * LCD_fonts_images.c
 *
 *  Created on: 20.04.2021
 *      Author: Elektronika RM
 */

#include "LCD_fonts_images.h"
#include <string.h>
#include <stdbool.h>
#include "LCD_Common.h"
#include "LCD_Hardware.h"
#include "common.h"
#include "FreeRTOS.h"
#include "ff.h"
#include "sd_card.h"
#include "errors_service.h"
#include "debug.h"

#define MAX_FONTS_AND_IMAGES_MEMORY_SIZE	0x600000
#define LCD_MOVABLE_FONTS_BUFF_SIZE		LCD_BUFF_XSIZE * LCD_BUFF_YSIZE

#define MAX_OPEN_FONTS_SIMULTANEOUSLY	 17
#define MAX_CHARS		256
#define POSITION_AND_WIDTH		2

#define MAX_OPEN_IMAGES_SIMULTANEOUSLY	 300
#define MAX_IMAGE_NAME_LEN		30
#define MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY	 40
#define MAX_SPACE_CORRECT	100

#define MAX_SIZE_CHANGECOLOR_BUFF	300

static const char CharsTab_full[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+-.,:;[]{}<>'~*()&#^=_$%°@|?!¥¹ÆæÊê£³ÑñÓóŒœŸ¯¿/1234567890";
static const char CharsTab_digits[]="+-1234567890.";

extern uint32_t pLcd[LCD_BUFF_XSIZE*LCD_BUFF_YSIZE];

static uint32_t buffChangeColorIN[MAX_SIZE_CHANGECOLOR_BUFF]={0};
static uint32_t buffChangeColorOUT[MAX_SIZE_CHANGECOLOR_BUFF]={0};
static int idxChangeColorBuff=0;
static int fontsTabPos_temp[MAX_CHARS][POSITION_AND_WIDTH];
static StructTxtPxlLen StructTxtPxlLen_ZeroValue={0,0,0};

SDRAM static char fontsImagesMemoryBuffer[MAX_FONTS_AND_IMAGES_MEMORY_SIZE];

static int movableFontsBuffer_pos;
SDRAM static uint32_t movableFontsBuffer[LCD_MOVABLE_FONTS_BUFF_SIZE];

typedef struct{
	uint32_t size;
	uint32_t style;
	uint32_t bkColor;
	uint32_t color;
} ID_FONT;
static ID_FONT FontID[MAX_OPEN_FONTS_SIMULTANEOUSLY];

typedef struct{
	uint32_t fontSizeToIndex;
	uint32_t fontStyleToIndex;
	uint32_t fontBkColorToIndex;
	uint32_t fontColorToIndex;
	int fontsTabPos[MAX_CHARS][POSITION_AND_WIDTH];
	int height;
	int heightHalf;
	char *pointerToMemoryFont;
	uint32_t fontSdramLenght;
} FONTS_SETTING;
static FONTS_SETTING Font[MAX_OPEN_FONTS_SIMULTANEOUSLY];

typedef struct{
	uint16_t imagesNumber;
	uint16_t actualImage;
	portTickType everyTime;
} ANIMATION_SETTING;

typedef struct{
	char name[MAX_IMAGE_NAME_LEN];
	uint8_t *pointerToMemory;
	uint32_t sdramLenght;
	ANIMATION_SETTING Animation;
} IMAGES_SETTING;
static IMAGES_SETTING Image[MAX_OPEN_IMAGES_SIMULTANEOUSLY];

typedef struct
{
	uint32_t colorIn[2];
	uint32_t colorOut[2];
	float aY;
	float aCr;
	float aCb;
	float bY;
	float bCr;
	float bCb;
}FontCoeff;
static FontCoeff coeff;

typedef struct{
	uint32_t posBuff;
	uint16_t xImgWidth;
	uint16_t yImgHeight;
	uint16_t posWin;
	uint16_t windowWidth;
	uint16_t windowHeight;
	uint16_t pxlTxtLen;
	uint16_t spaceEndStart;
}MOVABLE_FONTS_SETTING;

typedef struct{
	uint16_t id;
	uint16_t xPos;
	uint16_t yPos;
	int8_t heightType;
	uint8_t space;
	uint32_t bkColor;
	uint32_t bkScreenColor;
	uint32_t fontColor;
	uint8_t rotate;
	int coeff;
	uint8_t widthType;
	uint16_t xPos_prev;
	uint16_t yPos_prev;
	uint16_t widthPxl_prev;
	uint16_t heightPxl_prev;
	MOVABLE_FONTS_SETTING FontMov;
} FONTS_VAR_SETTING;
static FONTS_VAR_SETTING FontVar[MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY];

typedef struct
{
	uint8_t fontStyle;
	uint8_t fontSize;
	char char1;
	char char2;
	int8_t val;
}Struct_SpaceCorrect;
static Struct_SpaceCorrect space[MAX_SPACE_CORRECT];
static uint8_t StructSpaceCount=0;

static uint32_t CounterBusyBytesForFontsImages=0;

//####################### -- My Settings -- #######################################################################

static int MyRealizeSpaceCorrect(char *txt, int id)
{
//		if((FONT_20==FontID[id].size)&&(Times_New_Roman==FontID[id].style))
//		{
//			if((txt[0]=='i')&&(txt[1]=='j'))
//				return 20;
//		}
	return 0;
}

static int RealizeWidthConst(const char _char)
{
	if(((_char > 0x2F) && (_char < 0x3A)) || (_char == ':') || (_char == '-'))
		return 1;
	else
		return 0;
}

//################################################################################################################

static void LCD_CopyBuff2pLcd(int rot, uint32_t posBuff, uint32_t *buff, uint32_t xImgWidth, uint32_t yImgHeight, int posWin, uint16_t windowWidth, uint16_t xPosLcd, uint16_t yPosLcd, int param)
{
	uint32_t n=0, offsX,offsY, pos=posBuff+posWin, posLcd=yPosLcd*LCD_GetXSize()+xPosLcd;
	switch(rot)
	{
	case Rotate_0:
		for(int j=0; j<yImgHeight; ++j){
			for(int i=0; i<windowWidth; ++i)
				pLcd[posLcd+i] = buff[pos+n++];
			n+=xImgWidth-windowWidth;
			posLcd+=LCD_GetXSize();
		}
		break;
	case Rotate_90:
		offsX=yImgHeight-1;
		for(int j=0; j<yImgHeight; ++j){
			for(int i=0; i<windowWidth; ++i)
				pLcd[posLcd+offsX+i*LCD_GetXSize()] = buff[pos+n++];
			n+=xImgWidth-windowWidth;
			offsX--;
		}
		break;
	case Rotate_180:
		offsX=0;
		offsY=param;
		for(int j=0; j<yImgHeight; ++j){
			for(int i=0; i<windowWidth; ++i)
				pLcd[posLcd+offsX+(offsY+windowWidth-1-i)*LCD_GetXSize()] = buff[pos+n++];
			n+=xImgWidth-windowWidth;
			offsX++;
		}
		break;
	}
}

static void LCD_CopyBuff2pLcdIndirect(int rot, uint32_t posBuff, uint32_t *buff, uint32_t xImgWidth, uint32_t yImgHeight, int posWin, uint16_t windowWidth, int param)
{
	uint32_t offsX,offsY,k=0,n=0,pos=posBuff+posWin;
	switch(rot)
	{
	case Rotate_0:
		for(int j=0; j<yImgHeight; ++j){
			for(int i=0; i<windowWidth; ++i)
				pLcd[k++] = buff[pos+n++];
			n+=xImgWidth-windowWidth;
		}
		break;
	case Rotate_90:
		offsX=yImgHeight-1;
		for(int j=0; j<yImgHeight; ++j){
			for(int i=0; i<windowWidth; ++i)
				pLcd[offsX+i*yImgHeight] = buff[pos+n++];
			n+=xImgWidth-windowWidth;
			offsX--;
		}
		break;
	case Rotate_180:
		offsX=0;
		offsY=param;
		for(int j=0; j<yImgHeight; ++j){
			for(int i=0; i<windowWidth; ++i)
				pLcd[offsX+(offsY+windowWidth-1-i)*yImgHeight] = buff[pos+n++];
			n+=xImgWidth-windowWidth;
			offsX++;
		}
		break;
	}
}

static uint32_t k;
static void _StartLine(uint32_t posBuff,uint32_t BkpSizeX,uint32_t x,uint32_t y){
	k=posBuff+(y*BkpSizeX+x);
}
static void _NextLine(uint32_t BkpSizeX,uint32_t width){
	k+=(BkpSizeX-width);
}
static void _FillBuff(uint32_t *buff, int itCount, uint32_t color)
{
	if(itCount>10)
	{
		int j=itCount/2;
		int a=j;

		uint64_t *pLcd64=(uint64_t*) (buff+k);
		uint64_t color64=(((uint64_t)color)<<32)|((uint64_t)color);

		j--;
		while (j)
			pLcd64[j--]=color64;

		pLcd64[j]=color64;
		k+=a+itCount/2;

		if (itCount%2)
			buff[k++]=color;
	}
	else
	{
		for(int i=0;i<itCount;++i)
			buff[k++]=color;
	}
}
static void LCD_RectangleBuff(uint32_t *buff, uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY, uint32_t x,uint32_t y, uint32_t width, uint32_t height, uint32_t FrameColor, uint32_t FillColor, uint32_t BkpColor)
{
	_StartLine(posBuff,BkpSizeX,x,y);
	_FillBuff(buff, width, FrameColor);
	if(height>1)
	{
		_NextLine(BkpSizeX,width);
		for (int j=0; j<height-2; j++)
		{
			if(width>1)
			{
				_FillBuff(buff,1, FrameColor);
				_FillBuff(buff,width-2, FillColor);
				_FillBuff(buff,1, FrameColor);
				_NextLine(BkpSizeX,width);
			}
			else
			{
				_FillBuff(buff,width, FillColor);
				_NextLine(BkpSizeX,width);
			}
		}
		_FillBuff(buff,width, FrameColor);
	}
}

static int RealizeSpaceCorrect(char *txt, int id)
{
	if(StructSpaceCount){
		for(int i=0;i < StructSpaceCount;i++){
			if((FontID[id].style == space[i].fontStyle) && (FontID[id].size == space[i].fontSize)){
				if((txt[0] == space[i].char1) && (txt[1] == space[i].char2))
					return space[i].val + MyRealizeSpaceCorrect(txt,id);
	}}}
	return MyRealizeSpaceCorrect(txt,id);
}

static void CorrectFloatRange(float *data, float rangeDown, float rangeUp)
{
	if(*data>rangeUp)
		*data=rangeUp;
	else if(*data<rangeDown)
		*data=rangeDown;
}

static void CalculateFontCoeff(uint32_t colorIn_1, uint32_t colorIn_2, uint32_t colorOut_1, uint32_t colorOut_2,uint8_t maxVal)
{
	uint32_t R,G,B;
	float Y_in[2],  Cr_in[2],  Cb_in[2];
	float Y_out[2], Cr_out[2], Cb_out[2];

	coeff.colorIn[0]	= 0x00FFFFFF & colorIn_1;
	coeff.colorIn[1]	= 0x00FFFFFF & colorIn_2;
	coeff.colorOut[0]	= 0x00FFFFFF & colorOut_1;
	coeff.colorOut[1]	= 0x00FFFFFF & colorOut_2;

	void _Color2YCrCb(int nr, uint32_t color, float *y, float *cr, float *cb)
	{
		R= (color>>16)&0x000000FF;
		G= (color>>8) &0x000000FF;
		B=  color     &0x000000FF;

		if(nr)
		{
			if(R>maxVal) R=maxVal;
			if(G>maxVal) G=maxVal;
			if(B>maxVal) B=maxVal;
		}

		*y = _Y (R,G,B);
		*cr= _Cr(R,G,B);
		*cb= _Cb(R,G,B);
	}

	_Color2YCrCb(0,coeff.colorIn[0], &Y_in[0], &Cr_in[0], &Cb_in[0]);
	_Color2YCrCb(0,coeff.colorIn[1], &Y_in[1], &Cr_in[1], &Cb_in[1]);

	_Color2YCrCb(1,coeff.colorOut[0], &Y_out[0], &Cr_out[0], &Cb_out[0]);
	_Color2YCrCb(1,coeff.colorOut[1], &Y_out[1], &Cr_out[1], &Cb_out[1]);

	coeff.aY  = (Y_out[1] -Y_out[0])  / (Y_in[1] -Y_in[0]);
	coeff.aCr = (Cr_out[1]-Cr_out[0]) / (Cr_in[1]-Cr_in[0]);
	coeff.aCb = (Cb_out[1]-Cb_out[0]) / (Cb_in[1]-Cb_in[0]);

	coeff.bY  = Y_out[0]  - coeff.aY  * Y_in[0];
	coeff.bCr = Cr_out[0] - coeff.aCr * Cr_in[0];
	coeff.bCb = Cb_out[0] - coeff.aCb * Cb_in[0];
}

static uint32_t GetCalculatedRGB(uint8_t red, uint8_t green, uint8_t blue)
{
	uint32_t temp = RGB2INT(red,green,blue);

   for(int i=0; i<idxChangeColorBuff; i++){
   	if(buffChangeColorIN[i]==temp)
   			return buffChangeColorOUT[i];
   }
   buffChangeColorIN[idxChangeColorBuff]=temp;

	float Y  = coeff.aY  * _Y (red,green,blue) + coeff.bY;
	float Cr = coeff.aCr * _Cr(red,green,blue) + coeff.bCr;
	float Cb = coeff.aCb * _Cb(red,green,blue) + coeff.bCb;

	CorrectFloatRange(&Y,0,255);
	CorrectFloatRange(&Cr,0,255);
	CorrectFloatRange(&Cb,0,255);

	temp = RGB2INT((uint32_t)_R(Y,Cb,Cr),(uint32_t)_G(Y,Cb,Cr),(uint32_t)_B(Y,Cb,Cr));

   buffChangeColorOUT[idxChangeColorBuff++]=temp;
   if(idxChangeColorBuff>MAX_SIZE_CHANGECOLOR_BUFF){
   	ERROR_StrChangeColor();
   	return 0;
   }
	return temp;
}

static int LoadFontIndex(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
    int i;
    for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
    {
    	if(0==Font[i].fontSizeToIndex)
    	{
    		Font[i].fontSizeToIndex = fontSize+1;
    		Font[i].fontStyleToIndex = fontStyle;
    		Font[i].fontBkColorToIndex = backgroundColor;
    		Font[i].fontColorToIndex = fontColor;
    		return i;
    	}
    }
    return -1;
}

static int SearchFontIndex(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
    int i;
    for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
    {
    	if(((fontSize+1)	==	Font[i].fontSizeToIndex)&&
    		(fontStyle	    ==	Font[i].fontStyleToIndex)&&
			(backgroundColor==	Font[i].fontBkColorToIndex)&&
			(fontColor	    ==	Font[i].fontColorToIndex))
    		return i;
    }
    return -1;
}

static uint8_t ReturnFontSize(int fontIndex){
	return Font[fontIndex].fontSizeToIndex-1;
}

static void SetFontHeightHalf(int fontIndex, int heightHalf){
	Font[fontIndex].heightHalf = heightHalf;
}

static int LCD_GetFontID(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor)
{
    int i;
    for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
    {
    	if((fontSize			==	FontID[i].size)&&
    	   (fontStyle	    	==	FontID[i].style)&&
		   (backgroundColor	==	FontID[i].bkColor)&&
		   (fontColor	   	==	FontID[i].color))
    			return i;
    }
    return -1;
}

static bool DynamicFontMemoryAllocation(uint32_t fontFileSize, int fontIndex)
{
	if(CounterBusyBytesForFontsImages+fontFileSize < MAX_FONTS_AND_IMAGES_MEMORY_SIZE)
	{
		Font[fontIndex].fontSdramLenght=fontFileSize;
		Font[fontIndex].pointerToMemoryFont=(char*)( fontsImagesMemoryBuffer + CounterBusyBytesForFontsImages );
		CounterBusyBytesForFontsImages += fontFileSize;
		return true;
	}
	else
		return false;
}

static void SearchFontsField(char *pbmp, uint32_t width, uint32_t height, uint32_t bit_pixel, uint32_t *shiftXpos, uint8_t *backGround)
{
	int i,j,k;
	char *pbmp1;
	j=0;
	do
	{
		pbmp1=pbmp+3*(j+*shiftXpos);
		k=0;
		for(i=0; i < height; i++)
		{
			if((*(pbmp1+0)==backGround[0])&&(*(pbmp1+1)==backGround[1])&&(*(pbmp1+2)==backGround[2]))
				k++;
			else
				break;
			pbmp1 -= width*bit_pixel;
		}
		if(k!=height)
		{
			*shiftXpos+=j;
			break;
		}
		j++;
	}while(1);
}

static uint32_t CountCharLenght(char *pbmp, uint32_t width, uint32_t height, uint32_t bit_pixel, uint32_t *shiftXpos, uint8_t *backGround)
{
	int i,j,k;
	char *pbmp1;
	uint32_t charLenght=0;
	j=0;
	do
	{
		pbmp1=pbmp+3*(j+*shiftXpos);
		k=0;
		for(i=0; i < height; i++)
		{
			if((*(pbmp1+0)==backGround[0])&&(*(pbmp1+1)==backGround[1])&&(*(pbmp1+2)==backGround[2]))
				k++;
			else
			{
				k=0;
				break;
			}
			pbmp1 -= width*bit_pixel;
		}
		if(k==height)
		{
			charLenght+=j;
			break;
		}
		j++;

	}while(1);
	return charLenght;
}

static int CountHalfHeightForDot(char *pbmp, uint32_t width, uint32_t height, uint32_t bit_pixel, uint32_t shiftXpos, uint8_t *backGround)
{
	int i,j=0,m=0;
	char *pbmp1;

	pbmp1=pbmp + 3 * (j + shiftXpos);
	m=0;
	for(i=0;i < height;i++)
	{
		if((*(pbmp1 + 0) == backGround[0]) && (*(pbmp1 + 1) == backGround[1]) && (*(pbmp1 + 2) == backGround[2]))
		{
			if(m == 1)
				return i;
		}
		else
			m=1;
		pbmp1-=width * bit_pixel;
	}
	return -1;
}

static void SearchCurrentFont_TablePos(char *pbmp, int fontIndex)
{
	const char *pChar;
	uint8_t fontSize=ReturnFontSize(fontIndex);

	switch(fontSize)
	{
	case FONT_72:
	case FONT_72_bold:
	case FONT_72_italics:
	case FONT_130:
	case FONT_130_bold:
	case FONT_130_italics:
		pChar=CharsTab_digits;
		break;
	default:
		pChar=CharsTab_full;
		break;
	}

	int j, lenTab=strlen(pChar);
	uint32_t shiftXpos=0, index = 0, width = 0, height = 0, bit_pixel = 0;
	uint8_t backGround[3];

	/* Get bitmap data address offset */
	index = pbmp[10] + (pbmp[11] << 8) + (pbmp[12] << 16)  + (pbmp[13] << 24);

	/* Read bitmap width */
	width = pbmp[18] + (pbmp[19] << 8) + (pbmp[20] << 16)  + (pbmp[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4

	/* Read bitmap height */
	height = pbmp[22] + (pbmp[23] << 8) + (pbmp[24] << 16)  + (pbmp[25] << 24);

	/* Read bit/pixel */
	bit_pixel = pbmp[28] + (pbmp[29] << 8);
	bit_pixel/=8;

	pbmp += (index + (width * height * bit_pixel));
	pbmp -= width*bit_pixel;

	backGround[0]=pbmp[0];
	backGround[1]=pbmp[1];
	backGround[2]=pbmp[2];

	Font[fontIndex].height=height;

	for(j=0; j < lenTab; j++)
	{
		SearchFontsField(pbmp,width,height,bit_pixel,&shiftXpos,backGround);
		if(pChar[j]=='.')  SetFontHeightHalf(fontIndex, CountHalfHeightForDot(pbmp,width,height,bit_pixel,shiftXpos,backGround)+2);
		Font[fontIndex].fontsTabPos[ (int)pChar[j] ][1] = CountCharLenght(pbmp,width,height,bit_pixel,&shiftXpos,backGround);
		Font[fontIndex].fontsTabPos[ (int)pChar[j] ][0] = shiftXpos;
		shiftXpos += (Font[fontIndex].fontsTabPos[ (int)pChar[j] ][1]+1);
		if(j==0)
			Font[fontIndex].fontsTabPos[(int)' '][1] = (2*Font[fontIndex].fontsTabPos[ (int)pChar[j] ][1])/3; //spacja na podstawie 2/3 d³ugosci pierwszej litery z pChar
	}
	Font[fontIndex].fontsTabPos[(int)' '][0] = shiftXpos;
}

static void LCD_Set_ConstWidthFonts(int fontIndex)
{
	const char *pChar;
	uint8_t fontSize=ReturnFontSize(fontIndex);

	switch(fontSize)
	{
	case FONT_72:
	case FONT_72_bold:
	case FONT_72_italics:
	case FONT_130:
	case FONT_130_bold:
	case FONT_130_italics:
		pChar=CharsTab_digits;
		break;
	default:
		pChar=CharsTab_full;
		break;
	}
	int j, lenTab=strlen(pChar);

	int maxWidth=0;
	for(j=0;j < lenTab;j++)
	{
		if(RealizeWidthConst(pChar[j]))
		{
			if(Font[fontIndex].fontsTabPos[(int) pChar[j]][1] > maxWidth)
				maxWidth=Font[fontIndex].fontsTabPos[(int) pChar[j]][1];
		}
	}
	for(j=0;j < lenTab;j++)
	{
		if(RealizeWidthConst(pChar[j]))
		{
			fontsTabPos_temp[(int) pChar[j]][0]=Font[fontIndex].fontsTabPos[(int) pChar[j]][0];
			fontsTabPos_temp[(int) pChar[j]][1]=Font[fontIndex].fontsTabPos[(int) pChar[j]][1];

			Font[fontIndex].fontsTabPos[(int) pChar[j]][0]-=(maxWidth - Font[fontIndex].fontsTabPos[(int) pChar[j]][1]) / 2;
			Font[fontIndex].fontsTabPos[(int) pChar[j]][1]=maxWidth;
		}
	}
	fontsTabPos_temp[(int) ' '][1]=Font[fontIndex].fontsTabPos[(int) ' '][1];
	Font[fontIndex].fontsTabPos[(int) ' '][1]=maxWidth;
}

static void LCD_Reset_ConstWidthFonts(int fontIndex)
{
	const char *pChar;
	uint8_t fontSize=ReturnFontSize(fontIndex);

	switch(fontSize)
	{
	case FONT_72:
	case FONT_72_bold:
	case FONT_72_italics:
	case FONT_130:
	case FONT_130_bold:
	case FONT_130_italics:
		pChar=CharsTab_digits;
		break;
	default:
		pChar=CharsTab_full;
		break;
	}
	int j, lenTab=strlen(pChar);

	for(j=0; j < lenTab; j++)
	{
		if(RealizeWidthConst(pChar[j]))
		{
			Font[fontIndex].fontsTabPos[ (int)pChar[j] ][0] = fontsTabPos_temp[(int)pChar[j]][0];
			Font[fontIndex].fontsTabPos[ (int)pChar[j] ][1] = fontsTabPos_temp[(int)pChar[j]][1];
		}
	}
	Font[fontIndex].fontsTabPos[(int)' '][1] = fontsTabPos_temp[(int)' '][1];
}

static StructTxtPxlLen LCD_DrawStrToBuff(uint32_t posBuff,uint32_t windowX,uint32_t windowY,int id, int X, int Y, char *txt, uint32_t *LcdBuffer,int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth)
{
	StructTxtPxlLen structTemp={0,0,0};
	int fontIndex=SearchFontIndex(FontID[id].size, FontID[id].style, FontID[id].bkColor, FontID[id].color);
	if(fontIndex==-1)
		return structTemp;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,o,temp,lenTxt,lenTxtInPixel=0;
	int posX=X, posY=Y;
	char *pbmp;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint32_t backGround;
	uint32_t pos, pos2, xi;
	uint32_t Y_bkColor;

	if(constWidth)
		LCD_Set_ConstWidthFonts(fontIndex);

//	touchTemp.x_Start= posX;
//	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);
	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4
	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);
	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround= fontsBuffer[2]<<16 | fontsBuffer[1]<<8 | fontsBuffer[0];

	if(OnlyDigits==halfHight)
		height=Font[fontIndex].heightHalf;

	j=strlen(txt);
	for(i=0;i<j;i++)
	{
		temp = Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,id);
		if(posX+lenTxtInPixel+temp <= windowX)
			lenTxtInPixel += temp;
		else break;
	}
	lenTxt=i;

	if(bkColor)
	{
		LCD_RectangleBuff(LcdBuffer,posBuff,windowX,windowY,X,Y,lenTxtInPixel, Y+height>windowY?windowY-Y:height, bkColor,bkColor,bkColor);
		Y_bkColor= COLOR_TO_Y(bkColor)+coeff;
	}

	for(n=0;n<lenTxt;++n)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= posBuff+(windowX*posY + posX);
		xi=Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

		for(j=0; j < height; ++j)
		{
			if(Y+j>=windowY)
				break;
			pos=pos2+windowX*j;
			o=0;
			for(i=0; i<xi; ++i)
			{
				if ((*((uint32_t*)(pbmp+o))&0x00FFFFFF)!=backGround)
				{
					LcdBuffer[pos+i]= *((uint32_t*)(pbmp+o));

					if(coeff!=0)
					{
						if(coeff>0)
						{
							if(COLOR_TO_Y(LcdBuffer[pos+i]) < Y_bkColor)
								LcdBuffer[pos+i]= bkColor;
						}
						else
						{
							if(COLOR_TO_Y(LcdBuffer[pos+i]) > Y_bkColor)
								LcdBuffer[pos+i]= bkColor;
						}
					}
				}
				o+=3;
			}
			pbmp -= width*bit_pixel;
		}
		posX += xi;
		posX += space + RealizeSpaceCorrect(txt+n,id);
	}

	if(constWidth)
		LCD_Reset_ConstWidthFonts(fontIndex);
//	touchTemp.x_End= posX;
//	touchTemp.y_End= posY+height;
	structTemp.inChar=lenTxt;
	structTemp.inPixel=lenTxtInPixel;
	structTemp.height=j;
	return structTemp;
}
static StructTxtPxlLen LCD_DrawStrIndirectToBuffAndDisplay(uint32_t posBuff, int displayOn, uint32_t maxSizeX, uint32_t maxSizeY, int id, int X, int Y, char *txt, uint32_t *LcdBuffer,int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth)
{
	StructTxtPxlLen structTemp={0,0,0};
	int fontIndex=SearchFontIndex(FontID[id].size, FontID[id].style, FontID[id].bkColor, FontID[id].color);
	if(fontIndex==-1)
		return structTemp;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,o,lenTxt,temp,lenTxtInPixel=0;
	int posX=0;
	char *pbmp;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint32_t backGround;
	uint32_t pos, pos2, xi;
	uint32_t Y_bkColor;

	if(constWidth)
		LCD_Set_ConstWidthFonts(fontIndex);

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);
	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4
	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);
	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround= fontsBuffer[2]<<16 | fontsBuffer[1]<<8 | fontsBuffer[0];

	if(OnlyDigits==halfHight)
		height=Font[fontIndex].heightHalf;

	j=strlen(txt);
	for(i=0;i<j;i++)
	{
		temp = Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,id);
		if(X+lenTxtInPixel+temp <= maxSizeX)
			lenTxtInPixel += temp;
		else break;
	}
	lenTxt=i;

	if(bkColor){
		LCD_RectangleBuff(LcdBuffer,posBuff,lenTxtInPixel,height,0,0,lenTxtInPixel,Y+height>maxSizeY?maxSizeY-Y:height,bkColor,bkColor,bkColor);
		Y_bkColor= COLOR_TO_Y(bkColor)+coeff;
	}

	for(n=0;n<lenTxt;++n)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= posBuff+posX;
		xi=Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

		for(j=0; j < height; ++j)
		{
			if(Y+j>=maxSizeY)
				break;
			pos=pos2+lenTxtInPixel*j;
			o=0;
			for(i=0; i<xi; ++i)
			{
				if ((*((uint32_t*)(pbmp+o))&0x00FFFFFF)!=backGround)
				{
					LcdBuffer[pos+i]= *((uint32_t*)(pbmp+o));

					if(coeff!=0)
					{
						if(coeff>0)
						{
							if(COLOR_TO_Y(LcdBuffer[pos+i]) < Y_bkColor)
								LcdBuffer[pos+i]= bkColor;
						}
						else
						{
							if(COLOR_TO_Y(LcdBuffer[pos+i]) > Y_bkColor)
								LcdBuffer[pos+i]= bkColor;
						}
					}
				}
				o+=3;
			}
			pbmp -= width*bit_pixel;
		}
		posX += xi;
		posX += space + RealizeSpaceCorrect(txt+n,id);
	}

	if(constWidth)
		LCD_Reset_ConstWidthFonts(fontIndex);

	if(displayOn)
		LCD_DisplayBuff((uint32_t)X,(uint32_t)Y,(uint32_t)lenTxtInPixel,(uint32_t)j,LcdBuffer+posBuff);
	structTemp.inChar=lenTxt;
	structTemp.inPixel=lenTxtInPixel;
	structTemp.height=j;
	return structTemp;

}
static StructTxtPxlLen LCD_DrawStrChangeColorToBuff(uint32_t posBuff,uint32_t windowX,uint32_t windowY,int id, int X, int Y, char *txt, uint32_t *LcdBuffer,int OnlyDigits, int space, uint32_t NewBkColor, uint32_t NewFontColor, int constWidth)
{
	StructTxtPxlLen structTemp={0,0,0};
	int fontIndex=SearchFontIndex(FontID[id].size, FontID[id].style, FontID[id].bkColor, FontID[id].color);
	if(fontIndex==-1)
		return structTemp;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,o,lenTxt,temp,lenTxtInPixel=0;
	int posX=X, posY=Y;
	char *pbmp;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint32_t backGround;
	uint32_t pos, pos2, xi;

	if(constWidth)
		LCD_Set_ConstWidthFonts(fontIndex);

//	touchTemp.x_Start= posX;
//	touchTemp.y_Start= posY;

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);
	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4
	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);
	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround= fontsBuffer[2]<<16 | fontsBuffer[1]<<8 | fontsBuffer[0];

	if(OnlyDigits==halfHight)
		height=Font[fontIndex].heightHalf;

	j=strlen(txt);
	for(i=0;i<j;i++)
	{
		temp = Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,id);
		if(posX+lenTxtInPixel+temp <= windowX)
			lenTxtInPixel += temp;
		else break;
	}
	lenTxt=i;

	LCD_RectangleBuff(LcdBuffer,posBuff,windowX,windowY,X,Y,lenTxtInPixel,Y+height>windowY?windowY-Y:height,NewBkColor,NewBkColor,NewBkColor);
	idxChangeColorBuff=0;
   for(i=0;i<MAX_SIZE_CHANGECOLOR_BUFF;++i){
   	buffChangeColorIN[i]=0;
   	buffChangeColorOUT[i]=0;
   }

	for(n=0;n<lenTxt;++n)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= posBuff+(windowX*posY + posX);
		xi=Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

		for(j=0; j < height; ++j)
		{
			if(Y+j>=windowY)
				break;
			pos=pos2+windowX*j;
			o=0;
			for(i=0; i<xi; ++i)
			{
				if ((*((uint32_t*)(pbmp+o))&0x00FFFFFF)!=backGround)
					LcdBuffer[pos+i] = GetCalculatedRGB(*(pbmp+o+2),*(pbmp+o+1),*(pbmp+o+0));
				o+=3;
			}
			pbmp -= width*bit_pixel;
		}
		posX += xi;
		posX += space + RealizeSpaceCorrect(txt+n,id);
	}
//	touchTemp.x_End= posX;
//	touchTemp.y_End= posY+height;

	if(constWidth)
		LCD_Reset_ConstWidthFonts(fontIndex);

	structTemp.inChar=lenTxt;
	structTemp.inPixel=lenTxtInPixel;
	structTemp.height=j;
	return structTemp;

}
static StructTxtPxlLen LCD_DrawStrChangeColorIndirectToBuffAndDisplay(uint32_t posBuff, int displayOn, uint32_t maxSizeX, uint32_t maxSizeY, int id, int X, int Y, char *txt, uint32_t *LcdBuffer,int OnlyDigits, int space, uint32_t NewBkColor, uint32_t NewFontColor, int constWidth)
{
	StructTxtPxlLen structTemp={0,0,0};
	int fontIndex=SearchFontIndex(FontID[id].size, FontID[id].style, FontID[id].bkColor, FontID[id].color);
	if(fontIndex==-1)
		return structTemp;
	char *fontsBuffer=Font[fontIndex].pointerToMemoryFont;
	int i,j,n,o,lenTxt,temp,lenTxtInPixel=0;
	int posX=0;
	char *pbmp;
	uint32_t index=0, width=0, height=0, bit_pixel=0;
	uint32_t backGround;
	uint32_t pos, pos2, xi;

	if(constWidth)
		LCD_Set_ConstWidthFonts(fontIndex);

	/* Get bitmap data address offset */
	index = fontsBuffer[10] + (fontsBuffer[11] << 8) + (fontsBuffer[12] << 16)  + (fontsBuffer[13] << 24);
	/* Read bitmap width */
	width = fontsBuffer[18] + (fontsBuffer[19] << 8) + (fontsBuffer[20] << 16)  + (fontsBuffer[21] << 24);  //!!! szerokosc musi byc wielokrotnoscia 4
	/* Read bitmap height */
	height = fontsBuffer[22] + (fontsBuffer[23] << 8) + (fontsBuffer[24] << 16)  + (fontsBuffer[25] << 24);
	/* Read bit/pixel */
	bit_pixel = fontsBuffer[28] + (fontsBuffer[29] << 8);
	bit_pixel/=8;

	fontsBuffer += (index + (width * height * bit_pixel));
	fontsBuffer -= width*bit_pixel;

	backGround= fontsBuffer[2]<<16 | fontsBuffer[1]<<8 | fontsBuffer[0];

	if(OnlyDigits==halfHight)
		height=Font[fontIndex].heightHalf;

	j=strlen(txt);
	for(i=0;i<j;i++)
	{
		temp = Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,id);
		if(X+lenTxtInPixel+temp <= maxSizeX)
			lenTxtInPixel += temp;
		else break;
	}
	lenTxt=i;

	LCD_RectangleBuff(LcdBuffer,posBuff,lenTxtInPixel,height,0,0,lenTxtInPixel,Y+height>maxSizeY?maxSizeY-Y:height,NewBkColor,NewBkColor,NewBkColor);
	idxChangeColorBuff=0;
   for(i=0;i<MAX_SIZE_CHANGECOLOR_BUFF;++i){
   	buffChangeColorIN[i]=0;
   	buffChangeColorOUT[i]=0;
   }

	for(n=0;n<lenTxt;++n)
	{
		pbmp=fontsBuffer+3*Font[fontIndex].fontsTabPos[ (int)txt[n] ][0];
		pos2= posBuff+posX;
		xi=Font[fontIndex].fontsTabPos[ (int)txt[n] ][1];

		for(j=0; j < height; ++j)
		{
			if(Y+j>=maxSizeY)
				break;
			pos=pos2+lenTxtInPixel*j;
			o=0;
			for(i=0; i<xi; ++i)
			{
				if ((*((uint32_t*)(pbmp+o))&0x00FFFFFF)!=backGround)
					LcdBuffer[pos+i] = GetCalculatedRGB(*(pbmp+o+2),*(pbmp+o+1),*(pbmp+o+0));
				o+=3;
			}
			pbmp -= width*bit_pixel;
		}
		posX += xi;
		posX += space + RealizeSpaceCorrect(txt+n,id);
	}

	if(constWidth)
		LCD_Reset_ConstWidthFonts(fontIndex);

	if(displayOn)
		LCD_DisplayBuff((uint32_t)X,(uint32_t)Y,(uint32_t)lenTxtInPixel,(uint32_t)j,LcdBuffer+posBuff);
	structTemp.inChar=lenTxt;
	structTemp.inPixel=lenTxtInPixel;
	structTemp.height=j;
	return structTemp;
}

static StructTxtPxlLen LCD_DrawStr(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, uint32_t *LcdBuffer, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth){
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		return LCD_DrawStrToBuff(posBuff,BkpSizeX,BkpSizeY,fontID,Xpos,Ypos,txt,LcdBuffer,OnlyDigits,space,bkColor,coeff,constWidth);
	else
		return StructTxtPxlLen_ZeroValue;
}
static StructTxtPxlLen LCD_DrawStrIndirect(uint32_t posBuff,int displayOn,uint32_t maxSizeX,uint32_t maxSizeY,int fontID, int Xpos, int Ypos, char *txt, uint32_t *LcdBuffer, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth){
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
		return LCD_DrawStrIndirectToBuffAndDisplay(posBuff,displayOn,maxSizeX,maxSizeY,fontID,Xpos,Ypos,txt,LcdBuffer,OnlyDigits,space,bkColor,coeff,constWidth);
	else
		return StructTxtPxlLen_ZeroValue;
}
static StructTxtPxlLen LCD_DrawStrChangeColor(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, uint32_t *LcdBuffer, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, uint8_t maxVal, int constWidth){
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY){
		CalculateFontCoeff(FontID[fontID].bkColor,FontID[fontID].color,bkColor,fontColor,maxVal);
		return LCD_DrawStrChangeColorToBuff(posBuff,BkpSizeX,BkpSizeY,fontID,Xpos,Ypos,txt,LcdBuffer,OnlyDigits,space,bkColor,fontColor,constWidth);
	}
	else
		return StructTxtPxlLen_ZeroValue;
}
static StructTxtPxlLen LCD_DrawStrChangeColorIndirect(uint32_t posBuff,int displayOn,uint32_t maxSizeX,uint32_t maxSizeY,int fontID, int Xpos, int Ypos, char *txt, uint32_t *LcdBuffer, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, uint8_t maxVal, int constWidth){
	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY){
		CalculateFontCoeff(FontID[fontID].bkColor,FontID[fontID].color,bkColor,fontColor,maxVal);
		return LCD_DrawStrChangeColorIndirectToBuffAndDisplay(posBuff,displayOn,maxSizeX,maxSizeY,fontID,Xpos,Ypos,txt,LcdBuffer,OnlyDigits,space,bkColor,fontColor,constWidth);
	}
	else
		return StructTxtPxlLen_ZeroValue;
}

static void LCD_StartInsertingSpacesBetweenFonts(void){
	for(int i=0;i<MAX_SPACE_CORRECT;i++){
		space[i].fontStyle=0;
		space[i].fontSize=0;
		space[i].char1=0;
		space[i].char2=0;
		space[i].val=0;
	}
	StructSpaceCount=0;
}
static uint8_t* LCD_GetPtr2SpacesBetweenFontsStruct(void){
	return &space[0].fontStyle;
}
static int LCD_GetSpacesBetweenFontsStructSize(void){
	return sizeof(space);
}
static uint8_t* LCD_GetStructSpaceCount(void){
	return &StructSpaceCount;
}
static int ReadSpacesBetweenFontsFromSDcard(void){
	if(FR_OK==SDCardFileOpen(0,"Spaces_Between_Font.bin",FA_READ)){
		SDCardFileRead(0,(char*)LCD_GetStructSpaceCount(),1);
		SDCardFileRead(0,(char*)LCD_GetPtr2SpacesBetweenFontsStruct(),LCD_GetSpacesBetweenFontsStructSize());
		SDCardFileClose(0);
		return 0;
	}
	else return 1;
}

//################################## -- Global Declarations -- #########################################################
//################################## -- Global Declarations -- #########################################################
//################################## -- Global Declarations -- #########################################################

void LCD_SetStrVar_bkColor(int idVar, uint32_t bkColor){
	FontVar[idVar].bkColor=bkColor;
}
void LCD_SetStrVar_fontColor(int idVar, uint32_t fontColor){
	FontVar[idVar].fontColor=fontColor;
}
void LCD_SetStrVar_x(int idVar,int x){
	FontVar[idVar].xPos=x;
}
void LCD_SetStrVar_y(int idVar,int y){
	FontVar[idVar].yPos=y;
}
void LCD_OffsStrVar_x(int idVar,int x){
	FontVar[idVar].xPos+=x;
}
void LCD_OffsStrVar_y(int idVar,int y){
	FontVar[idVar].yPos+=y;
}
void LCD_SetStrVar_heightType(int idVar, int OnlyDigits){
	FontVar[idVar].heightType=OnlyDigits;
}
void LCD_SetStrVar_widthType(int idVar, int constWidth){
	FontVar[idVar].widthType=constWidth;
}
void LCD_SetStrVar_coeff(int idVar, int coeff){
	FontVar[idVar].coeff=coeff;
}
void LCD_SetStrVar_space(int idVar, int space){
	FontVar[idVar].space=space;
}
void LCD_SetStrVar_fontID(int idVar, int fontID){
	FontVar[idVar].id=fontID;
}
void LCD_SetStrVar_bkScreenColor(int idVar, int bkScreenColor){
	FontVar[idVar].bkScreenColor=bkScreenColor;
}
void LCD_SetStrVar_Mov_posWin(int idVar, int posWin){
	FontVar[idVar].FontMov.posWin=posWin;
}

uint32_t LCD_GetStrVar_bkColor(int idVar){
	return FontVar[idVar].bkColor;
}
uint32_t LCD_GetStrVar_fontColor(int idVar){
	return FontVar[idVar].bkColor;
}
int LCD_GetStrVar_x(int idVar){
	return FontVar[idVar].xPos;
}
int LCD_GetStrVar_y(int idVar){
	return FontVar[idVar].yPos;
}
int LCD_GetStrVar_heightType(int idVar){
	return FontVar[idVar].heightType;
}
int LCD_GetStrVar_widthType(int idVar){
	return FontVar[idVar].widthType;
}
int LCD_GetStrVar_coeff(int idVar){
	return FontVar[idVar].coeff;
}
int LCD_GetStrVar_space(int idVar){
	return FontVar[idVar].space;
}
int LCD_GetStrVar_fontID(int idVar){
	return FontVar[idVar].id;
}
int LCD_GetStrVar_bkScreenColor(int idVar){
	return FontVar[idVar].bkScreenColor;
}
int LCD_GetStrVar_Mov_posWin(int idVar){
	return FontVar[idVar].FontMov.posWin;
}

void LCD_DeleteAllFontAndImages(void)
{
	int i;
	CounterBusyBytesForFontsImages=0;
	for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
	{
		Font[i].fontSizeToIndex=0;
		Font[i].fontStyleToIndex=0;
		Font[i].fontBkColorToIndex=0;
		Font[i].fontColorToIndex=0;
		Font[i].pointerToMemoryFont=0;
		Font[i].fontSdramLenght=0;
	}
	for(i=0; i<MAX_OPEN_FONTS_SIMULTANEOUSLY; i++)
	{
		FontID[i].size=0;
		FontID[i].style=0;
		FontID[i].bkColor=0;
		FontID[i].color=0;
	}
	for(i=0; i<MAX_OPEN_IMAGES_SIMULTANEOUSLY; i++)
	{
		Image[i].name[0]=0;
		Image[i].pointerToMemory=0;
		Image[i].sdramLenght=0;
	}
	for(i=0; i<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY; i++)
	{
		FontVar[i].id=0;
	}
}

int LCD_DeleteFont(uint32_t fontID)
{
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return 1;
	Font[fontIndex].fontSizeToIndex=0;
	Font[fontIndex].fontStyleToIndex=0;
	Font[fontIndex].fontBkColorToIndex=0;
	Font[fontIndex].fontColorToIndex=0;
	Font[fontIndex].pointerToMemoryFont=0;

	FontID[fontID].size=0;
	FontID[fontID].style=0;
	FontID[fontID].bkColor=0;
	FontID[fontID].color=0;

	if( CounterBusyBytesForFontsImages >= Font[fontIndex].fontSdramLenght )
	{
		CounterBusyBytesForFontsImages -= Font[fontIndex].fontSdramLenght;
		Font[fontIndex].fontSdramLenght=0;
		return 0;
	}
	else
	{
		CounterBusyBytesForFontsImages=0;
		Font[fontIndex].fontSdramLenght=0;
		return 2;
	}
}

char *LCD_FontSize2Str(char *buffTemp, int fontSize)
{
	switch(fontSize)
	{
	case FONT_8:   			strcpy(buffTemp,"font_8"); break;
	case FONT_8_bold:   		strcpy(buffTemp,"font_8_bold"); break;
	case FONT_8_italics: 	strcpy(buffTemp,"font_8_italics"); break;
	case FONT_9:   			strcpy(buffTemp,"font_9"); break;
	case FONT_9_bold:   		strcpy(buffTemp,"font_9_bold"); break;
	case FONT_9_italics: 	strcpy(buffTemp,"font_9_italics"); break;
	case FONT_10:   			strcpy(buffTemp,"font_10"); break;
	case FONT_10_bold:   	strcpy(buffTemp,"font_10_bold"); break;
	case FONT_10_italics: 	strcpy(buffTemp,"font_10_italics"); break;
	case FONT_11:   			strcpy(buffTemp,"font_11"); break;
	case FONT_11_bold:   	strcpy(buffTemp,"font_11_bold"); break;
	case FONT_11_italics: 	strcpy(buffTemp,"font_11_italics"); break;
	case FONT_12:   			strcpy(buffTemp,"font_12"); break;
	case FONT_12_bold:   	strcpy(buffTemp,"font_12_bold"); break;
	case FONT_12_italics: 	strcpy(buffTemp,"font_12_italics"); break;
	case FONT_14:   			strcpy(buffTemp,"font_14"); break;
	case FONT_14_bold:   	strcpy(buffTemp,"font_14_bold"); break;
	case FONT_14_italics: 	strcpy(buffTemp,"font_14_italics"); break;
	case FONT_16:   			strcpy(buffTemp,"font_16"); break;
	case FONT_16_bold:   	strcpy(buffTemp,"font_16_bold"); break;
	case FONT_16_italics: 	strcpy(buffTemp,"font_16_italics"); break;
	case FONT_18:   			strcpy(buffTemp,"font_18"); break;
	case FONT_18_bold:   	strcpy(buffTemp,"font_18_bold"); break;
	case FONT_18_italics: 	strcpy(buffTemp,"font_18_italics"); break;
	case FONT_20:   			strcpy(buffTemp,"font_20"); break;
	case FONT_20_bold:   	strcpy(buffTemp,"font_20_bold"); break;
	case FONT_20_italics: 	strcpy(buffTemp,"font_20_italics"); break;
	case FONT_22:   			strcpy(buffTemp,"font_22"); break;
	case FONT_22_bold:   	strcpy(buffTemp,"font_22_bold"); break;
	case FONT_22_italics: 	strcpy(buffTemp,"font_22_italics"); break;
	case FONT_24:   			strcpy(buffTemp,"font_24"); break;
	case FONT_24_bold:   	strcpy(buffTemp,"font_24_bold"); break;
	case FONT_24_italics: 	strcpy(buffTemp,"font_24_italics"); break;
	case FONT_26:   			strcpy(buffTemp,"font_26"); break;
	case FONT_26_bold:   	strcpy(buffTemp,"font_26_bold"); break;
	case FONT_26_italics: 	strcpy(buffTemp,"font_26_italics"); break;
	case FONT_28:   			strcpy(buffTemp,"font_28"); break;
	case FONT_28_bold:   	strcpy(buffTemp,"font_28_bold"); break;
	case FONT_28_italics: 	strcpy(buffTemp,"font_28_italics"); break;
	case FONT_36:   			strcpy(buffTemp,"font_36"); break;
	case FONT_36_bold:   	strcpy(buffTemp,"font_36_bold"); break;
	case FONT_36_italics: 	strcpy(buffTemp,"font_36_italics"); break;
	case FONT_48:   			strcpy(buffTemp,"font_48"); break;
	case FONT_48_bold:   	strcpy(buffTemp,"font_48_bold"); break;
	case FONT_48_italics: 	strcpy(buffTemp,"font_48_italics"); break;
	case FONT_72:   			strcpy(buffTemp,"font_72"); break;
	case FONT_72_bold:   	strcpy(buffTemp,"font_72_bold "); break;
	case FONT_72_italics: 	strcpy(buffTemp,"font_72_italics"); break;
	case FONT_130:   			strcpy(buffTemp,"font_130"); break;
	case FONT_130_bold:   	strcpy(buffTemp,"font_130_bold"); break;
	case FONT_130_italics: 	strcpy(buffTemp,"font_130_italics"); break;

	default:	buffTemp[0]=0; break;
	}
	return buffTemp;
}

char *LCD_FontStyle2Str(char *buffTemp, int fontStyle)
{
	switch(fontStyle)
	{
	case Arial:   			  strcpy(buffTemp,"Arial"); break;
	case Times_New_Roman:  strcpy(buffTemp,"Times_New_Roman"); break;
	case Comic_Saens_MS:   strcpy(buffTemp,"Comic_Saens_MS"); break;

	default:	buffTemp[0]=0;	break;
	}
	return buffTemp;
}

void DisplayFontsStructState(void){
	char bufTemp[50];
	DbgVar(1,250,"\r\nADDRESS:0x%x  FONTS_MEMORY_SIZE:0x%x  Counter:0x%06x\r\n",fontsImagesMemoryBuffer, MAX_FONTS_AND_IMAGES_MEMORY_SIZE, CounterBusyBytesForFontsImages);
	for(int i=0; i < MAX_OPEN_FONTS_SIMULTANEOUSLY; i++){
		if(Font[i].fontSizeToIndex)
			DbgVar(1,250,"Idx:%d  %s  %s  FontAddr:0x%06x  fontSdramLenght:%d\r\n",i, LCD_FontStyle2Str(bufTemp,Font[i].fontStyleToIndex), LCD_FontSize2Str(bufTemp+20,Font[i].fontSizeToIndex-1), Font[i].pointerToMemoryFont-fontsImagesMemoryBuffer, Font[i].fontSdramLenght);
	}
}

void InfoForImagesFonts(void){
	DbgVar(1,250,"\r\nImages SDRAM size: 0x%08x\r\nCounterBusyBytesForFontsImages: 0x%08x\r\nStruct size Font+Image: %d+%d=%d  ",MAX_FONTS_AND_IMAGES_MEMORY_SIZE,CounterBusyBytesForFontsImages,sizeof(Font),sizeof(Image), sizeof(Font)+sizeof(Image));
}

int LCD_LoadFont(int fontSize, int fontStyle, uint32_t backgroundColor, uint32_t fontColor, uint32_t fontID)
{
	int resultSearch;
	uint32_t fontFileSize;

	resultSearch=SearchFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(-1!=resultSearch)
		return LCD_GetFontID(fontSize,fontStyle,backgroundColor,fontColor);

	int fontIndex=LoadFontIndex(fontSize,fontStyle,backgroundColor,fontColor);
	if(-1==fontIndex)
		return -2;
	char fileOpenName[100]="Fonts/";

	switch(backgroundColor)
	{
	case DARKGRAY:
		strncat(fileOpenName,"BackGround_darkGray/",20);
		break;
	case BLACK:
		strncat(fileOpenName,"BackGround_black/",17);
		break;
	case BROWN:
		strncat(fileOpenName,"BackGround_brown/",17);
		break;
	case WHITE:
		strncat(fileOpenName,"BackGround_white/",17);
		break;
	default:
		break;
	}

	switch(fontColor)
	{
	case WHITE:
		strncat(fileOpenName,"Color_white/",12);
		break;
	case MYBLUE:
		strncat(fileOpenName,"Color_blue/",11);
		break;
	case MYRED:
		strncat(fileOpenName,"Color_red/",10);
		break;
	case MYGREEN:
		strncat(fileOpenName,"Color_green/",12);
		break;
	case BLACK:
		strncat(fileOpenName,"Color_black/",12);
		break;
	default:
		break;
	}

	switch(fontStyle)
	{
	case Arial_Narrow:
		strncat(fileOpenName,"Arial_Narrow/",13);
		break;
	case Calibri_Light:
		strncat(fileOpenName,"Calibri_Light/",14);
		break;
	case Bodoni_MT_Condenset:
		strncat(fileOpenName,"Bodoni_MT_Condenset/",20);
		break;
	case Comic_Saens_MS:
		strncat(fileOpenName,"Comic_Saens_MS/",15);
		break;
	case Times_New_Roman:
		strncat(fileOpenName,"Times_New_Roman/",16);
		break;
	case Arial:
		strncat(fileOpenName,"Arial/",6);
		break;
	default:
		break;
	}

	switch(fontSize)
	{
	case FONT_8:
		strncat(fileOpenName,"font_8.bmp",10);
		break;
	case FONT_9:
		strncat(fileOpenName,"font_9.bmp",10);
		break;
	case FONT_10:
		strncat(fileOpenName,"font_10.bmp",11);
		break;
	case FONT_11:
		strncat(fileOpenName,"font_11.bmp",11);
		break;
	case FONT_12:
		strncat(fileOpenName,"font_12.bmp",11);
		break;
	case FONT_14:
		strncat(fileOpenName,"font_14.bmp",11);
		break;
	case FONT_16:
		strncat(fileOpenName,"font_16.bmp",11);
		break;
	case FONT_18:
		strncat(fileOpenName,"font_18.bmp",11);
		break;
	case FONT_20:
		strncat(fileOpenName,"font_20.bmp",11);
		break;
	case FONT_22:
		strncat(fileOpenName,"font_22.bmp",11);
		break;
	case FONT_24:
		strncat(fileOpenName,"font_24.bmp",11);
		break;
	case FONT_26:
		strncat(fileOpenName,"font_26.bmp",11);
		break;
	case FONT_28:
		strncat(fileOpenName,"font_28.bmp",11);
		break;
	case FONT_36:
		strncat(fileOpenName,"font_36.bmp",11);
		break;
	case FONT_48:
		strncat(fileOpenName,"font_48.bmp",11);
		break;
	case FONT_72:
		strncat(fileOpenName,"font_72.bmp",11);
		break;
	case FONT_130:
		strncat(fileOpenName,"font_130.bmp",12);
		break;
	case FONT_8_bold:
		strncat(fileOpenName,"font_8_bold.bmp",15);
		break;
	case FONT_9_bold:
		strncat(fileOpenName,"font_9_bold.bmp",15);
		break;
	case FONT_10_bold:
		strncat(fileOpenName,"font_10_bold.bmp",16);
		break;
	case FONT_11_bold:
		strncat(fileOpenName,"font_11_bold.bmp",16);
		break;
	case FONT_12_bold:
		strncat(fileOpenName,"font_12_bold.bmp",16);
		break;
	case FONT_14_bold:
		strncat(fileOpenName,"font_14_bold.bmp",16);
		break;
	case FONT_16_bold:
		strncat(fileOpenName,"font_16_bold.bmp",16);
		break;
	case FONT_18_bold:
		strncat(fileOpenName,"font_18_bold.bmp",16);
		break;
	case FONT_20_bold:
		strncat(fileOpenName,"font_20_bold.bmp",16);
		break;
	case FONT_22_bold:
		strncat(fileOpenName,"font_22_bold.bmp",16);
		break;
	case FONT_24_bold:
		strncat(fileOpenName,"font_24_bold.bmp",16);
		break;
	case FONT_26_bold:
		strncat(fileOpenName,"font_26_bold.bmp",16);
		break;
	case FONT_28_bold:
		strncat(fileOpenName,"font_28_bold.bmp",16);
		break;
	case FONT_36_bold:
		strncat(fileOpenName,"font_36_bold.bmp",16);
		break;
	case FONT_48_bold:
		strncat(fileOpenName,"font_48_bold.bmp",16);
		break;
	case FONT_72_bold:
		strncat(fileOpenName,"font_72_bold.bmp",16);
		break;
	case FONT_130_bold:
		strncat(fileOpenName,"font_130_bold.bmp",17);
		break;
	case FONT_8_italics:
		strncat(fileOpenName,"font_8_italics.bmp",18);
		break;
	case FONT_9_italics:
		strncat(fileOpenName,"font_9_italics.bmp",18);
		break;
	case FONT_10_italics:
		strncat(fileOpenName,"font_10_italics.bmp",19);
		break;
	case FONT_11_italics:
		strncat(fileOpenName,"font_11_italics.bmp",19);
		break;
	case FONT_12_italics:
		strncat(fileOpenName,"font_12_italics.bmp",19);
		break;
	case FONT_14_italics:
		strncat(fileOpenName,"font_14_italics.bmp",19);
		break;
	case FONT_16_italics:
		strncat(fileOpenName,"font_16_italics.bmp",19);
		break;
	case FONT_18_italics:
		strncat(fileOpenName,"font_18_italics.bmp",19);
		break;
	case FONT_20_italics:
		strncat(fileOpenName,"font_20_italics.bmp",19);
		break;
	case FONT_22_italics:
		strncat(fileOpenName,"font_22_italics.bmp",19);
		break;
	case FONT_24_italics:
		strncat(fileOpenName,"font_24_italics.bmp",19);
		break;
	case FONT_26_italics:
		strncat(fileOpenName,"font_26_italics.bmp",19);
		break;
	case FONT_28_italics:
		strncat(fileOpenName,"font_28_italics.bmp",19);
		break;
	case FONT_36_italics:
		strncat(fileOpenName,"font_36_italics.bmp",19);
		break;
	case FONT_48_italics:
		strncat(fileOpenName,"font_48_italics.bmp",19);
		break;
	case FONT_72_italics:
		strncat(fileOpenName,"font_72_italics.bmp",19);
		break;
	case FONT_130_italics:
		strncat(fileOpenName,"font_130_italics.bmp",20);
		break;
	default:
		break;
	}

	if(FR_OK!=SDCardFileInfo(fileOpenName,&fontFileSize))
		return -3;

	while((fontFileSize%4)!=0)
		fontFileSize++;

	if(true==DynamicFontMemoryAllocation( fontFileSize, fontIndex) )
	{
		if(FR_OK!=SDCardFileOpen(1,fileOpenName,FA_READ))
			return -4;
		if(0 > SDCardFileRead(1,Font[fontIndex].pointerToMemoryFont,MAX_FONTS_AND_IMAGES_MEMORY_SIZE))
			return -5;
		if(FR_OK!=SDCardFileClose(1))
			return -6;
		SearchCurrentFont_TablePos(Font[fontIndex].pointerToMemoryFont, fontIndex);
	}
	else
		return -7;

	if(fontID < MAX_OPEN_FONTS_SIMULTANEOUSLY)
	{
		FontID[fontID].size = fontSize;
		FontID[fontID].style = fontStyle;
		FontID[fontID].bkColor = backgroundColor;
		FontID[fontID].color = fontColor;
		return fontID;
	}
	else
		return -8;
}

int LCD_LoadFont_WhiteBlack(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont(fontSize,fontStyle,WHITE,BLACK,fontID);
}
int LCD_LoadFont_DarkgrayGreen(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont(fontSize,fontStyle,DARKGRAY,MYGREEN,fontID);
}
int LCD_LoadFont_DarkgrayRed(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont(fontSize,fontStyle,DARKGRAY,MYRED,fontID);
}
int LCD_LoadFont_DarkgrayBlue(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont(fontSize,fontStyle,DARKGRAY,MYBLUE,fontID);
}
int LCD_LoadFont_BrownWhite(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont(fontSize,fontStyle,BROWN,WHITE,fontID);
}
int LCD_LoadFont_DarkgrayWhite(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont(fontSize,fontStyle,DARKGRAY,WHITE,fontID);
}
int LCD_LoadFont_ChangeColor(int fontSize, int fontStyle, uint32_t fontID){
	return LCD_LoadFont_DarkgrayGreen(fontSize,fontStyle,fontID);
}

StructTxtPxlLen LCD_Str(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth){
	return LCD_DrawStr(0,LCD_GetXSize(),LCD_GetYSize(),fontID,Xpos,Ypos,txt, pLcd,OnlyDigits,space,bkColor,coeff,constWidth);
}
StructTxtPxlLen LCD_StrWindow(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth){
	return LCD_DrawStr(posBuff,BkpSizeX,BkpSizeY,fontID, Xpos,Ypos, txt, pLcd,OnlyDigits, space,bkColor,coeff,constWidth);
}
StructTxtPxlLen LCD_StrIndirect(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth){
	return LCD_DrawStrIndirect(0,1,LCD_GetXSize(),LCD_GetYSize(),fontID, Xpos,Ypos, txt, pLcd,OnlyDigits, space,bkColor,coeff,constWidth);
}
StructTxtPxlLen LCD_StrWindowIndirect(uint32_t posBuff, int Xwin, int Ywin, uint32_t BkpSizeX, uint32_t BkpSizeY, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space,uint32_t bkColor, int coeff, int constWidth){
	StructTxtPxlLen out= LCD_DrawStr(posBuff,BkpSizeX,BkpSizeY,fontID,Xpos,Ypos,txt,pLcd,OnlyDigits,space,bkColor,coeff,constWidth);
	if(out.inChar>0) LCD_DisplayBuff((uint32_t)Xwin,(uint32_t)Ywin,BkpSizeX,BkpSizeY,pLcd+posBuff);
	return out;
}
StructTxtPxlLen LCD_StrChangeColor(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth){
	return LCD_DrawStrChangeColor(0,LCD_GetXSize(),LCD_GetYSize(),fontID,Xpos,Ypos,txt,pLcd,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
}
StructTxtPxlLen LCD_StrChangeColorWindow(uint32_t posBuff,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth){
	return LCD_DrawStrChangeColor(posBuff,BkpSizeX,BkpSizeY,fontID,Xpos,Ypos,txt,pLcd,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
}
StructTxtPxlLen LCD_StrChangeColorIndirect(int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth){
	return LCD_DrawStrChangeColorIndirect(0,1,LCD_GetXSize(),LCD_GetYSize(),fontID,Xpos,Ypos,txt,pLcd,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
}
StructTxtPxlLen LCD_StrChangeColorWindowIndirect(uint32_t posBuff, int Xwin, int Ywin,uint32_t BkpSizeX,uint32_t BkpSizeY,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth){
	StructTxtPxlLen out= LCD_DrawStrChangeColor(posBuff,BkpSizeX,BkpSizeY,fontID,Xpos,Ypos,txt,pLcd,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
	if(out.inChar>0) LCD_DisplayBuff((uint32_t)Xwin,(uint32_t)Ywin,BkpSizeX,BkpSizeY,pLcd+posBuff);
	return out;
}
StructTxtPxlLen LCD_StrVar(int idVar,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor){
	StructTxtPxlLen temp;
	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
	{
		temp = LCD_Str(fontID,Xpos,Ypos,txt,OnlyDigits,space,bkColor,coeff,constWidth);
		if((temp.height==0)&&(temp.inChar==0)&&(temp.inPixel==0))
			return temp;
		FontVar[idVar].id=fontID;
		FontVar[idVar].xPos=Xpos;
		FontVar[idVar].yPos=Ypos;
		FontVar[idVar].heightType=OnlyDigits;
		FontVar[idVar].space=space;
		FontVar[idVar].bkColor=bkColor;
		FontVar[idVar].bkScreenColor=bkScreenColor;
		FontVar[idVar].coeff=coeff;
		FontVar[idVar].widthType=constWidth;
		FontVar[idVar].xPos_prev = FontVar[idVar].xPos;
		FontVar[idVar].yPos_prev = FontVar[idVar].yPos;
		FontVar[idVar].widthPxl_prev = temp.inPixel;
		FontVar[idVar].heightPxl_prev = temp.height;
		return temp;
	}
	else return StructTxtPxlLen_ZeroValue;
}
StructTxtPxlLen LCD_StrDescrVar(int idVar,int fontID, int Xpos, int Ypos, char *txtDescr, char *txtVar, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor){
	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
		return LCD_StrVar(idVar,fontID, Xpos+LCD_Str(fontID,Xpos,Ypos,txtDescr,OnlyDigits,space,bkColor,coeff,0).inPixel, Ypos,txtVar,OnlyDigits,space,bkColor,coeff,constWidth,bkScreenColor);
	else return StructTxtPxlLen_ZeroValue;
}

static void LCD_DimensionBkCorrect(int idVar, StructTxtPxlLen temp, uint32_t *LcdBuffer)
{
	int xEnd_prev = FontVar[idVar].xPos_prev + FontVar[idVar].widthPxl_prev;
	int xEnd		  = FontVar[idVar].xPos 	  + temp.inPixel;
	int yEnd_prev = FontVar[idVar].yPos_prev + FontVar[idVar].heightPxl_prev;
	int yEnd		  = FontVar[idVar].yPos 	  + temp.height;

	if(FontVar[idVar].xPos > FontVar[idVar].xPos_prev){
		int width=FontVar[idVar].xPos-FontVar[idVar].xPos_prev;
		LCD_RectangleBuff(LcdBuffer,0,width,FontVar[idVar].heightPxl_prev,0,0,width,FontVar[idVar].heightPxl_prev,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor);
		LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos_prev,(uint32_t)FontVar[idVar].yPos_prev,width,FontVar[idVar].heightPxl_prev,pLcd+0);
	}
	if(xEnd_prev > xEnd){
		int width=xEnd_prev-xEnd;
		LCD_RectangleBuff(LcdBuffer,0,width,FontVar[idVar].heightPxl_prev,0,0,width,FontVar[idVar].heightPxl_prev,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor);
		LCD_DisplayBuff((uint32_t)xEnd,(uint32_t)FontVar[idVar].yPos_prev,width,FontVar[idVar].heightPxl_prev,pLcd+0);
	}
	if(FontVar[idVar].yPos > FontVar[idVar].yPos_prev){
		int height=FontVar[idVar].yPos-FontVar[idVar].yPos_prev;
		LCD_RectangleBuff(LcdBuffer,0,FontVar[idVar].widthPxl_prev,height,0,0,FontVar[idVar].widthPxl_prev,height,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor);
		LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos_prev,(uint32_t)FontVar[idVar].yPos_prev,FontVar[idVar].widthPxl_prev,height,pLcd+0);
	}
	if(yEnd_prev > yEnd){
		int height=yEnd_prev-yEnd;
		LCD_RectangleBuff(LcdBuffer,0,FontVar[idVar].widthPxl_prev,height,0,0,FontVar[idVar].widthPxl_prev,height,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor,FontVar[idVar].bkScreenColor);
		LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos_prev,yEnd,FontVar[idVar].widthPxl_prev,height,pLcd+0);
	}
	FontVar[idVar].xPos_prev=FontVar[idVar].xPos;
	FontVar[idVar].yPos_prev=FontVar[idVar].yPos;
	FontVar[idVar].widthPxl_prev=temp.inPixel;
	FontVar[idVar].heightPxl_prev=temp.height;
}

StructTxtPxlLen LCD_StrVarIndirect(int idVar, char *txt){
	StructTxtPxlLen temp;
	temp = LCD_StrIndirect(FontVar[idVar].id,FontVar[idVar].xPos,FontVar[idVar].yPos,txt,FontVar[idVar].heightType,FontVar[idVar].space,FontVar[idVar].bkColor,FontVar[idVar].coeff,FontVar[idVar].widthType);
	if((temp.height==0)&&(temp.inChar==0)&&(temp.inPixel==0))
		return temp;
	LCD_DimensionBkCorrect(idVar,temp,pLcd);
	return temp;
}

StructTxtPxlLen LCD_StrChangeColorVar(int idVar,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor,uint8_t maxVal, int constWidth, uint32_t bkScreenColor){
	StructTxtPxlLen temp;
	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
	{
		temp = LCD_StrChangeColor(fontID,Xpos,Ypos,txt,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
		if((temp.height==0)&&(temp.inChar==0)&&(temp.inPixel==0))
			return temp;
		FontVar[idVar].id=fontID;
		FontVar[idVar].xPos=Xpos;
		FontVar[idVar].yPos=Ypos;
		FontVar[idVar].heightType=OnlyDigits;
		FontVar[idVar].space=space;
		FontVar[idVar].bkColor=bkColor;
		FontVar[idVar].bkScreenColor=bkScreenColor;
		FontVar[idVar].fontColor=fontColor;
		FontVar[idVar].coeff=maxVal;
		FontVar[idVar].widthType=constWidth;
		FontVar[idVar].xPos_prev = FontVar[idVar].xPos;
		FontVar[idVar].yPos_prev = FontVar[idVar].yPos;
		FontVar[idVar].widthPxl_prev = temp.inPixel;
		FontVar[idVar].heightPxl_prev = temp.height;
		return temp;
	}
	else return StructTxtPxlLen_ZeroValue;
}
StructTxtPxlLen LCD_StrChangeColorVarIndirect(int idVar, char *txt){
	StructTxtPxlLen temp;
	temp = LCD_StrChangeColorIndirect(FontVar[idVar].id,FontVar[idVar].xPos,FontVar[idVar].yPos,txt,FontVar[idVar].heightType,FontVar[idVar].space,FontVar[idVar].bkColor,FontVar[idVar].fontColor,FontVar[idVar].coeff,FontVar[idVar].widthType);
	if((temp.height==0)&&(temp.inChar==0)&&(temp.inPixel==0))
		return temp;
	LCD_DimensionBkCorrect(idVar,temp,pLcd);
	return temp;
}

void LCD_ResetStrMovBuffPos(void){
	movableFontsBuffer_pos=0;
}
void LCD_DisplayStrMovBuffState(void){
	DbgVar(1,250,"\r\nMovBuff -> MaxSize: %d   LoadedSize: %d   ",LCD_MOVABLE_FONTS_BUFF_SIZE, movableFontsBuffer_pos);
}

extern void SwapUint16(uint16_t *a, uint16_t *b);

StructTxtPxlLen LCD_StrRot(int rot, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth){
	StructTxtPxlLen temp;
	int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
	if(fontHeight<0)
		return StructTxtPxlLen_ZeroValue;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	temp=LCD_DrawStr(movableFontsBuffer_pos, pxlTxtLen,fontHeight, fontID,0,0,txt,movableFontsBuffer,OnlyDigits,space,bkColor,coeff,constWidth);
	LCD_CopyBuff2pLcd(rot,movableFontsBuffer_pos, movableFontsBuffer, pxlTxtLen,fontHeight,0, temp.inPixel, Xpos,Ypos,0);
	if(rot>Rotate_0)
		SwapUint16(&temp.inPixel,&temp.height);
	return temp;
}
StructTxtPxlLen LCD_StrChangeColorRot(int rot, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth){
	StructTxtPxlLen temp;
	int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
	if(fontHeight<0)
		return StructTxtPxlLen_ZeroValue;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, pxlTxtLen,fontHeight, fontID,0,0,txt,movableFontsBuffer,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
	LCD_CopyBuff2pLcd(rot,movableFontsBuffer_pos, movableFontsBuffer, pxlTxtLen,fontHeight,0, temp.inPixel, Xpos,Ypos,0);
	if(rot>Rotate_0)
		SwapUint16(&temp.inPixel,&temp.height);
	return temp;
}
StructTxtPxlLen LCD_StrRotVar(int idVar, int rot ,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor){
	StructTxtPxlLen temp;
	int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
	if(fontHeight<0)
		return StructTxtPxlLen_ZeroValue;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	FontVar[idVar].id=fontID;
	FontVar[idVar].xPos=Xpos;
	FontVar[idVar].yPos=Ypos;
	FontVar[idVar].heightType=OnlyDigits;
	FontVar[idVar].space=space;
	FontVar[idVar].bkColor=bkColor;
	FontVar[idVar].bkScreenColor=bkScreenColor;
	FontVar[idVar].coeff=coeff;
	FontVar[idVar].widthType=constWidth;
	FontVar[idVar].rotate=rot;

	temp=LCD_DrawStr(movableFontsBuffer_pos, pxlTxtLen,fontHeight, fontID,0,0,txt,movableFontsBuffer,OnlyDigits,space,bkColor,coeff,constWidth);
	LCD_CopyBuff2pLcd(rot,movableFontsBuffer_pos, movableFontsBuffer, pxlTxtLen,fontHeight,0, temp.inPixel, Xpos,Ypos,0);

	if(rot>Rotate_0)
		SwapUint16(&temp.inPixel,&temp.height);

	FontVar[idVar].xPos_prev = FontVar[idVar].xPos;
	FontVar[idVar].yPos_prev = FontVar[idVar].yPos;
	FontVar[idVar].widthPxl_prev = temp.inPixel;
	FontVar[idVar].heightPxl_prev = temp.height;

	return temp;
}

StructTxtPxlLen LCD_StrChangeColorRotVar(int idVar, int rot ,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth, uint32_t bkScreenColor){
	StructTxtPxlLen temp;
	int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
	if(fontHeight<0)
		return StructTxtPxlLen_ZeroValue;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	FontVar[idVar].id=fontID;
	FontVar[idVar].xPos=Xpos;
	FontVar[idVar].yPos=Ypos;
	FontVar[idVar].heightType=OnlyDigits;
	FontVar[idVar].space=space;
	FontVar[idVar].bkColor=bkColor;
	FontVar[idVar].fontColor=fontColor;
	FontVar[idVar].bkScreenColor=bkScreenColor;
	FontVar[idVar].coeff=maxVal;
	FontVar[idVar].widthType=constWidth;
	FontVar[idVar].rotate=rot;

	temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, pxlTxtLen,fontHeight, fontID,0,0,txt,movableFontsBuffer,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
	LCD_CopyBuff2pLcd(rot,movableFontsBuffer_pos, movableFontsBuffer, pxlTxtLen,fontHeight,0, temp.inPixel, Xpos,Ypos,0);

	if(rot>Rotate_0)
		SwapUint16(&temp.inPixel,&temp.height);

	FontVar[idVar].xPos_prev = FontVar[idVar].xPos;
	FontVar[idVar].yPos_prev = FontVar[idVar].yPos;
	FontVar[idVar].widthPxl_prev = temp.inPixel;
	FontVar[idVar].heightPxl_prev = temp.height;

	return temp;
}
StructTxtPxlLen LCD_StrRotVarIndirect(int idVar, char *txt){
	StructTxtPxlLen temp;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(FontVar[idVar].id,txt,FontVar[idVar].space,FontVar[idVar].widthType);
	if(pxlTxtLen<0)
		return StructTxtPxlLen_ZeroValue;
	int fontHeight= FontVar[idVar].heightType==fullHight?LCD_GetFontHeight(FontVar[idVar].id):LCD_GetFontHalfHeight(FontVar[idVar].id);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	temp=LCD_DrawStr(movableFontsBuffer_pos, pxlTxtLen,fontHeight, FontVar[idVar].id,0,0,txt,movableFontsBuffer,FontVar[idVar].heightType,FontVar[idVar].space,FontVar[idVar].bkColor,FontVar[idVar].coeff,FontVar[idVar].widthType);
	LCD_CopyBuff2pLcdIndirect(FontVar[idVar].rotate,movableFontsBuffer_pos, movableFontsBuffer, pxlTxtLen,fontHeight,0, temp.inPixel,0);
	switch(FontVar[idVar].rotate)
	{	case Rotate_0:
			LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos,(uint32_t)FontVar[idVar].yPos, pxlTxtLen,fontHeight, pLcd);
			break;
		case Rotate_90:
		case Rotate_180:
		default:
			LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos,(uint32_t)FontVar[idVar].yPos, fontHeight,pxlTxtLen, pLcd);
			break;
	}
	if(FontVar[idVar].rotate>Rotate_0)
		SwapUint16(&temp.inPixel,&temp.height);
	LCD_DimensionBkCorrect(idVar,temp,pLcd);
	return temp;
}
StructTxtPxlLen LCD_StrChangeColorRotVarIndirect(int idVar, char *txt){
	StructTxtPxlLen temp;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(FontVar[idVar].id,txt,FontVar[idVar].space,FontVar[idVar].widthType);
	if(pxlTxtLen<0)
		return StructTxtPxlLen_ZeroValue;
	int fontHeight= FontVar[idVar].heightType==fullHight?LCD_GetFontHeight(FontVar[idVar].id):LCD_GetFontHalfHeight(FontVar[idVar].id);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, pxlTxtLen,fontHeight, FontVar[idVar].id,0,0,txt,movableFontsBuffer,FontVar[idVar].heightType,FontVar[idVar].space,FontVar[idVar].bkColor,FontVar[idVar].fontColor,FontVar[idVar].coeff,FontVar[idVar].widthType);
	LCD_CopyBuff2pLcdIndirect(FontVar[idVar].rotate,movableFontsBuffer_pos, movableFontsBuffer, pxlTxtLen,fontHeight,0, temp.inPixel,0);
	switch(FontVar[idVar].rotate)
	{	case Rotate_0:
			LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos,(uint32_t)FontVar[idVar].yPos, pxlTxtLen,fontHeight, pLcd);
			break;
		case Rotate_90:
		case Rotate_180:
		default:
			LCD_DisplayBuff((uint32_t)FontVar[idVar].xPos,(uint32_t)FontVar[idVar].yPos, fontHeight,pxlTxtLen, pLcd);
			break;
	}
	if(FontVar[idVar].rotate>Rotate_0)
		SwapUint16(&temp.inPixel,&temp.height);
	LCD_DimensionBkCorrect(idVar,temp,pLcd);
	return temp;
}

StructTxtPxlLen LCD_StrRotWin(int rot, int winWidth, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth, uint32_t bkScreenColor)
{
	StructTxtPxlLen temp;
	int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
	if(fontHeight<0)
		return StructTxtPxlLen_ZeroValue;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight*pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	int i=0,it=0,iH=0,j;
	int movableFontsBuffer_pos_copy=movableFontsBuffer_pos;
	char bufTxt[winWidth/LCD_GetFontWidth(fontID,'1')];

		do
		{	if(iH>0){ if(txt[it]==' ') it++; }
			i= LCD_GetStrLenForPxlWidth(fontID,&txt[it],winWidth,space,constWidth);
			if(i==0) break;
			for(j=0;j<i;++j)
			{	if(j==sizeof(bufTxt)-1)
					break;
				bufTxt[j]=txt[it+j];
			}
			bufTxt[j]=0;
			it+=i;
			temp=LCD_DrawStr(movableFontsBuffer_pos, winWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,bkColor,coeff,constWidth);

			int diffWidth=winWidth-temp.inPixel;
			if(diffWidth>0)
				LCD_RectangleBuff(movableFontsBuffer,movableFontsBuffer_pos, winWidth,fontHeight, temp.inPixel,0, diffWidth,fontHeight, bkColor,bkColor,bkColor);

			movableFontsBuffer_pos += fontHeight*winWidth;
			iH++;
		}while(movableFontsBuffer_pos + fontHeight*winWidth < LCD_MOVABLE_FONTS_BUFF_SIZE);

		int winHeight = fontHeight*iH;
		LCD_CopyBuff2pLcd(rot,movableFontsBuffer_pos_copy, movableFontsBuffer, winWidth,winHeight, 0, winWidth, Xpos,Ypos,0);

		temp.inChar=0;
		temp.inPixel=winWidth;
		temp.height=winHeight;
		if(rot>Rotate_0)
			SwapUint16(&temp.inPixel,&temp.height);
		return temp;
}

StructTxtPxlLen LCD_StrChangeColorRotWin(int rot, int winWidth, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth, uint32_t bkScreenColor)
{
	StructTxtPxlLen temp;
	int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
	if(fontHeight<0)
		return StructTxtPxlLen_ZeroValue;
	int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

	int posMovBuff_copy = movableFontsBuffer_pos + fontHeight*pxlTxtLen;
	if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
		return StructTxtPxlLen_ZeroValue;

	int i=0,it=0,iH=0,j;
	int movableFontsBuffer_pos_copy=movableFontsBuffer_pos;
	char bufTxt[winWidth/LCD_GetFontWidth(fontID,'1')];

		do
		{	if(iH>0){ if(txt[it]==' ') it++; }
			i= LCD_GetStrLenForPxlWidth(fontID,&txt[it],winWidth,space,constWidth);
			if(i==0) break;
			for(j=0;j<i;++j)
			{	if(j==sizeof(bufTxt)-1)
					break;
				bufTxt[j]=txt[it+j];
			}
			bufTxt[j]=0;
			it+=i;
			if(iH==2)
				temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, winWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,DARKBLUE,fontColor,maxVal,constWidth);
			else
				temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, winWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);

			int diffWidth=winWidth-temp.inPixel;
			if(diffWidth>0)
				LCD_RectangleBuff(movableFontsBuffer,movableFontsBuffer_pos, winWidth,fontHeight, temp.inPixel,0, diffWidth,fontHeight, bkColor,bkColor,bkColor);

			movableFontsBuffer_pos += fontHeight*winWidth;
			iH++;
		}while(movableFontsBuffer_pos + fontHeight*winWidth < LCD_MOVABLE_FONTS_BUFF_SIZE);

		int winHeight = fontHeight*iH;
		LCD_CopyBuff2pLcd(rot,movableFontsBuffer_pos_copy, movableFontsBuffer, winWidth,winHeight, 0, winWidth, Xpos,Ypos,0);

		temp.inChar=0;
		temp.inPixel=winWidth;
		temp.height=winHeight;
		if(rot>Rotate_0)
			SwapUint16(&temp.inPixel,&temp.height);
		return temp;
}

StructTxtPxlLen LCD_StrMovH(int idVar, int rot, int posWin, int winWidth ,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth)
{
	StructTxtPxlLen temp;
	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
	{
		int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
		if(fontHeight<0)
			return StructTxtPxlLen_ZeroValue;
		int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

		int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
		if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
			return StructTxtPxlLen_ZeroValue;

		FontVar[idVar].id=fontID;
		FontVar[idVar].xPos=Xpos;
		FontVar[idVar].yPos=Ypos;
		FontVar[idVar].heightType=OnlyDigits;
		FontVar[idVar].space=space;
		FontVar[idVar].bkColor=bkColor;
		FontVar[idVar].coeff=coeff;
		FontVar[idVar].widthType=constWidth;
		FontVar[idVar].rotate=rot;

		FontVar[idVar].FontMov.xImgWidth=pxlTxtLen;
		FontVar[idVar].FontMov.yImgHeight=fontHeight;
		FontVar[idVar].FontMov.posWin=posWin;
		FontVar[idVar].FontMov.windowWidth=winWidth;
		FontVar[idVar].FontMov.spaceEndStart=winWidth/3;
		FontVar[idVar].FontMov.posBuff=movableFontsBuffer_pos;
		movableFontsBuffer_pos = posMovBuff_copy;

		temp=LCD_DrawStr(FontVar[idVar].FontMov.posBuff, FontVar[idVar].FontMov.xImgWidth, FontVar[idVar].FontMov.yImgHeight, fontID,0,0,txt,movableFontsBuffer,OnlyDigits,space,bkColor,coeff,constWidth);
		int diffWidth=winWidth-temp.inPixel;
		LCD_CopyBuff2pLcd(rot,FontVar[idVar].FontMov.posBuff, movableFontsBuffer, FontVar[idVar].FontMov.xImgWidth, FontVar[idVar].FontMov.yImgHeight, posWin, diffWidth>0?temp.inPixel:winWidth, Xpos,Ypos,diffWidth>0?diffWidth:0);

		FontVar[idVar].FontMov.pxlTxtLen=temp.inPixel;
		if(diffWidth>0)
		{	switch(rot)
			{
			case Rotate_0:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos+temp.inPixel,Ypos, diffWidth, FontVar[idVar].FontMov.yImgHeight, bkColor,bkColor,bkColor);
				break;
			case Rotate_90:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos+temp.inPixel, FontVar[idVar].FontMov.yImgHeight,diffWidth, bkColor,bkColor,bkColor);
				break;
			case Rotate_180:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos, FontVar[idVar].FontMov.yImgHeight,diffWidth, bkColor,bkColor,bkColor);
				break;
			}
		}
		FontVar[idVar].xPos_prev = Xpos;
		FontVar[idVar].yPos_prev = Ypos;
		FontVar[idVar].widthPxl_prev = winWidth;
		FontVar[idVar].heightPxl_prev = FontVar[idVar].FontMov.yImgHeight;

		temp.inChar=0;
		temp.inPixel=winWidth;
		if(rot>Rotate_0)
			SwapUint16(&temp.inPixel,&temp.height);
		return temp;
	}
	else return StructTxtPxlLen_ZeroValue;
}

StructTxtPxlLen LCD_StrChangeColorMovH(int idVar, int rot, int posWin, int winWidth ,int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth)
{
	StructTxtPxlLen temp;
	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
	{
		int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
		if(fontHeight<0)
			return StructTxtPxlLen_ZeroValue;
		int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

		int posMovBuff_copy = movableFontsBuffer_pos + fontHeight * pxlTxtLen;
		if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
			return StructTxtPxlLen_ZeroValue;

		FontVar[idVar].id=fontID;
		FontVar[idVar].xPos=Xpos;
		FontVar[idVar].yPos=Ypos;
		FontVar[idVar].heightType=OnlyDigits;
		FontVar[idVar].space=space;
		FontVar[idVar].bkColor=bkColor;
		FontVar[idVar].coeff=maxVal;
		FontVar[idVar].widthType=constWidth;
		FontVar[idVar].rotate=rot;

		FontVar[idVar].FontMov.xImgWidth=pxlTxtLen;
		FontVar[idVar].FontMov.yImgHeight=fontHeight;
		FontVar[idVar].FontMov.posWin=posWin;
		FontVar[idVar].FontMov.windowWidth=winWidth;
		FontVar[idVar].FontMov.spaceEndStart=winWidth/3;
		FontVar[idVar].FontMov.posBuff=movableFontsBuffer_pos;
		movableFontsBuffer_pos = posMovBuff_copy;

		temp=LCD_DrawStrChangeColor(FontVar[idVar].FontMov.posBuff, FontVar[idVar].FontMov.xImgWidth, FontVar[idVar].FontMov.yImgHeight, fontID,0,0,txt,movableFontsBuffer,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
		int diffWidth=winWidth-temp.inPixel;
		LCD_CopyBuff2pLcd(rot,FontVar[idVar].FontMov.posBuff, movableFontsBuffer, FontVar[idVar].FontMov.xImgWidth, FontVar[idVar].FontMov.yImgHeight, posWin, diffWidth>0?temp.inPixel:winWidth, Xpos,Ypos,diffWidth>0?diffWidth:0);

		FontVar[idVar].FontMov.pxlTxtLen=temp.inPixel;
		if(diffWidth>0)
		{	switch(rot)
			{
			case Rotate_0:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos+temp.inPixel,Ypos, diffWidth, FontVar[idVar].FontMov.yImgHeight, bkColor,bkColor,bkColor);
				break;
			case Rotate_90:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos+temp.inPixel, FontVar[idVar].FontMov.yImgHeight,diffWidth, bkColor,bkColor,bkColor);
				break;
			case Rotate_180:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos, FontVar[idVar].FontMov.yImgHeight,diffWidth, bkColor,bkColor,bkColor);
				break;
			}
		}
		FontVar[idVar].xPos_prev = Xpos;
		FontVar[idVar].yPos_prev = Ypos;
		FontVar[idVar].widthPxl_prev = winWidth;
		FontVar[idVar].heightPxl_prev = FontVar[idVar].FontMov.yImgHeight;

		temp.inChar=0;
		temp.inPixel=winWidth;
		if(rot>Rotate_0)
			SwapUint16(&temp.inPixel,&temp.height);
		return temp;
	}
	else return StructTxtPxlLen_ZeroValue;
}

int LCD_StrMovHIndirect(int idVar, int incrDecr)
{
	#define M	FontVar[idVar].FontMov
	#define F	FontVar[idVar]

	if(SearchFontIndex(FontID[F.id].size, FontID[F.id].style, FontID[F.id].bkColor, FontID[F.id].color)<0)
		return -1;

	if(M.windowWidth>M.pxlTxtLen) return 0;

	if(M.posWin+incrDecr>=0)
		M.posWin += incrDecr;

	if(M.posWin + M.windowWidth <= M.pxlTxtLen)
	{	LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth, M.yImgHeight, M.posWin, M.windowWidth,0);
		switch(F.rotate)
		{	case Rotate_0:
				LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowWidth,M.yImgHeight, pLcd);
				break;
			case Rotate_90:
			case Rotate_180:
			default:
				LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.yImgHeight,M.windowWidth, pLcd);
				break;
		}
	}
	else
	{	int windowWidth_new = M.pxlTxtLen - M.posWin;
		int windowWidth_empty = M.posWin + M.windowWidth - M.pxlTxtLen;

		if(windowWidth_new>0)
		{	LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth, M.yImgHeight, M.posWin, windowWidth_new,0);
			switch(F.rotate)
			{	case Rotate_0:
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, windowWidth_new, M.yImgHeight, pLcd);
					break;
				case Rotate_90:
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.yImgHeight,windowWidth_new, pLcd);
					break;
				case Rotate_180:
				default:
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos+windowWidth_empty, M.yImgHeight,windowWidth_new, pLcd);
					break;
			}

			if(windowWidth_empty>M.spaceEndStart)
			{	int windowWidth_next = M.windowWidth - (windowWidth_new + M.spaceEndStart);

				switch(F.rotate)
				{	case Rotate_0:
						LCD_RectangleBuff(pLcd,0,M.spaceEndStart,M.yImgHeight,0,0,M.spaceEndStart,M.yImgHeight,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos+windowWidth_new, (uint32_t)F.yPos, M.spaceEndStart, M.yImgHeight, pLcd);
						break;
					case Rotate_90:
						LCD_RectangleBuff(pLcd,0,M.yImgHeight,M.spaceEndStart,0,0,M.yImgHeight,M.spaceEndStart,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos+windowWidth_new, M.yImgHeight, M.spaceEndStart, pLcd);
						break;
					case Rotate_180:
					default:
						LCD_RectangleBuff(pLcd,0,M.yImgHeight,M.spaceEndStart,0,0,M.yImgHeight,M.spaceEndStart,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos+windowWidth_next, M.yImgHeight, M.spaceEndStart, pLcd);
						break;
				}

				LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth, M.yImgHeight, 0, windowWidth_next,0);
				switch(F.rotate)
				{	case Rotate_0:
						LCD_DisplayBuff((uint32_t)F.xPos+windowWidth_new+M.spaceEndStart, (uint32_t)F.yPos, windowWidth_next, M.yImgHeight, pLcd);
						break;
					case Rotate_90:
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos+windowWidth_new+M.spaceEndStart, M.yImgHeight, windowWidth_next, pLcd);
						break;
					case Rotate_180:
					default:
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, M.yImgHeight, windowWidth_next, pLcd);
						break;
				}
			}
			else{
				switch(F.rotate)
				{	case Rotate_0:
						LCD_RectangleBuff(pLcd,0,windowWidth_empty,M.yImgHeight,0,0,windowWidth_empty,M.yImgHeight,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos+windowWidth_new, (uint32_t)F.yPos, windowWidth_empty, M.yImgHeight, pLcd);
						break;
					case Rotate_90:
						LCD_RectangleBuff(pLcd,0,M.yImgHeight,windowWidth_empty,0,0,M.yImgHeight,windowWidth_empty,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos+windowWidth_new, M.yImgHeight, windowWidth_empty, pLcd);
						break;
					case Rotate_180:
					default:
						LCD_RectangleBuff(pLcd,0,M.yImgHeight,windowWidth_empty,0,0,M.yImgHeight,windowWidth_empty,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, M.yImgHeight, windowWidth_empty, pLcd);
						break;
				}
			}
		}
		else
		{	int spaceEndStart_new = M.spaceEndStart + windowWidth_new;

			if(spaceEndStart_new>0)
			{	int windowWidth_next = M.windowWidth - spaceEndStart_new;

				switch(F.rotate)
				{	case Rotate_0:
						LCD_RectangleBuff(pLcd,0,spaceEndStart_new,M.yImgHeight,0,0,spaceEndStart_new,M.yImgHeight,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, spaceEndStart_new, M.yImgHeight, pLcd);
						break;
					case Rotate_90:
						LCD_RectangleBuff(pLcd,0,M.yImgHeight,spaceEndStart_new,0,0,M.yImgHeight,spaceEndStart_new,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, M.yImgHeight,spaceEndStart_new, pLcd);
						break;
					case Rotate_180:
					default:
						LCD_RectangleBuff(pLcd,0,M.yImgHeight,spaceEndStart_new,0,0,M.yImgHeight,spaceEndStart_new,F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos+windowWidth_next, M.yImgHeight,spaceEndStart_new, pLcd);
						break;
				}

				LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth, M.yImgHeight, 0, windowWidth_next,0);
				switch(F.rotate)
				{	case Rotate_0:
						LCD_DisplayBuff((uint32_t)F.xPos+spaceEndStart_new, (uint32_t)F.yPos, windowWidth_next, M.yImgHeight, pLcd);
						break;
					case Rotate_90:
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos+spaceEndStart_new, M.yImgHeight, windowWidth_next, pLcd);
						break;
					case Rotate_180:
					default:
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, M.yImgHeight, windowWidth_next, pLcd);
						break;
				}
			}
			else{
				M.posWin=0;
				LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth, M.yImgHeight, M.posWin, M.windowWidth,0);
				switch(F.rotate)
				{	case Rotate_0:
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, M.windowWidth, M.yImgHeight, pLcd);
						break;
					case Rotate_90:
					case Rotate_180:
					default:
						LCD_DisplayBuff((uint32_t)F.xPos, (uint32_t)F.yPos, M.yImgHeight, M.windowWidth, pLcd);
						break;
				}
				return 1;
			}
		}
	}
	return 0;
}

StructTxtPxlLen LCD_StrMovV(int idVar, int rot, int posWin, int winWidth,int winHeight, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, int coeff, int constWidth)
{
	StructTxtPxlLen temp;

	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
	{
		int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
		if(fontHeight<0)
			return StructTxtPxlLen_ZeroValue;
		int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

		int posMovBuff_copy = movableFontsBuffer_pos + fontHeight*pxlTxtLen;
		if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
			return StructTxtPxlLen_ZeroValue;

		int i=0,it=0,iH=0,j;
		char bufTxt[winWidth/LCD_GetFontWidth(fontID,'1')];

		FontVar[idVar].id=fontID;
		FontVar[idVar].xPos=Xpos;
		FontVar[idVar].yPos=Ypos;
		FontVar[idVar].heightType=OnlyDigits;
		FontVar[idVar].space=space;
		FontVar[idVar].bkColor=bkColor;
		FontVar[idVar].coeff=coeff;
		FontVar[idVar].widthType=constWidth;
		FontVar[idVar].rotate=rot;

		FontVar[idVar].FontMov.xImgWidth=winWidth;
		FontVar[idVar].FontMov.posWin=posWin;
		FontVar[idVar].FontMov.windowWidth=winWidth;
		FontVar[idVar].FontMov.windowHeight = winHeight;
		FontVar[idVar].FontMov.spaceEndStart=fontHeight;
		FontVar[idVar].FontMov.posBuff=movableFontsBuffer_pos;

		do
		{	if(iH>0){ if(txt[it]==' ') it++; }
			i= LCD_GetStrLenForPxlWidth(fontID,&txt[it],winWidth,space,constWidth);
			if(i==0) break;
			for(j=0;j<i;++j)
			{	if(j==sizeof(bufTxt)-1)
					break;
				bufTxt[j]=txt[it+j];
			}
			bufTxt[j]=0;
			it+=i;
			temp=LCD_DrawStr(movableFontsBuffer_pos, FontVar[idVar].FontMov.xImgWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,bkColor,coeff,constWidth);

			int diffWidth=winWidth-temp.inPixel;
			if(diffWidth>0)
				LCD_RectangleBuff(movableFontsBuffer,movableFontsBuffer_pos, FontVar[idVar].FontMov.xImgWidth,fontHeight, temp.inPixel,0, diffWidth,fontHeight, bkColor,bkColor,bkColor);

			movableFontsBuffer_pos += fontHeight*FontVar[idVar].FontMov.xImgWidth;
			iH++;
		}while(movableFontsBuffer_pos + fontHeight*FontVar[idVar].FontMov.xImgWidth < LCD_MOVABLE_FONTS_BUFF_SIZE);

		FontVar[idVar].FontMov.yImgHeight = fontHeight*iH;
		int diffHeight = winHeight - FontVar[idVar].FontMov.yImgHeight;
		LCD_CopyBuff2pLcd(rot,FontVar[idVar].FontMov.posBuff, movableFontsBuffer, FontVar[idVar].FontMov.xImgWidth,winHeight, posWin*FontVar[idVar].FontMov.xImgWidth, winWidth, Xpos,Ypos,0);

		if(diffHeight>0)
		{	switch(rot)
			{
			case Rotate_0:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos+FontVar[idVar].FontMov.yImgHeight, winWidth,diffHeight, bkColor,bkColor,bkColor);
				break;
			case Rotate_90:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos, diffHeight,winWidth, bkColor,bkColor,bkColor);
				break;
			case Rotate_180:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos+FontVar[idVar].FontMov.yImgHeight,Ypos, diffHeight,winWidth, bkColor,bkColor,bkColor);
				break;
			}
		}
		FontVar[idVar].xPos_prev = Xpos;
		FontVar[idVar].yPos_prev = Ypos;
		FontVar[idVar].widthPxl_prev = winWidth;
		FontVar[idVar].heightPxl_prev = winHeight;

		temp.inChar=0;
		temp.inPixel=winWidth;
		temp.height=winHeight;
		if(rot>Rotate_0)
			SwapUint16(&temp.inPixel,&temp.height);
		return temp;
	}
	else return StructTxtPxlLen_ZeroValue;
}

StructTxtPxlLen LCD_StrChangeColorMovV(int idVar, int rot, int posWin, int winWidth,int winHeight, int fontID, int Xpos, int Ypos, char *txt, int OnlyDigits, int space, uint32_t bkColor, uint32_t fontColor, int maxVal, int constWidth)
{
	StructTxtPxlLen temp;

	if(idVar<MAX_OPEN_FONTS_VAR_SIMULTANEOUSLY)
	{
		int fontHeight= OnlyDigits==fullHight?LCD_GetFontHeight(fontID):LCD_GetFontHalfHeight(fontID);
		if(fontHeight<0)
			return StructTxtPxlLen_ZeroValue;
		int pxlTxtLen = LCD_GetWholeStrPxlWidth(fontID,txt,space,constWidth);

		int posMovBuff_copy = movableFontsBuffer_pos + fontHeight*pxlTxtLen;
		if(posMovBuff_copy >= LCD_MOVABLE_FONTS_BUFF_SIZE)
			return StructTxtPxlLen_ZeroValue;

		int i=0,it=0,iH=0,j;
		char bufTxt[winWidth/LCD_GetFontWidth(fontID,'1')];

		FontVar[idVar].id=fontID;
		FontVar[idVar].xPos=Xpos;
		FontVar[idVar].yPos=Ypos;
		FontVar[idVar].heightType=OnlyDigits;
		FontVar[idVar].space=space;
		FontVar[idVar].bkColor=bkColor;
		FontVar[idVar].coeff=maxVal;
		FontVar[idVar].widthType=constWidth;
		FontVar[idVar].rotate=rot;

		FontVar[idVar].FontMov.xImgWidth=winWidth;
		FontVar[idVar].FontMov.posWin=posWin;
		FontVar[idVar].FontMov.windowWidth=winWidth;
		FontVar[idVar].FontMov.windowHeight = winHeight;
		FontVar[idVar].FontMov.spaceEndStart=fontHeight;
		FontVar[idVar].FontMov.posBuff=movableFontsBuffer_pos;

		do
		{	if(iH>0){ if(txt[it]==' ') it++; }
			i= LCD_GetStrLenForPxlWidth(fontID,&txt[it],winWidth,space,constWidth);
			if(i==0) break;
			for(j=0;j<i;++j)
			{	if(j==sizeof(bufTxt)-1)
					break;
				bufTxt[j]=txt[it+j];
			}
			bufTxt[j]=0;
			it+=i;
			if(iH==2)
				temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, FontVar[idVar].FontMov.xImgWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,GRAY,fontColor,maxVal,constWidth);
			else
				temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, FontVar[idVar].FontMov.xImgWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);
			//temp=LCD_DrawStrChangeColor(movableFontsBuffer_pos, FontVar[idVar].FontMov.xImgWidth, fontHeight, fontID,0,0,bufTxt,movableFontsBuffer,OnlyDigits,space,bkColor,fontColor,maxVal,constWidth);

			int diffWidth=winWidth-temp.inPixel;
			if(diffWidth>0)
				LCD_RectangleBuff(movableFontsBuffer,movableFontsBuffer_pos, FontVar[idVar].FontMov.xImgWidth,fontHeight, temp.inPixel,0, diffWidth,fontHeight, bkColor,bkColor,bkColor);

			movableFontsBuffer_pos += fontHeight*FontVar[idVar].FontMov.xImgWidth;
			iH++;
		}while(movableFontsBuffer_pos + fontHeight*FontVar[idVar].FontMov.xImgWidth < LCD_MOVABLE_FONTS_BUFF_SIZE);

		FontVar[idVar].FontMov.yImgHeight = fontHeight*iH;
		int diffHeight = winHeight - FontVar[idVar].FontMov.yImgHeight;
		LCD_CopyBuff2pLcd(rot,FontVar[idVar].FontMov.posBuff, movableFontsBuffer, FontVar[idVar].FontMov.xImgWidth,winHeight, posWin*FontVar[idVar].FontMov.xImgWidth, winWidth, Xpos,Ypos,0);

		if(diffHeight>0)
		{	switch(rot)
			{
			case Rotate_0:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos+FontVar[idVar].FontMov.yImgHeight, winWidth,diffHeight, bkColor,bkColor,bkColor);
				break;
			case Rotate_90:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos,Ypos, diffHeight,winWidth, bkColor,bkColor,bkColor);
				break;
			case Rotate_180:
				LCD_RectangleBuff(pLcd,0, LCD_GetXSize(),LCD_GetYSize(), Xpos+FontVar[idVar].FontMov.yImgHeight,Ypos, diffHeight,winWidth, bkColor,bkColor,bkColor);
				break;
			}
		}
		FontVar[idVar].xPos_prev = Xpos;
		FontVar[idVar].yPos_prev = Ypos;
		FontVar[idVar].widthPxl_prev = winWidth;
		FontVar[idVar].heightPxl_prev = winHeight;

		temp.inChar=0;
		temp.inPixel=winWidth;
		temp.height=winHeight;
		if(rot>Rotate_0)
			SwapUint16(&temp.inPixel,&temp.height);
		return temp;
	}
	else return StructTxtPxlLen_ZeroValue;
}

int LCD_StrMovVIndirect(int idVar, int incrDecr)
{
	#define M	FontVar[idVar].FontMov
	#define F	FontVar[idVar]

	if(SearchFontIndex(FontID[F.id].size, FontID[F.id].style, FontID[F.id].bkColor, FontID[F.id].color)<0)
		return -1;

	if(M.windowHeight>M.yImgHeight) return 0;

	if(M.posWin+incrDecr>=0)
		M.posWin += incrDecr;

	if(M.posWin+M.windowHeight <= M.yImgHeight)
	{
		LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth,M.windowHeight, M.posWin*M.xImgWidth, M.windowWidth,0);
		switch(F.rotate)
		{	case Rotate_0:
				LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowWidth, M.windowHeight, pLcd);
				break;
			case Rotate_90:
			case Rotate_180:
			default:
				LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowHeight, M.windowWidth, pLcd);
				break;
		}
	}
	else
	{	int height_empty = M.posWin + M.windowHeight - M.yImgHeight;
		int height_old = M.windowHeight-height_empty;

		if(height_old>0)
		{
			LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth,height_old, M.posWin*M.xImgWidth, M.windowWidth,0);
			switch(F.rotate)
			{	case Rotate_0:
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowWidth, height_old, pLcd);
					break;
				case Rotate_90:
					LCD_DisplayBuff((uint32_t)F.xPos+height_empty,(uint32_t)F.yPos, height_old, M.windowWidth, pLcd);
					break;
				case Rotate_180:
				default:
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, height_old, M.windowWidth, pLcd);
					break;
			}

			if(height_empty <= M.spaceEndStart)
			{
				switch(F.rotate)
				{	case Rotate_0:
					LCD_RectangleBuff(pLcd,0, M.windowWidth,height_empty ,0,0, M.windowWidth,height_empty, F.bkColor,F.bkColor,F.bkColor);
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos+height_old, M.windowWidth,height_empty, pLcd);
						break;
					case Rotate_90:
						LCD_RectangleBuff(pLcd,0, height_empty,M.windowWidth ,0,0, height_empty,M.windowWidth, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, height_empty,M.windowWidth, pLcd);
						break;
					case Rotate_180:
					default:
						LCD_RectangleBuff(pLcd,0, height_empty,M.windowWidth ,0,0, height_empty,M.windowWidth, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos+height_old,(uint32_t)F.yPos, height_empty,M.windowWidth, pLcd);
						break;
				}
			}
			else
			{	int height_new = height_empty - M.spaceEndStart;
				switch(F.rotate)
				{	case Rotate_0:
						LCD_RectangleBuff(pLcd,0, M.windowWidth,M.spaceEndStart, 0,0, M.windowWidth,M.spaceEndStart, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)(F.yPos+height_old), M.windowWidth,M.spaceEndStart, pLcd);
						break;
					case Rotate_90:
						LCD_RectangleBuff(pLcd,0, M.spaceEndStart,M.windowWidth, 0,0, M.spaceEndStart,M.windowWidth, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos+height_new,(uint32_t)F.yPos, M.spaceEndStart,M.windowWidth, pLcd);
						break;
					case Rotate_180:
						LCD_RectangleBuff(pLcd,0, M.spaceEndStart,M.windowWidth, 0,0, M.spaceEndStart,M.windowWidth, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos+height_old,(uint32_t)F.yPos, M.spaceEndStart,M.windowWidth, pLcd);
					default:
						break;
				}

				LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth,height_new, 0, M.windowWidth,0);
				switch(F.rotate)
				{	case Rotate_0:
					LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)(F.yPos+height_old+M.spaceEndStart), M.windowWidth,height_new, pLcd);
						break;
					case Rotate_90:
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, height_new,M.windowWidth, pLcd);
						break;
					case Rotate_180:
						LCD_DisplayBuff((uint32_t)(F.xPos+height_old+M.spaceEndStart),(uint32_t)F.yPos, height_new,M.windowWidth, pLcd);
					default:
						break;
				}
			}
		}
		else
		{	int spaceEndStart_new = M.spaceEndStart + height_old;
			if(spaceEndStart_new>0)
			{
				int height_new = M.windowHeight - spaceEndStart_new;
				switch(F.rotate)
				{	case Rotate_0:
						LCD_RectangleBuff(pLcd,0, M.windowWidth,spaceEndStart_new ,0,0, M.windowWidth,spaceEndStart_new, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowWidth,spaceEndStart_new, pLcd);
						break;
					case Rotate_90:
						LCD_RectangleBuff(pLcd,0, spaceEndStart_new,M.windowWidth ,0,0, spaceEndStart_new,M.windowWidth, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos+height_new,(uint32_t)F.yPos, spaceEndStart_new,M.windowWidth, pLcd);
						break;
					case Rotate_180:
						LCD_RectangleBuff(pLcd,0, spaceEndStart_new,M.windowWidth ,0,0,spaceEndStart_new,M.windowWidth, F.bkColor,F.bkColor,F.bkColor);
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, spaceEndStart_new,M.windowWidth, pLcd);
					default:
						break;
				}

				LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth,height_new, 0, M.windowWidth,0);
				switch(F.rotate)
				{	case Rotate_0:
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)(F.yPos+spaceEndStart_new), M.windowWidth,height_new, pLcd);
						break;
					case Rotate_90:
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, height_new,M.windowWidth, pLcd);
						break;
					case Rotate_180:
						LCD_DisplayBuff((uint32_t)F.xPos+spaceEndStart_new,(uint32_t)F.yPos, height_new,M.windowWidth, pLcd);
					default:
						break;
				}
			}
			else
			{	M.posWin=0;
				LCD_CopyBuff2pLcdIndirect(F.rotate,M.posBuff, movableFontsBuffer, M.xImgWidth,M.windowHeight, 0, M.windowWidth,0);
				switch(F.rotate)
				{	case Rotate_0:
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowWidth, M.windowHeight, pLcd);
						break;
					case Rotate_90:
					case Rotate_180:
					default:
						LCD_DisplayBuff((uint32_t)F.xPos,(uint32_t)F.yPos, M.windowHeight, M.windowWidth, pLcd);
						break;
				}
				return 1;
			}
		}
	}
	return 0;
}

int LCD_GetWholeStrPxlWidth(int fontID, char *txt, int space, int constWidth){
	int lenTxtInPixel=0;
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return -1;
	else
	{
		int len=strlen(txt);
		if(constWidth)
			LCD_Set_ConstWidthFonts(fontIndex);
		for(int i=0;i<len;i++)
			lenTxtInPixel += Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,fontID);
		if(constWidth)
			LCD_Reset_ConstWidthFonts(fontIndex);
		return lenTxtInPixel;
	}
}
int LCD_GetStrPxlWidth(int fontID, char *txt, int len, int space, int constWidth){
	int lenTxtInPixel=0;
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return -1;
	else
	{
		if(constWidth)
			LCD_Set_ConstWidthFonts(fontIndex);
		for(int i=0;i<len;i++)
			lenTxtInPixel += Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,fontID);
		if(constWidth)
			LCD_Reset_ConstWidthFonts(fontIndex);
		return lenTxtInPixel;
	}
}
int LCD_GetStrLenForPxlWidth(int fontID, char *txt, int lenInPxl, int space, int constWidth){
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return -1;
	else
	{
		int i,lenTxtInPixel=0;
		int len=0,m=strlen(txt);

		if(constWidth)
			LCD_Set_ConstWidthFonts(fontIndex);
		for(i=0;i<m;i++)
		{	len=Font[fontIndex].fontsTabPos[ (int)txt[i] ][1] + space + RealizeSpaceCorrect(txt+i,fontID);
			if(lenTxtInPixel+len>lenInPxl) break;
			lenTxtInPixel += len;
		}
		if(constWidth)
			LCD_Reset_ConstWidthFonts(fontIndex);

		return i;
	}
}

int LCD_GetFontWidth(int fontID, char font){
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return -1;
	else
		return Font[fontIndex].fontsTabPos[ (int)font ][1];
}

int LCD_GetFontHeight(int fontID)
{
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return -1;
	else
		return Font[fontIndex].height;
}

int LCD_GetFontHalfHeight(int fontID)
{
	int fontIndex=SearchFontIndex(FontID[fontID].size, FontID[fontID].style, FontID[fontID].bkColor, FontID[fontID].color);
	if(fontIndex==-1)
		return -1;
	else
		return Font[fontIndex].heightHalf;
}

int CopyCharsTab(char *buf, int len, int offset, int fontSize)
{
	const char *pChar;
	int i,j, lenChars;

	switch(fontSize)
	{
	case FONT_72:
	case FONT_72_bold:
	case FONT_72_italics:
	case FONT_130:
	case FONT_130_bold:
	case FONT_130_italics:
		pChar=CharsTab_digits;
		break;
	default:
		pChar=CharsTab_full;
		break;
	}

	lenChars=strlen(pChar);
	for(i=0;i<len;++i)
	{
		j=offset+i;
		if(j<lenChars)
			buf[i]=pChar[j];
		else
			break;
	}
	buf[i]=0;

	if(i==len)
		return 0;
	else
		return 1;
}

int LCD_SelectedSpaceBetweenFontsIncrDecr(uint8_t incrDecr, uint8_t fontStyle, uint8_t fontSize, char char1, char char2)
{
	for(int i=0;i<StructSpaceCount;i++)
	{
		if((fontStyle==space[i].fontStyle)&&(fontSize==space[i].fontSize)&&(char1==space[i].char1)&&(char2==space[i].char2))
		{
			if(incrDecr){
				if(space[i].val<127)
					space[i].val++;
			}
			else{
				if(space[i].val>-127)
					space[i].val--;
			}
			return space[i].val;
		}
	}

	if(StructSpaceCount<MAX_SPACE_CORRECT)
	{
		space[StructSpaceCount].fontStyle=fontStyle;
		space[StructSpaceCount].fontSize=fontSize;
		space[StructSpaceCount].char1=char1;
		space[StructSpaceCount].char2=char2;

		if(incrDecr){
			if(space[StructSpaceCount].val<127)
				space[StructSpaceCount].val++;
		}
		else{
			if(space[StructSpaceCount].val>-127)
				space[StructSpaceCount].val--;
		}
		StructSpaceCount++;
		return space[StructSpaceCount-1].val;
	}

	return 0xFFFF;
}

void LCD_DisplayRemeberedSpacesBetweenFonts(void){
	char bufTemp[50];
	Dbg(1,"\r\nSpacesBetweenFonts:");
	for(int i=0; i<StructSpaceCount; i++)
		DbgVar(1,50,"\r\n%d: %s %s %c %c  %d ",i+1,LCD_FontStyle2Str(bufTemp,space[i].fontStyle),LCD_FontSize2Str(bufTemp+20,space[i].fontSize),space[i].char1,space[i].char2,space[i].val);
	Dbg(1,"\r\n");
}
void LCD_WriteSpacesBetweenFontsOnSDcard(void){
	SDCardFileOpen(0,"Spaces_Between_Font.bin",FA_CREATE_ALWAYS|FA_WRITE);
	SDCardFileWrite(0,(char*)LCD_GetStructSpaceCount(),1);
	SDCardFileWrite(0,(char*)LCD_GetPtr2SpacesBetweenFontsStruct(),LCD_GetSpacesBetweenFontsStructSize());
	SDCardFileClose(0);
}
void LCD_ResetSpacesBetweenFonts(void){
	LCD_StartInsertingSpacesBetweenFonts();
	LCD_WriteSpacesBetweenFontsOnSDcard();
}
void LCD_SetSpacesBetweenFonts(void){
	if(ReadSpacesBetweenFontsFromSDcard())
		LCD_StartInsertingSpacesBetweenFonts();
}

uint16_t LCD_Ypos(StructTxtPxlLen structTemp, int cmd, int offs)
{
	static uint16_t yPos=0;
	switch(cmd)
	{
	case SetPos:
		yPos=offs;
		return yPos;
	case GetPos:
		return yPos+offs;
	default:
		return yPos+=structTemp.height+offs;
	}
}
uint16_t LCD_Xpos(StructTxtPxlLen structTemp, int cmd, int offs)
{
	static uint16_t xPos=0;
	switch(cmd)
	{
	case SetPos:
		xPos=offs;
		return xPos;
	case GetPos:
		return xPos+offs;
	case IncPos:
	default:
		return xPos+=structTemp.inPixel+offs;
	}
}

uint16_t LCD_Ymiddle(int cmd, uint32_t val)
{
	static uint16_t startPosY=0, heightY=0;
	switch(cmd)
	{
	case SetPos:
		startPosY= val;
		heightY= (val>>16)-startPosY;
		return startPosY;
	case GetPos:
	default:
		return MIDDLE(startPosY,heightY,LCD_GetFontHeight(val));
	}
}
uint16_t LCD_Xmiddle(int cmd, uint32_t val, char *txt, int space, int constWidth)
{
	static uint16_t startPosX=0, widthX=0;
	int len;
	switch(cmd)
	{
	case SetPos:
		startPosX= val;
		widthX= (val>>16)-startPosX;
		return startPosX;
	case GetPos:
	default:
		len=LCD_GetWholeStrPxlWidth(val,txt,space,constWidth);
		return MIDDLE(startPosX,widthX,(len>widthX?widthX:len));
	}
}
