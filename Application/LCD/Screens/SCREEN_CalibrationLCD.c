/*
 * SCREEN_CalibrationLCD.c
 *
 *  Created on: Apr 23, 2024
 *      Author: maraf
 */

#include "SCREEN_CalibrationLCD.h"
#include <LCD_BasicGaphics.h>
#include <LCD_Hardware.h>
#include <string.h>
#include <stdio.h>
#include "touch.h"
#include "stmpe811.h"
#include "debug.h"

static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys)
{
  /* Draw the ring */
  BSP_LCD_FillCircle(log.x, log.y, 25);
  BSP_LCD_FillCircle(log.x, log.y, 10);
  HAL_Delay(1000);

  /* Wait until touch is pressed */
  WaitForPressedState(1);

  BSP_TS_GetState(&TS_State);
  phys->x = TS_State.x;
  phys->y = TS_State.y;

  /* Wait until touch is released */
  WaitForPressedState(0);
  BSP_LCD_FillCircle(log.x, log.y, 25);
}

void Touchscreen_Calibration(void)
{
	uint8_t status = 0;
	XY_Touch_Struct pos[2] = { {45, 45}, {LCD_GetXSize()-45, LCD_GetYSize()-45} };
	XY_Touch_Struct phys[2];

	BSP_LCD_Clear(0xFF000000);

	status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());

	if(status)
	{
		Dbg(1,"\r\nERROR touch ");
	}
	else
	{
		while (1)
		{
	      SetLogXY_1(pos[0].x, pos[0].y);
	      SetLogXY_2(pos[1].x, pos[1].y);


	      for (int i = 0; i < 2; i++)
	      {
	      	GetPhysValues(pos[i], &phys[i]);
	      }

	      BSP_LCD_FillCircle(pos[0].x, pos[0].y, 25);
	      BSP_LCD_FillCircle(pos[0].x, pos[0].y, 10);
	      HAL_Delay(1000);

	      WaitForPressedState(press);

	      BSP_TS_GetState(&TS_State);

	      phys[0].x = TS_State.x;
	      phys[0].y = TS_State.y;

	      WaitForPressedState(release);
	      BSP_LCD_FillCircle(pos[0].x, pos[0].y, 25);



		}
	}



}
