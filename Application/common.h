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
#define IS_RANGE(x,min,max) (x>=min)&&(x<=max)

#define STRUCT_TAB_SIZE(_struct) (sizeof(_struct) / sizeof(&_struct[0]))

#define SET_bit(allBits,bitNr)	(allBits |=   1<<(bitNr))
#define RST_bit(allBits,bitNr)	(allBits &= ~(1<<(bitNr)))
#define CHECK_bit(allBits,bitNr) ((allBits>>(bitNr>63?63:bitNr)) & 0x0000000000000001)

#define SDRAM __attribute__ ((section(".sdram")))

#define LCD_BUFF_XSIZE		800
#define LCD_BUFF_YSIZE		480

typedef struct{
	char *pData;
	uint32_t len;
}DATA_TO_SEND;

typedef struct{
	int x;
	int y;
}structPosition;

#endif /* COMMON_H_ */
