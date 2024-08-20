/*
 * string_oper.c
 *
 *  Created on: 07.05.2021
 *      Author: RafalMar
 */
#include "stm32f7xx_hal.h"
#include "string_oper.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "double_float.h"

#define SIZE_STRBUFF		200
#define SIZE_STRBUFF_OUT	150

static char strBuff[SIZE_STRBUFF]={0};
static char strBuffOut[SIZE_STRBUFF_OUT]={0};
static int idx=0;

char* Int2Str(int value, char freeSign, int maxDigits, int plusMinus)
{
	int i=10,k=1,j,idx_copy;
	int absolutValue;
	char sign;

	idx_copy=idx;

	int _IsSign(void)
	{
		if(value<0)
		{
			switch(plusMinus)
			{
			case Sign_plusMinus:
			case Sign_minus:
				return 1;
			}
		}
		else if(value>0)
		{
			switch(plusMinus)
			{
			case Sign_plusMinus:
			case Sign_plus:
				return 1;
			}
		}
		return 0;
	}

	if(value<0){
		absolutValue=-value;
		sign='-';
	}
	else{
		absolutValue=value;
		sign='+';
	}

	while(1)
	{
		if(absolutValue<i)
		{
		   if(k<maxDigits)
			{
		   	if(idx+maxDigits >= SIZE_STRBUFF){
		   		idx=0;
		   		idx_copy=0;
		   	}

		   	if(Sign_none!=plusMinus)
		   	{
			   	if(Space==freeSign)
			   		strBuff[idx++]=Space;
			   	else
			   	{
			   		if(1==_IsSign())
			   			strBuff[idx++]=sign;
			   		else
			   			strBuff[idx++]=Space;
			   	}
		   	}

		   	if(None!=freeSign)
		   	{
			   	j=maxDigits-k;
			   	memset(&strBuff[idx],freeSign,j);
			   	idx+=j;
		   	}

		   	if(Space==freeSign)
		   	{
		   		if(1==_IsSign())
		   			strBuff[idx-1]=sign;
		   	}
			}
		   else
		   {
		   	if(idx+k >= SIZE_STRBUFF){
		   		idx=0;
		   		idx_copy=0;
		   	}

		   	if(Sign_none!=plusMinus)
		   	{
		   		if(1==_IsSign())
		   			strBuff[idx++]=sign;
		   		else
		   			strBuff[idx++]=Space;
		   	}
		   }
			itoa(absolutValue,&strBuff[idx],10);
			idx+=k;
			break;
		}
		else
		{
			i*=10;
			k++;
		}
	}
	strBuff[idx++]=0;
	return strBuff+idx_copy;
}

char* Float2Str(float value, char freeSign, int maxDigits, int plusMinus, int dec_digits)
{
	int i=10,k=1,j,idx_copy;
	float absolutValue;
	char sign;

	idx_copy=idx;

	int _IsSign(void)
	{
		if(value<0)
		{
			switch(plusMinus)
			{
			case Sign_plusMinus:
			case Sign_minus:
				return 1;
			}
		}
		else if(value>0)
		{
			switch(plusMinus)
			{
			case Sign_plusMinus:
			case Sign_plus:
				return 1;
			}
		}
		return 0;
	}

	if(value<0){
		absolutValue=-value;
		sign='-';
	}
	else{
		absolutValue=value;
		sign='+';
	}

	while(1)
	{
		if(absolutValue<i)
		{
		   if(k<maxDigits)
			{
		   	if(idx+maxDigits+dec_digits+1 >= SIZE_STRBUFF){
		   		idx=0;
		   		idx_copy=0;
		   	}

		   	if(Sign_none!=plusMinus)
		   	{
		   		if(Space==freeSign)
			   		strBuff[idx++]=Space;
			   	else
			   	{
			   		if(1==_IsSign())
			   			strBuff[idx++]=sign;
			   		else
			   			strBuff[idx++]=Space;
			   	}
		   	}

		   	if(None!=freeSign)
		   	{
			   	j=maxDigits-k;
			   	memset(&strBuff[idx],freeSign,j);
			   	idx+=j;
		   	}

		   	if(Space==freeSign)
		   	{
		   		if(1==_IsSign())
		   			strBuff[idx-1]=sign;
		   	}
			}
		   else
		   {
		   	if(idx+k+dec_digits+1 >= SIZE_STRBUFF){
		   		idx=0;
		   		idx_copy=0;
		   	}

		   	if(Sign_none!=plusMinus)
		   	{
		   		if(1==_IsSign())
		   			strBuff[idx++]=sign;
		   		else
		   			strBuff[idx++]=Space;
		   	}
		   }
			float2stri(&strBuff[idx], absolutValue,dec_digits);
			idx+=k+dec_digits+1;
			break;
		}
		else
		{
			i*=10;
			k++;
		}
	}
	strBuff[idx++]=0;
	return strBuff+idx_copy;
}

char* StrAll(int nmbStr, ...)
{
	int i,len,lenOut;
	char *ptr;
	va_list va;

	va_start(va,0);
	strBuffOut[0]=0;

	for(i=0;i<nmbStr;++i)
	{
		ptr=va_arg(va,char*);
		len=strlen(ptr);
		lenOut=strlen(strBuffOut);

		if(lenOut+len>=SIZE_STRBUFF_OUT){
			strncat(strBuffOut,ptr,SIZE_STRBUFF_OUT-lenOut-1);
			break;
		}
		else
			strncat(strBuffOut,ptr,len);
	}

	va_end(va);
	return strBuffOut;
}

void SwapUint16(uint16_t *a, uint16_t *b)
{
	uint16_t a_temp=*a;
	uint16_t b_temp=*b;
	*a=b_temp;
	*b=a_temp;
}

int STRING_GetTheLongestTxt(int nmb, char **txt)
{
	int maxLen=0, maxLen_temp=0, itMaxLen=0;

	for(int i=0; i<nmb; ++i){
		maxLen_temp = strlen(*txt++);
		if(maxLen_temp > maxLen){
			maxLen = maxLen_temp;
			itMaxLen = i;
		}
	}
	return itMaxLen;
}

void Int16ToCharBuff(char* buff, uint16_t val){
	buff[0] = val>>8;
	buff[1] = val;
}
void Int32ToCharBuff(char* buff, uint32_t val){
	buff[0] = val>>24;
	buff[1] = val>>16;
	buff[2] = val>>8;
	buff[3] = val;
}
uint16_t CharBuffToInt16(char* buff){
	return ((buff[0]<<8) | buff[1]);
}
uint32_t CharBuffToInt32(char* buff){
	return ((buff[0]<<24) | (buff[1]<<16) | (buff[2]<<8) | buff[3]);
}
