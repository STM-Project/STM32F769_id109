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

#define SERVICE_TOUCH_PROB_TIME_MS	20

xTaskHandle vtaskTouchLcdHandle;

enum new_touch{
	Point_1 = 1,
	Point_2,
	Point_3,
	MoveLeftX
};

void vtaskTouchLcd(void *pvParameters)
{
	portTickType xLastExecutionTime;
	uint16_t state;

	touchTemp[0].x= 0;
	touchTemp[0].y= 0;
	touchTemp[1].x= touchTemp[0].x+200;
	touchTemp[1].y= touchTemp[0].y+150;
	SetTouch(ID_TOUCH_POINT,Point_1);

	touchTemp[0].x= 0;
	touchTemp[0].y= 300;
	touchTemp[1].x= touchTemp[0].x+200;
	touchTemp[1].y= touchTemp[0].y+180;
	SetTouch(ID_TOUCH_POINT,Point_2);

	touchTemp[0].x= 600;
	touchTemp[0].y= 0;
	touchTemp[1].x= touchTemp[0].x+200;
	touchTemp[1].y= touchTemp[0].y+150;
	SetTouch(ID_TOUCH_POINT,Point_3);

	touchTemp[0].x= LCD_GetXSize()-LCD_GetXSize()/5;
	touchTemp[1].x= LCD_GetXSize()/5;
	SetTouch(ID_TOUCH_MOVE,MoveLeftX);

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
			case MoveLeftX:
				Dbg(1,"\r\nTouchMove");
				break;
		}		

		vTaskDelayUntil(&xLastExecutionTime, SERVICE_TOUCH_PROB_TIME_MS);
	}
}

void CreateTouchLcdTask(void)
{
	xTaskCreate(vtaskTouchLcd, "vtaskTouchLcd", 1000, NULL, (unsigned portBASE_TYPE ) 5, &vtaskTouchLcdHandle);
}
