/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
#include "debug.h"
#include "fatfs.h"
#include "sd_card.h"
#include "timer.h"
#include "SCREEN_ReadPanel.h"
#include "lang.h"
#include "TouchLcdTask.h"
#include "touch.h"
#include "debug.h"
#include "LCD_Hardware.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
extern void LCD_SetSpacesBetweenFonts(void);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
__weak void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 2048);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
#include "tim.h"

#define TOUCH_GET_PER_X_PROBE		3

enum new_touch{
	Point_1 = 1,
	Point_2,
	Point_3,
	Move_1,
	Move_2,
	Move_3,
	Move_4,
	AnyPress,
	AnyPressWithWait
};
#include "SCREEN_CalibrationLCD.h"
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */

	uint16_t state;
	XY_Touch_Struct pos;

	MX_FATFS_Init();

	 SetLang(2,English);
	 LCD_SetSpacesBetweenFonts();
	 Dbg(1,"\r\nStart ");

	 DeleteAllTouch();

	 CreateTouchLcdTask();




//	 	touchTemp[0].x= 0;
//	 	touchTemp[0].y= 0;
//	 	touchTemp[1].x= touchTemp[0].x+200;
//	 	touchTemp[1].y= touchTemp[0].y+150;
//	 	SetTouch(ID_TOUCH_POINT,Point_1,press);
//
//	 	touchTemp[0].x= 0;
//	 	touchTemp[0].y= 300;
//	 	touchTemp[1].x= touchTemp[0].x+200;
//	 	touchTemp[1].y= touchTemp[0].y+180;
//	 	SetTouch(ID_TOUCH_POINT,Point_2,release);
//
//	 	touchTemp[0].x= 600;
//	 	touchTemp[0].y= 0;
//	 	touchTemp[1].x= touchTemp[0].x+200;
//	 	touchTemp[1].y= touchTemp[0].y+150;
//	 	SetTouch(ID_TOUCH_POINT,Point_3,release);
//
//	 	touchTemp[0].x= LCD_GetXSize()-LCD_GetXSize()/5;
//	 	touchTemp[1].x= LCD_GetXSize()/5;
//	 	touchTemp[0].y= 150;
//	 	touchTemp[1].y= 300;
//	 	SetTouch(ID_TOUCH_MOVE_LEFT,Move_1,press);
//
//	 	touchTemp[0].x= LCD_GetXSize()/5;
//	 	touchTemp[1].x= LCD_GetXSize()-LCD_GetXSize()/5;
//	 	touchTemp[0].y= 150;
//	 	touchTemp[1].y= 300;
//	 	SetTouch(ID_TOUCH_MOVE_RIGHT,Move_2,release);
//
//	 	touchTemp[0].y= LCD_GetYSize()-LCD_GetYSize()/5;
//	 	touchTemp[1].y= LCD_GetYSize()/5;
//	 	touchTemp[0].x= 300;
//	 	touchTemp[1].x= 450;
//	 	SetTouch(ID_TOUCH_MOVE_UP,Move_3,press);
//
//	 	touchTemp[0].y= LCD_GetYSize()/5;
//	 	touchTemp[1].y= LCD_GetYSize()-LCD_GetYSize()/5;
//	 	touchTemp[0].x= 500;
//	 	touchTemp[1].x= 650;
//	 	SetTouch(ID_TOUCH_MOVE_DOWN,Move_4,release);

	 	touchTemp[0].x= 0;
	 	touchTemp[1].x= 800;
	 	touchTemp[0].y= 0;
	 	touchTemp[1].y= 480;
	 	//SetTouch(ID_TOUCH_GET_ANY_POINT,AnyPress,TOUCH_GET_PER_X_PROBE);
	 	SetTouch(ID_TOUCH_GET_ANY_POINT_WITH_WAIT,AnyPressWithWait,TOUCH_GET_PER_X_PROBE);




//	StartMeasureTime(0);
//
//	StopMeasureTime(0,"\r\nTimerCount:");



  /* Infinite loop */
  for(;;)
  {

	 SCREEN_ReadPanel();


	 	 	state = LCD_Touch_Get(&pos);
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
	 			case AnyPress:
	 				DbgVar(1,40,"\r\nAny Press: x= %03d y= %03d", pos.x, pos.y);
	 				break;
	 			case AnyPressWithWait:
	 				DbgVar(1,40,"\r\nAny Press With Wait: x= %03d y= %03d", pos.x, pos.y);
	 				 int *pp = SCREEN_Calibration_function();
	 					 DbgVar(1,100,"\r\n\r\n  %d  %d  %d ",*(pp+14), *(pp+15), *(pp+16));
	 				break;
	 		}



	 vTaskDelay(20);




  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

