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

	touchTemp.x_Start= 400;
	touchTemp.y_Start= 240;
	touchTemp.x_End= touchTemp.x_Start+50;
	touchTemp.y_End= touchTemp.y_Start+50;
	SetTouch(Point_1);

	touchTemp.x_Start= 0;
	touchTemp.y_Start= 400;
	touchTemp.x_End= touchTemp.x_Start+100;
	touchTemp.y_End= touchTemp.y_Start+50;
	SetTouch(Point_2);

	touchTemp.x_Start= 700;
	touchTemp.y_Start= 0;
	touchTemp.x_End= touchTemp.x_Start+100;
	touchTemp.y_End= touchTemp.y_Start+50;
	SetTouch(Point_3);

	xLastExecutionTime = xTaskGetTickCount();
	vTaskDelayUntil(&xLastExecutionTime, 100);

	while(1)
	{

		if(LCD_Touch_service(TouchPoint,Point_1))
			Dbg(1,"\r\nTouchPoint_1");

//		if(LCD_Touch_service(TouchPoint,Point_1))
//			Dbg(1,"\r\nTouchPoint_1");
//		else if(LCD_Touch_service(TouchPoint,Point_2))
//			Dbg(1,"\r\nTouchPoint_2");
//		else if(LCD_Touch_service(TouchPoint,Point_3))
//			Dbg(1,"\r\nTouchPoint_3");
//		else if(LCD_Touch_service(TouchMove,0))
//			Dbg(1,"\r\nTouchAndMoveLeft");

	
			
		vTaskDelayUntil(&xLastExecutionTime, SERVICE_TOUCH_PROB_TIME_MS);
	}
}

void CreateTouchLcdTask(void)
{
	xTaskCreate(vtaskTouchLcd, "vtaskTouchLcd", 1000, NULL, (unsigned portBASE_TYPE ) 5, &vtaskTouchLcdHandle);
}
