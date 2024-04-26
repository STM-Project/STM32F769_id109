/*
 * SCREEN_CalibrationLCD.c
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */
#include "SCREEN_CalibrationLCD.h"
#include <LCD_BasicGaphics.h>
#include <stdio.h>
#include "touch.h"
#include "stmpe811.h"
#include "TouchLcdTask.h"
#include "LCD_Common.h"
#include "debug.h"

#define DEBUG_ON	1
#define DEBUG_TEXT_1		"\r\nERROR touch "

static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys)
{

	void __ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
	{
		int widthCalculated=LCD_CalculateCircleWidth(width);
//		LCD_ClearPartScreen(0,widthCalculated,widthCalculated,bkColor);
//		LCD_ShapeIndirect(x,	y,	LCD_Circle, 	width,width, SetColorBoldFrame(frameColor,bold), fillColor, bkColor);

		LCD_ShapeWindow	         (LCD_Circle,0,widthCalculated,widthCalculated, 0,0, width,          width,     SetColorBoldFrame(frameColor,bold), fillColor, bkColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Circle, 0,widthCalculated,widthCalculated, 14,14, 24,24,                  SetColorBoldFrame(frameColor,bold), fillColor, fillColor);
	}

  /* Draw the ring */
	__ShowCircleIndirect(log.x, 	 log.y, 	  50, 0, WHITE, ORANGE, BLACK);
//	__ShowCircleIndirect(log.x+14, log.y+14, 24, 0, WHITE, ORANGE, ORANGE);


//	LCD_ShapeIndirect(log.x,	log.y,	LCD_Circle, 	100,100, SetColorBoldFrame(WHITE,0), ORANGE, BLACK);
//
//	LCD_ClearPartScreen(0,LCD_CalculateCircleWidth(75),LCD_CalculateCircleWidth(75),ORANGE);
//	LCD_ShapeIndirect(log.x+13,	log.y+13,	LCD_Circle, 	75,75, SetColorBoldFrame(WHITE,0), ORANGE, ORANGE);

	//__Show_FrameAndCircle_Indirect(log.x+13,log.y+13, 24, 0);

  /* Wait until touch is pressed */
  WaitForTouchState(press);

  BSP_TS_GetState(&TS_State);
  phys->x = TS_State.x;
  phys->y = TS_State.y;

  /* Wait until touch is released */
  WaitForTouchState(release);

  LCD_ShapeIndirect(log.x,log.y,LCD_Circle, 50,50, SetColorBoldFrame(WHITE,0), RED, BLACK);
}

void Touchscreen_Calibration(void)
{
	uint8_t status = 0;
	XY_Touch_Struct pos[2] = { {50, 50}, {LCD_GetXSize()-100, LCD_GetYSize()-100} };
	XY_Touch_Struct phys[2];

	DeleteTouchLcdTask();
	LCD_Clear(RGB2INT(0,0,0));  LCD_Show();

	status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());

	if(status)
		Dbg(DEBUG_ON, DEBUG_TEXT_1);
	else
	{
	   SetLogXY(pos);

	   for (int i = 0; i < 2; i++)
	   {
	      	GetPhysValues(pos[i], &phys[i]);
	   }

	   SetPhysXY(phys);

	   CalcutaleCoeffCalibration();

	   CalibrationWasDone();

	   DisplayCoeffCalibration();

	   HAL_Delay(2000);

	   CreateTouchLcdTask();
	}
}
