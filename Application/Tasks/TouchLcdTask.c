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

	touchTemp.pos[0].x= 0;
	touchTemp.pos[0].y= 0;
	touchTemp.pos[1].x= touchTemp.pos[0].x+200;
	touchTemp.pos[1].y= touchTemp.pos[0].y+150;
	SetTouch(Point_1);

	touchTemp.pos[0].x= 0;
	touchTemp.pos[0].y= 300;
	touchTemp.pos[1].x= touchTemp.pos[0].x+200;
	touchTemp.pos[1].y= touchTemp.pos[0].y+180;
	SetTouch(Point_2);

	touchTemp.pos[0].x= 600;
	touchTemp.pos[0].y= 0;
	touchTemp.pos[1].x= touchTemp.pos[0].x+200;
	touchTemp.pos[1].y= touchTemp.pos[0].y+150;
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
