/*
 * common.h
 *
 *  Created on: 11.04.2021
 *      Author: Elektronika RM
 */

#ifndef COMMON_H_
#define COMMON_H_
#include "string_oper.h"

#define EXAMPLE(x)  TEST_##x

#define GET_DATE_COMPILATION  __DATE__
#define GET_TIME_COMPILATION  __TIME__
#define GET_CODE_FILE  __FILE__
#define GET_CODE_LINE  __LINE__
#define GET_CODE_FUNCTION  __FUNCTION__

#define GET_CODE_PLACE 	StrAll(4,"\r\n", GET_CODE_FILE, ":", GET_CODE_LINE)

#define TEXT2PRINT(message,type)		(type == 0 ? StrAll(9,"\r\n", GET_CODE_FILE, " : ", GET_CODE_FUNCTION, "() : ", Int2Str(GET_CODE_LINE,None,6,Sign_none), " \"", message,"\"") \
															  : (type == 1  ? StrAll(7,"\r\n", GET_CODE_FUNCTION, "() :", Int2Str(GET_CODE_LINE,None,6,Sign_none), " \"", message,"\"") \
																	  	  	  	 : StrAll(3,"\r\n\"", message,"\"") ))

#define getName(var)  #var
#define ABS(X)  ((X) > 0 ? (X) : -(X))
#define TOOGLE(x)  x=1-x
#define INCR(x,step,max)  ((x+step>max) ? (x=max) : (x+=step))
#define DECR(x,step,min)  ((x-step<min) ? (x=min) : (x-=step))
#define INCR_WRAP(x,step,min,max)  ((x+step>max) ? (x=min) : (x+=step))
#define DECR_WRAP(x,step,min,max)  ((x-step<min) ? (x=max) : (x-=step))
#define INTEGER(f)	 ((int)f)
#define FRACTION(f)	 (f-(int)f)
#define ROUND_VAL(val,fraction)	 ((FRACTION(val)>fraction) ? (INTEGER(val)+1) : (INTEGER(val)));
#define EVEN_INT(x)   ( (x%2) ? (x+1) : (x) )
#define TANG_ARG(deg) 0.0174532*deg
#define DEG(tang_arg) tang_arg/0.0174532

#define INCR_FLOAT_WRAP(x,step,min,max)  ((x<max-step/2) ? (x+=step) : (x=min))
#define DECR_FLOAT_WRAP(x,step,min,max)  ((x<step/2) ?     (x=max)   : (x-=step))

#define MIDDLE(startPos,widthBk,width)	startPos+(widthBk-width)/2
#define IS_RANGE(val,min,max) (val>=min)&&(val<=max)

#define SET_IN_RANGE(val,min,max) (((val)<(min))?(min):(((val)>(max))?(max):(val)))
#define SET_NEW_RANGE(val,min,max,newMin,newMax) (((val)<(min))?(newMin):(((val)>(max))?(newMax):(val)))

#define STRUCT_TAB_SIZE(_struct) (sizeof(_struct) / sizeof(&_struct[0]))

#define SET_bit(allBits,bitNr)	((allBits) |=   1<<(bitNr))
#define RST_bit(allBits,bitNr)	((allBits) &= ~(1<<(bitNr)))
#define CHECK_bit(allBits,bitNr) (((allBits)>>((bitNr)>31?31:(bitNr))) & 0x00000001)

#define MAXVAL(ptr,nmb,minVal,maxValCalc)		for(int i=0,j=(minVal); i<(nmb); (ptr[i]>j ? j=ptr[i] : 0), (maxValCalc)=j, ++i);

#define BKCOPY_VAL(dst,src,val)	dst=src; src=val
#define BKCOPY(dst,src)	dst=src

#define SDRAM __attribute__ ((section(".sdram")))

#define PTR2CHAR(txt,ptr)	char *ptr
#define PTR2INT(txt,ptr)	int *ptr

#define LCD_BUFF_XSIZE		800
#define LCD_BUFF_YSIZE		480

#define MASK(val,hexMask)		((val)&0x##hexMask)
#define SHIFT_RIGHT(val,shift,hexMask)		(((val)>>(shift))&0x##hexMask)
#define CONDITION(condition,val1,val2)	((condition) ? (val1) : (val2))

#define PERCENT_SCALE(val,maxVal)	(((val)*100)/(maxVal))

#define STRUCT_SIZE_SHAPE_POS		3

typedef struct{
	char *pData;
	uint32_t len;
}DATA_TO_SEND;

typedef struct{
	int x;
	int y;
}structPosition;

typedef struct{
	int w;
	int h;
}structSize;

typedef struct{
	structPosition pos[STRUCT_SIZE_SHAPE_POS];
	structSize size[STRUCT_SIZE_SHAPE_POS];
	uint16_t param[STRUCT_SIZE_SHAPE_POS];
}SHAPE_POS;

/* ----------- Functions Definitations ----------- */
typedef void(*figureShape)(uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t,uint32_t);

typedef int FUNC_Keyboard(uint8_t,uint8_t,figureShape,uint8_t,uint16_t,uint16_t,uint16_t,uint16_t,uint8_t,uint16_t,uint16_t,uint8_t);
#define FUNC1_SET(func,a,b,c,d,e,f,g,h,i,j,k,l)		func,a,b,c,d,e,f,g,h,i,j,k,l
#define FUNC1_DEF(pfunc)		FUNC_Keyboard *pfunc,uint8_t a,uint8_t b,figureShape c,uint8_t d,uint16_t e,uint16_t f,uint16_t g,uint16_t h,uint8_t i,uint16_t j,uint16_t k,uint8_t l
#define FUNC1_ARG		a,b,c,d,e,f,g,h,i,j,k,l

typedef void VOID_FUNCTION(void);


/* --------- End Functions Definitations ----------- */


#endif /* COMMON_H_ */
