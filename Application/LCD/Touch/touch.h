
#ifndef TOUCH_H_
#define TOUCH_H_

#include "stm32f7xx_hal.h"

enum TOUCH_TYPE {
  TouchPoint,
  TouchAndMoveLeft,
  TouchAndMoveRight,
  TouchAndMoveUp,
  TouchAndMoveDown
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

int 		LCD_Touch_service			(uint8_t touchType);
void 		DeleteAllTouch				(void);
int 		GetTouchToTemp				(uint16_t idx);
void 		DeleteSelectTouch			(uint16_t idx);
void 		DeleteAllTouchWithout	(uint16_t idx);
int 		SetTouch						(uint16_t idx);
void 		Touchscreen_Calibration	(void);

#endif
