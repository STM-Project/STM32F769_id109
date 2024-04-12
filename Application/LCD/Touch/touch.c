
#include <LCD_BasicGaphics.h>
#include <LCD_Hardware.h>
#include <string.h>
#include <stdio.h>

#include "touch.h"
#include "stmpe811.h"
#include "debug.h"

#define MAX_OPEN_TOUCH_SIMULTANEOUSLY	 200
#define BUF_LCD_TOUCH_SIZE	100

typedef struct
{
  uint8_t press;
  uint8_t ongoing;
  uint8_t idx;
  XY_Touch_Struct  pos[BUF_LCD_TOUCH_SIZE];
}Service_lcd_Touch_Struct;

typedef struct
{
  uint16_t index;
  uint16_t x_Start;
  uint16_t y_Start;
  uint16_t x_End;
  uint16_t y_End;
}Touch_Struct;

static Service_lcd_Touch_Struct  ServiceTouch = {.idx=0};
static Touch_Struct  Touch[MAX_OPEN_TOUCH_SIMULTANEOUSLY];

static uint8_t Calibration_Done = 0;
static int32_t  A1, A2, B1, B2;
static int32_t aPhysX[2], aPhysY[2], aLogX[2], aLogY[2];

uint8_t touchDetect;
TS_Temporary  touchTemp;

//static void WaitForPressedState(uint8_t Pressed)
//{
//  TS_StateTypeDef  State;
//
//  do
//  {
//    BSP_TS_GetState(&State);
//    HAL_Delay(10);
//    if (State.TouchDetected == Pressed)
//    {
//      uint16_t TimeStart = HAL_GetTick();
//      do {
//        BSP_TS_GetState(&State);
//        HAL_Delay(10);
//        if (State.TouchDetected != Pressed)
//        {
//          break;
//        } else if ((HAL_GetTick() - 100) > TimeStart)
//        {
//          return;
//        }
//      } while (1);
//    }
//  } while (1);
//}

/**
  * @brief  Get Physical position
  * @param  LogX : logical X position
  * @param  LogY : logical Y position
  * @param  pPhysX : Physical X position
  * @param  pPhysY : Physical Y position
  * @retval None
  */
//static void GetPhysValues(int32_t LogX, int32_t LogY, int32_t * pPhysX, int32_t * pPhysY)
//{
//  /* Draw the ring */
//  BSP_LCD_FillCircle(LogX, LogY, 25);
//  BSP_LCD_FillCircle(LogX, LogY, 10);
//  osDelay(1000);
//
//  /* Wait until touch is pressed */
//  WaitForPressedState(1);
//
//  BSP_TS_GetState(&TS_State);
//  *pPhysX = TS_State.x;
//  *pPhysY = TS_State.y;
//
//  /* Wait until touch is released */
//  WaitForPressedState(0);
//  BSP_LCD_FillCircle(LogX, LogY, 25);
//}

//void Touchscreen_Calibration(void)
//{
//  uint8_t status = 0;
//  uint8_t i = 0;
//
//  BSP_LCD_Clear(0xFF000000);
//
//  status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());
//
//  if (status != TS_OK)
//  {
//	  dbg("\r\nERROR touch ");
//  }
//
//  while (1)
//  {
//    if (status == TS_OK)
//    {
//      aLogX[0] = 45;
//      aLogY[0] = 45;
//      aLogX[1] = LCD_GetXSize() - 45;
//      aLogY[1] = LCD_GetYSize() - 45;
//
//      for (i = 0; i < 2; i++)
//      {
//        GetPhysValues(aLogX[i], aLogY[i], &aPhysX[i], &aPhysY[i]);
//      }
//      A1 = (1000 * ( aLogX[1] - aLogX[0]))/ ( aPhysX[1] - aPhysX[0]);
//      B1 = (1000 * aLogX[0]) - A1 * aPhysX[0];
//
//      A2 = (1000 * ( aLogY[1] - aLogY[0]))/ ( aPhysY[1] - aPhysY[0]);
//      B2 = (1000 * aLogY[0]) - A2 * aPhysY[0];
//
//      Calibration_Done = 1;
//
//      DbgSprintf(250,"\r\nCalibracja: %d %d   %d %d ",A1,B1,A2,B2);
//      //alibracja: 1072 -33256   1133 -46773
//      //Calibracja: A1=1070; B1=-29900;   A2=1130;  B2=-43140;
//      HAL_Delay(1000);
//      return;
//    }
//
//    HAL_Delay(5);
//  }
//}

static uint16_t Calibration_GetX(uint32_t x)
{
	int32_t temp= (((A1 * x) + B1)/1000);
	//temp-=80;
	if(temp<0) temp=0;
	else if(temp>LCD_GetXSize()) temp=LCD_GetXSize();
	return (uint16_t)temp;
}

static uint16_t Calibration_GetY(uint32_t y)
{
	int32_t temp= (((A2 * y) + B2)/1000);
	//temp-=90;
	if(temp<0) temp=0;
	else if(temp>LCD_GetYSize()) temp=LCD_GetYSize();
	return (uint16_t)temp;
}

uint8_t IsCalibrationDone(void)
{
  return (Calibration_Done);
}






static uint8_t CheckTouch(XY_Touch_Struct *pos)
{
  if(touchDetect)
  {
	 touchDetect=0;
	 BSP_TS_ClearIT();
	 BSP_TS_GetState(&TS_State);
	 if(TS_State.TouchDetected)
	 {
		 A1=1062; B1=-10224;   A2=1083;  B2=-23229;
		 pos->x = Calibration_GetX(TS_State.x);
	     pos->y = Calibration_GetY(TS_State.y);
	     return 1;
	 }
  }
  return 0;
}

static int CHECK_TouchPiont(void)
{
	if(0 == ServiceTouch.press)
	{
		for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
		{
			if((ServiceTouch.pos[0].x >= Touch[i].x_Start)&&(ServiceTouch.pos[0].x < Touch[i].x_End) &&
				(ServiceTouch.pos[0].y >= Touch[i].y_Start)&&(ServiceTouch.pos[0].y < Touch[i].y_End))
			{
				return (int)Touch[i].index;
			}
	}	
	return -1;
}

static int CHECK_TouchAndMoveLeft(void)
{
	//if(0 == ServiceTouch.press)
	//{
		if( ServiceTouch.pos[0].x > LCD_GetXSize()-LCD_GetXSize()/5 )
		{
			for(int i=1; i<BUF_LCD_TOUCH_SIZE; ++i)
			{
				if( ServiceTouch.pos[i].x > LCD_GetXSize()-LCD_GetXSize()/5 )
					return 1;
			}	
		}
	//}	
	return -1;
}

int LCD_Touch_service(uint8_t touchType)
{
    int state = -1;
	XY_Touch_Struct  pos;
	
	if(CheckTouch(&pos))
	{		
		if(0 == ServiceTouch.press){
			ServiceTouch.press = 1;
			ServiceTouch.idx = 0;
		}	
		
		if(BUF_LCD_TOUCH_SIZE > ServiceTouch.idx){
			ServiceTouch.ongoing = 1;
			ServiceTouch.pos[ServiceTouch.idx].x = pos.x;
			ServiceTouch.pos[ServiceTouch.idx++].x = pos.y;
		}
		else
			ServiceTouch.ongoing = 0;
	}
	else
	{
		ServiceTouch.press = 0;
		ServiceTouch.ongoing = 0;
	}

	if(0 < ServiceTouch.idx)
	{
		switch(touchType)
		{
			case TouchPoint:
				state = CHECK_TouchPiont();
				break;
			case TouchAndMoveLeft:
				state = CHECK_TouchAndMoveLeft();
				break;
			default:
				state = -1;
				break;
		}
		
		if( (-1 != state) || ((-1 == state) && (BUF_LCD_TOUCH_SIZE <= ServiceTouch.idx)) ) // -1 nie rozpoznano schematu dodtyku ENUM dac !!!!
			ServiceTouch.idx = 0;
	}	
	
	return state;
}










void clearTouchTemp(void)
{
	touchTemp.x_Start=0;
	touchTemp.y_Start=0;
	touchTemp.x_End=0;
	touchTemp.y_End=0;
}

int isTouchTemp(void)
{
	if((touchTemp.x_Start != touchTemp.x_End)&&(touchTemp.y_Start != touchTemp.y_End))
		return 1;
	else
		return 0;
}

int SetTouch(uint16_t idx)
{
  int i;
  if(1==isTouchTemp())
  {
	for(i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(Touch[i].index==idx)
		{
		   clearTouchTemp();
		   return -2;
		}
	}
	for(i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
	  if(Touch[i].index==0)
	  {
		 Touch[i].index=idx;
		 Touch[i].x_Start= touchTemp.x_Start;
		 Touch[i].x_End= touchTemp.x_End;
		 Touch[i].y_Start= touchTemp.y_Start;
		 Touch[i].y_End= touchTemp.y_End;
		 clearTouchTemp();
		 return i;
	  }
	}
  }
  clearTouchTemp();
  return -1;
}

int GetTouchToTemp(uint16_t idx)
{
	int i;
	for(i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(Touch[i].index==idx)
		{
			touchTemp.x_Start= Touch[i].x_Start;
			touchTemp.y_Start= Touch[i].y_Start;
			touchTemp.x_End= Touch[i].x_End;
			touchTemp.y_End= Touch[i].y_End;
			return 1;
		}
	}
	return 0;
}

void DeleteAllTouch(void)
{
	int i;
	for(i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		Touch[i].index=0;
	}
}

void DeleteSelectTouch(uint16_t idx)
{
	int i;
	for(i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(Touch[i].index==idx)
			Touch[i].index=0;
	}
}

void DeleteAllTouchWithout(uint16_t idx)
{
	int i;
	for(i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(Touch[i].index!=idx) Touch[i].index=0;
	}
}

