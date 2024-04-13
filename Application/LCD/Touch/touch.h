
#ifndef TOUCH_H_
#define TOUCH_H_

#include "stm32f7xx_hal.h"

#define MAX_OPEN_TOUCH_SIMULTANEOUSLY	 200

enum TOUCH_TYPE{
  TouchPoint,
  TouchMove
};

typedef struct
{
  uint16_t x;
  uint16_t y;
}XY_Touch_Struct;

typedef struct
{
  uint16_t x_Start;
  uint16_t y_Start;
  uint16_t x_End;
  uint16_t y_End;
}TS_Temporary;

extern uint8_t touchDetect;
extern TS_Temporary  touchTemp;

uint16_t	LCD_Touch_service			(uint16_t touchType, uint16_t param);
void 		DeleteAllTouch				(void);
int 		GetTouchToTemp				(uint16_t idx);
void 		DeleteSelectTouch			(uint16_t idx);
void 		DeleteAllTouchWithout	(uint16_t idx);
int 		SetTouch						(uint16_t idx);
void 		Touchscreen_Calibration	(void);

#endif
