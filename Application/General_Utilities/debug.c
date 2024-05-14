/*
 * debug.c
 *
 *  Created on: 02.04.2021
 *      Author: Elektronika RM
 */

#include <string.h>
#include <stdarg.h>
#include "debug.h"
#include "usart.h"
#include "mini_printf.h"
#include "FreeRTOS.h"

#define RECV_BUFF_SIZE	100
#define DEBUG_DEBUG		1

static char ALIGN_32BYTES(dbgRecvBuffer[RECV_BUFF_SIZE]);

void DEBUG_Init(void)
{
	memset(dbgRecvBuffer,0,RECV_BUFF_SIZE);
	DEBUG_ReceiveStart((uint8_t*)dbgRecvBuffer, RECV_BUFF_SIZE);
}

void Dbg(int on, char *txt)
{
	if(on)
		DEBUG_Send(txt);
}

void DbgMulti(int on, char *startTxt, char *txt, char *endTxt)
{
	if(on)
	{
		DEBUG_Send(startTxt);
		DEBUG_Send(txt);
		DEBUG_Send(endTxt);
	}
}

void DbgVar(int on, unsigned int buffLen, const char *fmt, ...)
{
	if(on)
	{
		char *temp = (char*)pvPortMalloc(buffLen);
		va_list va;
		va_start(va, fmt);
		mini_vsnprintf(temp, buffLen, fmt, va);
		va_end(va);
		DEBUG_Send(temp);
		vPortFree(temp);
	}
}

void DbgVar2(int on, unsigned int buffLen, const char *fmt, ...)
{
	if(on)
	{
		char *temp = (char*)pvPortMalloc(buffLen);
		va_list va;
		va_start(va, fmt);
		sprintf(temp, fmt, va);
		va_end(va);
		DEBUG_Send(temp);
		vPortFree(temp);
	}
}

static int DEBUG_IsAnythingReceive(void)
{
	if(dbgRecvBuffer[0]>0)
		return 1;
	else
		return 0;
}

static int DEBUG_IsTxtReceive(char *txt)
{
	if(strstr(dbgRecvBuffer,txt))
	{
		DEBUG_ReceiveStop();
		memset(dbgRecvBuffer,0,RECV_BUFF_SIZE);
		DEBUG_ReceiveStart((uint8_t*)dbgRecvBuffer, RECV_BUFF_SIZE);
		return 1;
	}
	return 0;
}

void DEBUG_RxFullBuffService(void)
{
	DEBUG_Send(dbgRecvBuffer);
	memset(dbgRecvBuffer,0,RECV_BUFF_SIZE);
	DEBUG_ReceiveStart((uint8_t*)dbgRecvBuffer, RECV_BUFF_SIZE);
	Dbg(DEBUG_DEBUG,"\r\n -----  DEBUG_RxFullBuffService -------  ");
}

int DEBUG_RcvStr(char *txt)
{
	if(DEBUG_IsAnythingReceive())
	{
	  if(DEBUG_IsTxtReceive(txt))
		  return 1;
	  else
		  return 0;
	}
	else
		return 0;
}

char* _Col(FONT_BKG_COLOR background, uint8_t red, uint8_t green, uint8_t blue)
{
	static char tab[21];
	uint8_t fontBkg;
	switch(background){case font: fontBkg=38; default: fontBkg=48;}
	mini_snprintf(tab,20,"\x1b[%d;2;%d;%d;%dm",fontBkg,red,green,blue);
	return tab;
}

