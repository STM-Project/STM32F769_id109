
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
#define AB_COEFF_TAB_SIZE	4

#define TIME_700_MS_TO_NUMBER_PROBE	  	(700 / SERVICE_TOUCH_PROB_TIME_MS)
#define TIME_5000_MS_TO_NUMBER_PROBE	(5000 / SERVICE_TOUCH_PROB_TIME_MS / Touch[i].param)

#define SCROLL_SEL__MAX_NUMBER			10
#define SCROLL_SEL__NUMBER_PROBE2SEL	5
#define SCROLL_SEL__SEL_SPREAD			7

typedef struct{
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

void SetLogXY(XY_Touch_Struct *pos, uint16_t *width, int maxSize)
{
   for(int i=0; i<maxSize; i++){
   	aLogX[i] = pos->x + *width/2;
      aLogY[i] = pos->y + *width/2;
   	if(i == 2*AB_COEFF_TAB_SIZE-1)
   		break;
      pos++;
      width++;
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

int LCD_TOUCH_CalcCoeffCalibr(int maxSize)
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

			if(j == AB_COEFF_TAB_SIZE)
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

void DisplayCoeffCalibration(void){
	 DbgVar(1,250,"\r\nCoeff Calibration: A1=%d B1=%d   A2=%d B2=%d ",_A1,_B1,_A2,_B2);
}

void DisplayTouchPosXY(int touchIdx, XY_Touch_Struct pos, char *txt){
	if(GetTouchToTemp(touchIdx))
		DbgVar(1,100,"\r\nPos (%d--"Cya_"%d"_X"--%d)  (%d--"Cya_"%d"_X"--%d) %s",touchTemp[0].x, pos.x, touchTemp[1].x,	touchTemp[0].y, pos.y, touchTemp[1].y, txt);
}
void DisplayAnyTouchPosXY(void){
	if(press == ServiceTouch.press)
		DbgVar(1,20,"\r\nPos %d:  %d %d ",ServiceTouch.idx, ServiceTouch.pos[ServiceTouch.idx-1].x, ServiceTouch.pos[ServiceTouch.idx-1].y);
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

static uint8_t GetTouchPos(XY_Touch_Struct *pos)
{
 /* if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)==0) */
  if(touchDetect)
  {
	 touchDetect=0;
	 BSP_TS_ClearIT();
	 BSP_TS_GetState(&TS_State);

	 if(TS_State.TouchDetected)
	 {
		 pos->x = Calibration_GetX(TS_State.x);
	    pos->y = Calibration_GetY(TS_State.y);
	    return 1;
	 }
  }
  return 0;
}

static uint16_t GetTouchType(int *param)
{
	*param=0;
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(0 < Touch[i].index)
		{
			switch(Touch[i].id)
			{
				case ID_TOUCH_POINT:
					if( pressRelease == Touch[i].param ? 1 : Touch[i].param == ServiceTouch.press)
					{
						for(int j=0; j<ServiceTouch.idx; j++){
							if((ServiceTouch.pos[j].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[j].x < Touch[i].pos[1].x) &&
								(ServiceTouch.pos[j].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[j].y < Touch[i].pos[1].y))
							{
								if(press == ServiceTouch.press)
								{
									if(Touch[i].flags1 == 0){
										Touch[i].flags1 = 1;
										*param=j;
										return Touch[i].index;
									}
								}
								else{
									*param=j;
									return Touch[i].index;
								}
						}}
					}
					break;

				case ID_TOUCH_POINT_WITH_HOLD:
					if(ServiceTouch.press == press)
					{
						if(IS_RANGE(ServiceTouch.idx-1, 0, BUF_LCD_TOUCH_SIZE-1)){
						if((ServiceTouch.pos[ServiceTouch.idx-1].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[ServiceTouch.idx-1].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[ServiceTouch.idx-1].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[ServiceTouch.idx-1].y < Touch[i].pos[1].y))
						{
							if(ServiceTouch.idx > Touch[i].param){
								if(0==Touch[i].flags1){
									Touch[i].flags1 = 1;
									*param = ServiceTouch.idx-1;
									return Touch[i].index;
								}
							}
						}
						else Touch[i].flags1 = 1;
						}
					}
					break;

				case ID_TOUCH_POINT_RELEASE_WITH_HOLD:
					if(ServiceTouch.press == press)
					{
						if(IS_RANGE(ServiceTouch.idx-1, 0, BUF_LCD_TOUCH_SIZE-1)){
						if((ServiceTouch.pos[ServiceTouch.idx-1].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[ServiceTouch.idx-1].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[ServiceTouch.idx-1].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[ServiceTouch.idx-1].y < Touch[i].pos[1].y))
						{
								if(ServiceTouch.idx < Touch[i].param){
									if(!Touch[i].flags1)
										Touch[i].flags1 = 1;
								}
								else	Touch[i].flags1 = 2;
						}}
					}
					else{
							if(ServiceTouch.idx < Touch[i].param && 1 == Touch[i].flags1){
								*param = ServiceTouch.idx-1;
								return Touch[i].index;
							}
					}
					break;

				case ID_TOUCH_GET_ANY_POINT:
					if(ServiceTouch.press == press)
					{
						if(IS_RANGE(ServiceTouch.idx-1, 0, BUF_LCD_TOUCH_SIZE-1)){
						if((ServiceTouch.pos[ServiceTouch.idx-1].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[ServiceTouch.idx-1].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[ServiceTouch.idx-1].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[ServiceTouch.idx-1].y < Touch[i].pos[1].y))
						{
							if(0 == (ServiceTouch.idx % Touch[i].param)){
								*param = ServiceTouch.idx-1;
								return Touch[i].index;
							}
						}}
					}
					break;

				case ID_TOUCH_GET_ANY_POINT_WITH_WAIT:
					if(ServiceTouch.press == press)
					{
						if(IS_RANGE(ServiceTouch.idx-1, 0, BUF_LCD_TOUCH_SIZE-1)){
						if((ServiceTouch.pos[ServiceTouch.idx-1].x >= Touch[i].pos[0].x) && (ServiceTouch.pos[ServiceTouch.idx-1].x < Touch[i].pos[1].x) &&
							(ServiceTouch.pos[ServiceTouch.idx-1].y >= Touch[i].pos[0].y) && (ServiceTouch.pos[ServiceTouch.idx-1].y < Touch[i].pos[1].y))
						{
							switch(Touch[i].flags1)
							{
								case 0:
									Touch[i].flags1 = 1;
									*param = ServiceTouch.idx-1;
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
										*param = ServiceTouch.idx-1;
										return Touch[i].index;
									}
									break;

								case 3:
									if(0 == (ServiceTouch.idx % Touch[i].param/2)){
										*param = ServiceTouch.idx-1;
										return Touch[i].index;
									}
									break;
							}
						}}
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
										&& ServiceTouch.pos[j].y >= Touch[i].pos[0].y && ServiceTouch.pos[j].y <= Touch[i].pos[1].y ){
										*param = j;
										return Touch[i].index;
									}
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
										&& ServiceTouch.pos[j].y >= Touch[i].pos[0].y && ServiceTouch.pos[j].y <= Touch[i].pos[1].y ){
										*param = j;
										return Touch[i].index;
									}
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
										&& ServiceTouch.pos[j].x >= Touch[i].pos[0].x && ServiceTouch.pos[j].x <= Touch[i].pos[1].x ){
										*param = j;
										return Touch[i].index;
									}
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
										&& ServiceTouch.pos[j].x >= Touch[i].pos[0].x && ServiceTouch.pos[j].x <= Touch[i].pos[1].x ){
										*param = j;
										return Touch[i].index;
									}
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
/* Poniższa funkcja jest wywoływana w jednym wątku i zapisuje do zmiennej: ServiceTouch.idx */
void LCD_TOUCH_Service(void)
{
	XY_Touch_Struct pos = {0};

	if(GetTouchPos(&pos))
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
/* Poniższa funkcja jest wywoływana w drugim wątku i zapisuje też do zmiennej: ServiceTouch.idx */
uint16_t LCD_TOUCH_GetTypeAndPosition(XY_Touch_Struct *posXY)
{
	uint16_t touchRecognize = 0;
	int param = 0;

	if(0 < ServiceTouch.idx)
	{
		touchRecognize = GetTouchType(&param);
		if( 0 != touchRecognize || ((release == ServiceTouch.press) && (0 == touchRecognize)) )
			ServiceTouch.idx = 0;
	}

	posXY->x = ServiceTouch.pos[param].x;
	posXY->y = ServiceTouch.pos[param].y;

	return touchRecognize;
}

static void clearTouchTemp(void)
{
	for(int i=0; i<MAX_NUMBER_PIONTS_TOUCH; ++i){
		touchTemp[i].x = 0;
		touchTemp[i].y = 0;
	}
}

static int isTouchTemp(void)
{
	if((touchTemp[0].x != touchTemp[1].x) || (touchTemp[0].y != touchTemp[1].y))
		return 1;
	else
		return 0;
}

int LCD_TOUCH_isPress(void){
	return ServiceTouch.press;
}

int LCD_TOUCH_Set(uint16_t ID, uint16_t idx, uint8_t param)
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

int LCD_TOUCH_SetNewPos(uint16_t idx, uint16_t x, uint16_t y, uint16_t xLen, uint16_t yLen)
{
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
	{
		if(Touch[i].index==idx)
		{
			Touch[i].pos[0].x = x;
			Touch[i].pos[0].y = y;
			Touch[i].pos[1].x = x + xLen;
			Touch[i].pos[1].y = y + yLen;
			return 1;
		}
	}
	return 0;
}

void LCD_TOUCH_DeleteAllSetTouch(void)
{
	for(int i=0; i<MAX_OPEN_TOUCH_SIMULTANEOUSLY; ++i)
		Touch[i].index=0;
}

void LCD_TOUCH_DeleteSelectTouch(uint16_t idx)
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

uint16_t LCD_TOUCH_SetTimeParam_ms(uint16_t time){
	return time/SERVICE_TOUCH_PROB_TIME_MS;
}

int LCD_TOUCH_ScrollSel_Service(uint8_t nr, uint8_t pressRelease, uint16_t *y, uint8_t rollRateCoeff)
{
	static struct SCROLL_SEL{
		uint16_t posY;
		uint16_t prevY[SCROLL_SEL__NUMBER_PROBE2SEL];
		uint16_t countTouchProbe2Sel;
		int delta;
		int delta_copy;
		uint8_t stateTouch;
		uint8_t itx;
		uint8_t selPoint;
	}roll[SCROLL_SEL__MAX_NUMBER]={0};

	switch(pressRelease)
	{
	case press:
		roll[nr].stateTouch = pressRelease;
		if( (roll[nr].posY > 0) && (roll[nr].countTouchProbe2Sel >= SCROLL_SEL__NUMBER_PROBE2SEL) )
		{
			roll[nr].delta = (int)roll[nr].posY - *y;
			if(ABS(roll[nr].delta) > SCROLL_SEL__SEL_SPREAD)
				roll[nr].selPoint = 1;
		}
		else roll[nr].selPoint = 0;

		roll[nr].posY = *y;
		roll[nr].prevY[ (roll[nr].itx < SCROLL_SEL__NUMBER_PROBE2SEL-1) ? roll[nr].itx++ : roll[nr].itx ] = *y;
		roll[nr].countTouchProbe2Sel++;

		roll[nr].delta *= rollRateCoeff;
		return roll[nr].delta;

	case release:
		roll[nr].stateTouch = pressRelease;
		roll[nr].posY = 0;
		roll[nr].delta_copy = roll[nr].delta;
		roll[nr].delta = 0;
		roll[nr].itx = 0;
		roll[nr].countTouchProbe2Sel=0;

		if(roll[nr].selPoint == 0)
			return roll[nr].prevY[0];
		else
			return 0;

	case checkPress:
		*y = roll[nr].stateTouch;
		switch(roll[nr].stateTouch){
			case press:		return roll[nr].delta;
			case release:	return roll[nr].prevY[0];
		}
		break;

	case neverMind:
		return roll[nr].delta_copy;
	}

	return 0;
}

int LCD_TOUCH_ScrollSel_SetCalculate(uint8_t nr, uint16_t *offsWin, uint16_t *selWin, uint16_t WinposY, uint16_t heightAll, uint16_t heightKey, uint16_t heightWin)
{
	static struct SCROLL_SEL{
		uint16_t offsWin;
		uint16_t selWin;
		uint16_t rateCoeff;
	}roll[SCROLL_SEL__MAX_NUMBER]={0};

	if(NULL == offsWin && NULL == selWin){
		if(WinposY)	return roll[nr].rateCoeff;
		else			return roll[nr].selWin;
	}

	if(0 == WinposY && 0 == heightAll && 0 == heightKey){
		roll[nr].offsWin = *offsWin;
		roll[nr].selWin = *selWin;
		roll[nr].rateCoeff = heightWin;			/*	'heightWin' as param RateCoeff to write setup */
		return 1;
	}

	void _refreshWin(void){
		*offsWin = roll[nr].offsWin;
		*selWin = roll[nr].selWin;
	}

	uint16_t statePress;
	int val = LCD_TOUCH_ScrollSel_Service(nr,checkPress, &statePress,0);

	switch(statePress)
	{
		case press:
			if(val > 0){
				if(roll[nr].offsWin < ((heightAll-heightWin)+1 - val) )
					roll[nr].offsWin += val;
			}
			else{
				val *= -1;
				if(roll[nr].offsWin > val)
					roll[nr].offsWin -= val;
				else
					roll[nr].offsWin = 0;
			}
			_refreshWin();
			return statePress;

		case release:
			roll[nr].selWin  = (val + roll[nr].offsWin - WinposY) / (heightKey-1);
			_refreshWin();
			return statePress;
	}
	return neverMind;
}

int LCD_TOUCH_ScrollSel_GetSel(uint8_t nr){
	return LCD_TOUCH_ScrollSel_SetCalculate(nr, NULL, NULL, 0,0,0,0);
}
int LCD_TOUCH_ScrollSel_GetRateCoeff(uint8_t nr){
	return LCD_TOUCH_ScrollSel_SetCalculate(nr, NULL, NULL, 1,0,0,0);
}

void LCD_TOUCH_ScrollSel_FreeRolling(uint8_t nr, FUNC1_DEF(pFunc))
{
	int delta = LCD_TOUCH_ScrollSel_Service(nr,neverMind,NULL,0);
	if(delta!=0)
	{
		int paramCoeff[3] = {150,40,0};
		int temp = 0, temp2 = 0;
		uint16_t val = 0;

		/* DbgVar(1,7,BkR_" %d "_X,delta); */
		if(delta > 0)
		{
			delta *= paramCoeff[0];
			val = delta;
			while(1)
			{
				if(LCD_TOUCH_ScrollSel_Service(nr,press, &val,1))
					pFunc(FUNC1_ARG);

				if(press == LCD_TOUCH_isPress())
					break;

				paramCoeff[2] += 2;
				temp += (paramCoeff[1]+paramCoeff[2]);
				temp2 = delta/temp;

				if(temp2 > 0)
				{
					if(val > temp2)
						val -= temp2;
					else
						break;
				}
				else
					break;
			}
		}
		else
		{
			delta = ABS(delta)*paramCoeff[0];
			val = 1;
			while(1)
			{
				if(LCD_TOUCH_ScrollSel_Service(nr,press, &val,1))
					pFunc(FUNC1_ARG);

				if(press == LCD_TOUCH_isPress())
					break;

				paramCoeff[2] += 2;
				temp += (paramCoeff[1]+paramCoeff[2]);
				temp2 = delta/temp;

				if(temp2 > 0)
				{
					if(val < delta-temp2)
						val += temp2;
					else
						break;
				}
				else
					break;
			}
		}
	}

	LCD_TOUCH_ScrollSel_Service(nr,release,NULL,0);
}

uint8_t LCD_TOUCH_ScrollSel_DetermineRateRoll(uint8_t nr, uint16_t touchState, uint16_t xPos){
	 return (GetTouchToTemp(touchState) && IS_RANGE(xPos, touchTemp[0].x+3*(touchTemp[1].x-touchTemp[0].x)/4, touchTemp[1].x)) ? LCD_TOUCH_ScrollSel_GetRateCoeff(nr) : 1;
}

