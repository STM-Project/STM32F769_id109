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
	Point_1 = 5000,
	Point_2,
	Point_3
};

void vtaskTouchLcd(void *pvParameters)
{
	portTickType xLastExecutionTime;
	uint16_t state;

	touchTemp.x_Start= 0;
	touchTemp.y_Start= 0;
	touchTemp.x_End= touchTemp.x_Start+200;
	touchTemp.y_End= touchTemp.y_Start+150;
	SetTouch(Point_1);

	touchTemp.x_Start= 0;
	touchTemp.y_Start= 300;
	touchTemp.x_End= touchTemp.x_Start+200;
	touchTemp.y_End= touchTemp.y_Start+180;
	SetTouch(Point_2);

	touchTemp.x_Start= 600;
	touchTemp.y_Start= 0;
	touchTemp.x_End= touchTemp.x_Start+200;
	touchTemp.y_End= touchTemp.y_Start+150;
	SetTouch(Point_3);

	xLastExecutionTime = xTaskGetTickCount();
	vTaskDelayUntil(&xLastExecutionTime, 100);

	while(1)
	{
		state = LCD_Touch_service(TouchPoint,0);
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
			case 10000:
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
