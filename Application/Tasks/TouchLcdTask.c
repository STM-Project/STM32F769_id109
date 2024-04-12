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

#define SERVICE_TOUCH_PROB_TIME_MS	50

xTaskHandle vtaskTouchLcdHandle;

void vtaskTouchLcd(void *pvParameters)
{
	XY_Touch_Struct pos;
	portTickType xLastExecutionTime;

	xLastExecutionTime = xTaskGetTickCount();
	vTaskDelayUntil(&xLastExecutionTime, 100);

	while(1)
	{
		if(CheckTouch(&pos))
			DbgVar(1,30,"\r\nPos1: x=%d, y=%d ",pos.x, pos.y);

		vTaskDelayUntil(&xLastExecutionTime, SERVICE_TOUCH_PROB_TIME_MS);
	}
}

void CreateTouchLcdTask(void)
{
	xTaskCreate(vtaskTouchLcd, "vtaskTouchLcd", 1000, NULL, (unsigned portBASE_TYPE ) 5, &vtaskTouchLcdHandle);
}
