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

xTaskHandle vtaskTouchLcdHandle;

void vtaskTouchLcd(void *pvParameters)
{
	uint16_t x,y;
	portTickType xLastExecutionTime;

	xLastExecutionTime = xTaskGetTickCount();
	vTaskDelayUntil(&xLastExecutionTime, 100);

	while(1)
	{
		CheckTouch(&x, &y);

		vTaskDelayUntil(&xLastExecutionTime, 20);
	}
}

void CreateTouchLcdTask(void)
{
	xTaskCreate(vtaskTouchLcd, "vtaskTouchLcd", 1000, NULL, (unsigned portBASE_TYPE ) 5, &vtaskTouchLcdHandle);
}
