
#ifndef TOUCH_H_
#define TOUCH_H_

#include "stm32f7xx_hal.h"

enum TOUCH_TYPE{
  ID_TOUCH_POINT,
  ID_TOUCH_MOVE_LEFT,
  ID_TOUCH_MOVE_RIGHT,
  ID_TOUCH_MOVE_UP,
  ID_TOUCH_MOVE_DOWN,
  ID_TOUCH_GET_ANY_POINT,
  ID_TOUCH_GET_ANY_POINT_WITH_WAIT
};

enum TOUCH_PRESS_RELEASE{
  release,
  press,
  pressRelease,
  neverMind
};

typedef struct
{
  uint16_t x;
  uint16_t y;
}XY_Touch_Struct;

extern uint8_t touchDetect;
extern XY_Touch_Struct  touchTemp[];

uint16_t	LCD_Touch_service			(XY_Touch_Struct *posXY);
void 		DeleteAllTouch				(void);
int 		GetTouchToTemp				(uint16_t idx);
void 		DeleteSelectTouch			(uint16_t idx);
void 		DeleteAllTouchWithout	(uint16_t idx);
int 		SetTouch						(uint16_t id, uint16_t idx, uint8_t param);
void 		Touchscreen_Calibration	(void);

#endif
