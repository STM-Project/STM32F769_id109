
#include <LCD_BasicGaphics.h>
#include <LCD_Hardware.h>
#include <string.h>
#include <stdio.h>

#include "touch.h"
#include "stmpe811.h"
#include "debug.h"

#define MAX_OPEN_TOUCH_SIMULTANEOUSLY	 80
#define BUF_LCD_TOUCH_SIZE		100
#define MAX_NUMBER_PIONTS_TOUCH	 2

#define TIME_700_MS_TO_NUMBER_PROBE	  	(700 / SERVICE_TOUCH_PROB_TIME_MS)
#define TIME_5000_MS_TO_NUMBER_PROBE	(5000 / SERVICE_TOUCH_PROB_TIME_MS / Touch[i].param)

typedef struct
{
  uint8_t press;
  uint8_t idx;
  XY_Touch_Struct  pos[BUF_LCD_TOUCH_SIZE];
}Service_lcd_Touch_Struct;

typedef struct
{
  uint16_t id;
  uint16_t index;
  uint8_t param;
  uint8_t flags1;
  uint8_t flags2;
  XY_Touch_Struct  pos[MAX_NUMBER_PIONTS_TOUCH];
}Touch_Struct;


static Service_lcd_Touch_Struct  ServiceTouch = {.idx=0};
static Touch_Struct  Touch[MAX_OPEN_TOUCH_SIMULTANEOUSLY];
XY_Touch_Struct  touchTemp[MAX_NUMBER_PIONTS_TOUCH] = {0};

static uint8_t Calibration_Done = 0;
static int32_t  A1, A2, B1, B2;
static int32_t aPhysX[2], aPhysY[2], aLogX[2], aLogY[2];

uint8_t touchDetect;


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

static uint16_t CHECK_Touch(void)
{
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(0 < Touch[i].index)
		{
			switch(Touch[i].id)
			{
				case ID_TOUCH_POINT:
					if(ServiceTouch.press == Touch[i].param)
					{
						if((ServiceTouch.pos[0].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[0].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[0].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[0].y < Touch[i].pos[1].y))
						{
							if(press == Touch[i].param)
							{
								if(Touch[i].flags1 == 0){
									Touch[i].flags1 = 1;
									return Touch[i].index;
								}
							}
							else
								return Touch[i].index;
						}
					}
					break;

				case ID_TOUCH_GET_ANY_POINT:
					if(ServiceTouch.press == press)
					{
						if((ServiceTouch.pos[ServiceTouch.idx-1].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[ServiceTouch.idx-1].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[ServiceTouch.idx-1].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[ServiceTouch.idx-1].y < Touch[i].pos[1].y))
						{
							if(0 == (ServiceTouch.idx % Touch[i].param))
								return Touch[i].index;
						}
					}
					break;

				case ID_TOUCH_GET_ANY_POINT_WITH_WAIT:
					if(ServiceTouch.press == press)
					{
						if((ServiceTouch.pos[ServiceTouch.idx-1].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[ServiceTouch.idx-1].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[ServiceTouch.idx-1].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[ServiceTouch.idx-1].y < Touch[i].pos[1].y))
						{
							switch(Touch[i].flags1)
							{
								case 0:
									Touch[i].flags1 = 1;
									return Touch[i].index;

								case 1:
									if(ServiceTouch.idx > TIME_700_MS_TO_NUMBER_PROBE)
										Touch[i].flags1 = 2;
									break;

								case 2:
									if(0 == (ServiceTouch.idx % Touch[i].param)){
										Touch[i].flags2++;
										if(Touch[i].flags2 > TIME_5000_MS_TO_NUMBER_PROBE)
											Touch[i].flags1 = 3;
										return Touch[i].index;
									}
									break;

								case 3:
									if(0 == (ServiceTouch.idx % Touch[i].param/2))
										return Touch[i].index;
									break;
							}
						}
					}
					break;

				case ID_TOUCH_MOVE_LEFT:
					if(ServiceTouch.press == Touch[i].param)
					{
						if(2 < ServiceTouch.idx)
						{
							if( ServiceTouch.pos[0].x >= Touch[i].pos[0].x
								&& ServiceTouch.pos[0].y >= Touch[i].pos[0].y && ServiceTouch.pos[0].y <= Touch[i].pos[1].y )
							{
								for(int j=1; j<ServiceTouch.idx; ++j)
								{
									if( ServiceTouch.pos[j].x <= Touch[i].pos[1].x
										&& ServiceTouch.pos[j].y >= Touch[i].pos[0].y && ServiceTouch.pos[j].y <= Touch[i].pos[1].y )
										return Touch[i].index;
								}
							}
						}
					}
					break;

				case ID_TOUCH_MOVE_RIGHT:
					if(ServiceTouch.press == Touch[i].param)
					{
						if(2 < ServiceTouch.idx)
						{
							if( ServiceTouch.pos[0].x <= Touch[i].pos[0].x
								&& ServiceTouch.pos[0].y >= Touch[i].pos[0].y && ServiceTouch.pos[0].y <= Touch[i].pos[1].y )
							{
								for(int j=1; j<ServiceTouch.idx; ++j)
								{
									if( ServiceTouch.pos[j].x >= Touch[i].pos[1].x
										&& ServiceTouch.pos[j].y >= Touch[i].pos[0].y && ServiceTouch.pos[j].y <= Touch[i].pos[1].y )
										return Touch[i].index;
								}
							}
						}
					}
					break;

				case ID_TOUCH_MOVE_UP:
					if(ServiceTouch.press == Touch[i].param)
					{
						if(2 < ServiceTouch.idx)
						{
							if( ServiceTouch.pos[0].y >= Touch[i].pos[0].y
								&& ServiceTouch.pos[0].x >= Touch[i].pos[0].x && ServiceTouch.pos[0].x <= Touch[i].pos[1].x )
							{
								for(int j=1; j<ServiceTouch.idx; ++j)
								{
									if( ServiceTouch.pos[j].y <= Touch[i].pos[1].y
										&& ServiceTouch.pos[j].x >= Touch[i].pos[0].x && ServiceTouch.pos[j].x <= Touch[i].pos[1].x )
										return Touch[i].index;
								}
							}
						}
					}
					break;

				case ID_TOUCH_MOVE_DOWN:
					if(ServiceTouch.press == Touch[i].param)
					{
						if(2 < ServiceTouch.idx)
						{
							if( ServiceTouch.pos[0].y <= Touch[i].pos[0].y
								&& ServiceTouch.pos[0].x >= Touch[i].pos[0].x && ServiceTouch.pos[0].x <= Touch[i].pos[1].x )
							{
								for(int j=1; j<ServiceTouch.idx; ++j)
								{
									if( ServiceTouch.pos[j].y >= Touch[i].pos[1].y
										&& ServiceTouch.pos[j].x >= Touch[i].pos[0].x && ServiceTouch.pos[j].x <= Touch[i].pos[1].x )
										return Touch[i].index;
								}
							}
						}
					}
					break;

				default:
					break;
			}
		}
	}
	return 0;
}

void LCD_Touch_Service(void)
{
	XY_Touch_Struct pos = {0};

	if(CheckTouch(&pos))
	{
		if(release == ServiceTouch.press){
			ServiceTouch.press = press;
			ServiceTouch.idx = 0;
			for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i){
				Touch[i].flags1 = 0;
				Touch[i].flags2 = 0;
			}
		}

		if(BUF_LCD_TOUCH_SIZE > ServiceTouch.idx){
			ServiceTouch.pos[ServiceTouch.idx].x = pos.x;
			ServiceTouch.pos[ServiceTouch.idx++].y = pos.y;
		}
		else
			ServiceTouch.idx = 0;
	}
	else
		ServiceTouch.press = release;
}

uint16_t LCD_Touch_Get(XY_Touch_Struct *posXY)
{
	uint16_t touchRecognize = 0;

	if(0 < ServiceTouch.idx)
	{
		touchRecognize = CHECK_Touch();
		if( 0 != touchRecognize || ((release == ServiceTouch.press) && (0 == touchRecognize)) )
			ServiceTouch.idx = 0;
	}

	posXY->x = ServiceTouch.pos[ServiceTouch.idx].x;
	posXY->y = ServiceTouch.pos[ServiceTouch.idx].y;

	return touchRecognize;
}

void clearTouchTemp(void)
{
	for(int i=0; i<MAX_NUMBER_PIONTS_TOUCH; ++i){
		touchTemp[i].x = 0;
		touchTemp[i].y = 0;
	}
}

int isTouchTemp(void)
{
	if((touchTemp[0].x != touchTemp[1].x) || (touchTemp[0].y != touchTemp[1].y))
		return 1;
	else
		return 0;
}

int SetTouch(uint16_t ID, uint16_t idx, uint8_t param)
{
  if(isTouchTemp())
  {
	  for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	  {
		  if(Touch[i].index == idx)
		  {
			  clearTouchTemp();
			  return -2;
		  }
	  }
	  for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	  {
		  if(0 == Touch[i].index)
		  {
			  Touch[i].id = ID;
			  Touch[i].index = idx;
			  Touch[i].param = param;
			  Touch[i].flags1 = 0;
			  Touch[i].flags2 = 0;

			  for(int j=0; j<MAX_NUMBER_PIONTS_TOUCH; ++j)
				  Touch[i].pos[j] = touchTemp[j];

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
			touchTemp[0].x= Touch[i].pos[0].x;
			touchTemp[0].y= Touch[i].pos[0].y;
			touchTemp[1].x= Touch[i].pos[1].x;
			touchTemp[1].y= Touch[i].pos[1].y;
			return 1;
		}
	}
	return 0;
}

void DeleteAllTouch(void)
{
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
		Touch[i].index=0;
}

void DeleteSelectTouch(uint16_t idx)
{
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i){
		if(Touch[i].index==idx)
			Touch[i].index=0;
	}
}

void DeleteAllTouchWithout(uint16_t idx)
{
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i){
		if(Touch[i].index!=idx)
			Touch[i].index=0;
	}
}

