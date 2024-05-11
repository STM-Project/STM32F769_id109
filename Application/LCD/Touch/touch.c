
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
#define AB_COEFF_TAB_SIZE	5

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

static int32_t  A1[AB_COEFF_TAB_SIZE]={0}, 	_A1 = Coeff_A1;
static int32_t  B1[AB_COEFF_TAB_SIZE]={0}, 	_B1 = Coeff_B1;

static int32_t  A2[AB_COEFF_TAB_SIZE]={0}, 	_A2 = Coeff_A2;
static int32_t  B2[AB_COEFF_TAB_SIZE]={0}, 	_B2 = Coeff_B2;

static int32_t aLogX[2*AB_COEFF_TAB_SIZE]={0};
static int32_t aLogY[2*AB_COEFF_TAB_SIZE]={0};

static int32_t aPhysX[2*AB_COEFF_TAB_SIZE]={0};
static int32_t aPhysY[2*AB_COEFF_TAB_SIZE]={0};

uint8_t touchDetect;

void WaitForTouchState(uint8_t Pressed)
{
	int i=0;
	TS_StateTypeDef  State;
	do{
	   BSP_TS_GetState(&State);
	   HAL_Delay(10);
	   if (State.TouchDetected == Pressed)
	   {
	   	i=0;
	   	do {
	   		BSP_TS_GetState(&State);
	   		HAL_Delay(10);
	   		if (State.TouchDetected != Pressed)
	   			break;
	   		else if (i > 0)
	   			return;
	   		i++;
	   	 }
	   	 while (1);
	   }
	}
	while (1);
}

void SetLogXY(XY_Touch_Struct *pos, int maxSize)
{
   for(int i=0; i<maxSize; i++){
   	aLogX[i] = pos->x;
      aLogY[i] = pos->y;
   	if(i == 2*AB_COEFF_TAB_SIZE-1)
   		break;
      pos++;
   }
}

void SetPhysXY(XY_Touch_Struct *pos, int maxSize)
{
   for(int i=0; i<maxSize; i++){
   	aPhysX[i] = pos->x;
   	aPhysY[i] = pos->y;
   	if(i == 2*AB_COEFF_TAB_SIZE-1)
   		break;
      pos++;
   }
}

int CalcutaleCoeffCalibration(int maxSize)
{
	int j=0;

	void _average_AB(int max)
	{
		_A1 =0;  _A2 =0;
		_B1 =0;  _B2 =0;

		for(int i=0; i<max; i++){
			_A1 += A1[i];
			_A2 += A2[i];

			_B1 += B1[i];
			_B2 += B2[i];
		}
		_A1=_A1/max;
		_A2=_A2/max;

		_B1=_B1/max;
		_B2=_B2/max;
	}

	if(maxSize%2)
		maxSize--;

	for(int i=0; i<maxSize; i+=2)
	{
		if(aPhysX[i] != aPhysX[i+1] && aPhysY[i] != aPhysY[i+1])
		{
			A1[j] = (1000 * ( aLogX[i+1] - aLogX[i]))/ ( aPhysX[i+1] - aPhysX[i]);
			B1[j] = (1000 * aLogX[i]) - A1[j] * aPhysX[i];

			A2[j] = (1000 * ( aLogY[i+1] - aLogY[i]))/ ( aPhysY[i+1] - aPhysY[i]);
			B2[j] = (1000 * aLogY[i]) - A2[j] * aPhysY[i];
			j++;

			if(j == AB_COEFF_TAB_SIZE-1)
				break;
		}
		else
			return 1;
	}

	if(maxSize){
		_average_AB(j);
		return 0;
	}
	else
		return 1;
}

void DisplayCoeffCalibration(void)
{
	 DbgVar(1,250,"\r\nCoeff Calibration: A1=%d B1=%d   A2=%d B2=%d ",_A1,_B1,_A2,_B2);
}

static uint16_t Calibration_GetX(uint32_t x)
{
	int32_t temp= (((_A1 * x) + _B1)/1000);

	if(temp<0)
		temp=0;
	else if(temp>LCD_GetXSize())
		temp=LCD_GetXSize();

	return (uint16_t)temp;
}

static uint16_t Calibration_GetY(uint32_t y)
{
	int32_t temp= (((_A2 * y) + _B2)/1000);

	if(temp<0)
		temp=0;
	else if(temp>LCD_GetYSize())
		temp=LCD_GetYSize();

	return (uint16_t)temp;
}

uint8_t IsCalibrationDone(void)
{
  return (Calibration_Done);
}

void CalibrationWasDone(void)
{
	Calibration_Done = 1;
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
		 //_A1=1075; _B1=-24900;   _A2=1124;  _B2=-53012;
		// _A1=1067; _B1=-46030;   _A2=1111;  _B2=-65544;
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

