
#ifndef TOUCH_H_
#define TOUCH_H_

#include "stm32f7xx_hal.h"

typedef enum{
//	Coeff_A1 = 1070,
//	Coeff_B1 = -26479,
//	Coeff_A2 = 1085,
//	Coeff_B2 = -26279
	Coeff_A1 = 1074,
	Coeff_B1 = -28966,
	Coeff_A2 = 1129,
	Coeff_B2 = -46096
}DEFAULT_TOUCH_COEFF_CALIBR;

#define SERVICE_TOUCH_PROB_TIME_MS	20

#define TOUCH_GET_PER_X_PROBE		3
#define TOUCH_GET_PER_ANY_PROBE		1

enum TOUCH_TYPE{
  ID_TOUCH_POINT,
  ID_TOUCH_POINT_WITH_HOLD,
  ID_TOUCH_POINT_RELEASE_WITH_HOLD,		/* if press and release in only specific range time */
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
  neverMind,
  checkPress
};

typedef struct{
  uint16_t x;
  uint16_t y;
}XY_Touch_Struct;

extern uint8_t touchDetect;
extern XY_Touch_Struct  touchTemp[];

void		LCD_TOUCH_Service			(void);
uint16_t LCD_TOUCH_GetTypeAndPosition				(XY_Touch_Struct *posXY);
void 		LCD_TOUCH_DeleteAllSetTouch				(void);
int 		GetTouchToTemp				(uint16_t idx);
int 		LCD_TOUCH_SetNewPos				(uint16_t idx, uint16_t x, uint16_t y, uint16_t xLen, uint16_t yLen);
void 		LCD_TOUCH_DeleteSelectTouch			(uint16_t idx);
void 		DeleteAllTouchWithout	(uint16_t idx);
int 		LCD_TOUCH_Set				(uint16_t id, uint16_t idx, uint8_t param);
int LCD_TOUCH_isPress(void);
void SetLogXY(XY_Touch_Struct *pos, uint16_t *width, int maxSize);
void SetPhysXY(XY_Touch_Struct *pos, int maxSize);
void WaitForTouchState(uint8_t Pressed);
int LCD_TOUCH_CalcCoeffCalibr(int maxSize);
uint8_t IsCalibrationDone(void);
void CalibrationWasDone(void);
void DisplayCoeffCalibration(void);
void DisplayTouchPosXY(int touchIdx, XY_Touch_Struct pos, char *txt);
void DisplayAnyTouchPosXY(void);
uint16_t LCD_TOUCH_SetTimeParam_ms(uint16_t time);
int LCD_TOUCH_ScrollSel_Service(uint8_t nr, uint8_t pressRelease, int *y, uint8_t rollRateCoeff);
int LCD_TOUCH_ScrollSel_SetCalculate(uint8_t nr, uint16_t *offsWin, uint16_t *selWin, uint16_t WinposY, uint16_t heightAll, uint16_t heightKey, uint16_t heightWin);
int LCD_TOUCH_ScrollSel_GetSel(uint8_t nr);
int LCD_TOUCH_ScrollSel_GetRateCoeff(uint8_t nr);

#endif
