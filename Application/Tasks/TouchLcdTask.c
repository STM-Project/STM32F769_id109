/*
 * TouchTask.c
 *
 *  Created on: Apr 10, 2024
 *      Author: maraf
 */
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "touch.h"
#include "debug.h"
#include "LCD_Hardware.h"

#define SERVICE_TOUCH_PROB_TIME_MS	20

xTaskHandle vtaskTouchLcdHandle;

enum new_touch{
	Point_1 = 1,
	Point_2,
	Point_3,
	Move_1,
	Move_2,
	Move_3,
	Move_4
};

void vtaskTouchLcd(void *pvParameters)
{
	portTickType xLastExecutionTime;
	uint16_t state;

	touchTemp[0].x= 0;
	touchTemp[0].y= 0;
	touchTemp[1].x= touchTemp[0].x+200;
	touchTemp[1].y= touchTemp[0].y+150;
	SetTouch(ID_TOUCH_POINT,Point_1,release);

	touchTemp[0].x= 0;
	touchTemp[0].y= 300;
	touchTemp[1].x= touchTemp[0].x+200;
	touchTemp[1].y= touchTemp[0].y+180;
	SetTouch(ID_TOUCH_POINT,Point_2,release);

	touchTemp[0].x= 600;
	touchTemp[0].y= 0;
	touchTemp[1].x= touchTemp[0].x+200;
	touchTemp[1].y= touchTemp[0].y+150;
	SetTouch(ID_TOUCH_POINT,Point_3,release);

	touchTemp[0].x= LCD_GetXSize()-LCD_GetXSize()/5;
	touchTemp[1].x= LCD_GetXSize()/5;
	touchTemp[0].y= 150;
	touchTemp[1].y= 300;
	SetTouch(ID_TOUCH_MOVE_LEFT,Move_1,press);

	touchTemp[0].x= LCD_GetXSize()/5;
	touchTemp[1].x= LCD_GetXSize()-LCD_GetXSize()/5;
	touchTemp[0].y= 150;
	touchTemp[1].y= 300;
	SetTouch(ID_TOUCH_MOVE_RIGHT,Move_2,release);

	touchTemp[0].y= LCD_GetYSize()-LCD_GetYSize()/5;
	touchTemp[1].y= LCD_GetYSize()/5;
	touchTemp[0].x= 300;
	touchTemp[1].x= 450;
	SetTouch(ID_TOUCH_MOVE_UP,Move_3,press);

	touchTemp[0].y= LCD_GetYSize()/5;
	touchTemp[1].y= LCD_GetYSize()-LCD_GetYSize()/5;
	touchTemp[0].x= 500;
	touchTemp[1].x= 650;
	SetTouch(ID_TOUCH_MOVE_DOWN,Move_4,release);

	xLastExecutionTime = xTaskGetTickCount();
	vTaskDelayUntil(&xLastExecutionTime, 100);

	while(1)
	{
		state = LCD_Touch_service();
		switch(state)
		{
			case Point_1:
				Dbg(1,"\r\nTouchPoint_1");
				break;
			case Point_2:
				Dbg(1,"\r\nTouchPoint_2");
				break;
			case Point_3:
				Dbg(1,"\r\nTouchPoint_3");
				break;
			case Move_1:
				Dbg(1,"\r\nTouchMove_1");
				break;
			case Move_2:
				Dbg(1,"\r\nTouchMove_2");
				break;
			case Move_3:
				Dbg(1,"\r\nTouchMove_3");
				break;
			case Move_4:
				Dbg(1,"\r\nTouchMove_4");
				break;
		}		

		vTaskDelayUntil(&xLastExecutionTime, SERVICE_TOUCH_PROB_TIME_MS);
	}
}

void CreateTouchLcdTask(void)
{
	xTaskCreate(vtaskTouchLcd, "vtaskTouchLcd", 1000, NULL, (unsigned portBASE_TYPE ) 5, &vtaskTouchLcdHandle);
}
