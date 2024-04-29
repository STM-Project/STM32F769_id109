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
#define DEBUG_TEXT_1		"\r\nERROR touch "  //!! tłumaczenia setLang

#define BK_COLOR	BLACK  //!!!! Parametry oglone stron dac w jakiejs strukturze by moc latwo je zieniac !!!

#define CIRCLE_NUMBER	3
#define CIRCLE_WIDTH		50
#define CIRCLE_1_POS		50, 50
#define CIRCLE_2_POS		LCD_GetXSize()-100, LCD_GetYSize()-100

#define ALA \
	X("CIRCLE_1_POS", 50, 50, 1) \
	X("CIRCLE_2_POS", LCD_GetXSize()-100, LCD_GetYSize()-100, 2) \
	X("CIRCLE_3_POS", 300, 300, 3)


static void GetPhysValues(XY_Touch_Struct log, XY_Touch_Struct *phys)
{
	void __ShowCircleIndirect(uint16_t x, uint16_t y, uint16_t width, uint8_t bold, uint32_t frameColor, uint32_t fillColor, uint32_t bkColor)
	{
		#define  wskBuffLcd	0
		int widthCalculated = LCD_CalculateCircleWidth(width);

		LCD_ShapeWindow	         (LCD_Circle,wskBuffLcd,widthCalculated,widthCalculated, 0,0, width, width, SetColorBoldFrame(frameColor,bold), fillColor, bkColor);
		LCD_ShapeWindowIndirect(x,y,LCD_Circle, wskBuffLcd,widthCalculated,widthCalculated, 14,14, 24,24, SetColorBoldFrame(frameColor,bold), TRANSPARENT, fillColor);
	}

	__ShowCircleIndirect(log.x, log.y, CIRCLE_WIDTH, 0, WHITE, ORANGE, BLACK);

  WaitForTouchState(press);

  BSP_TS_GetState(&TS_State);
  phys->x = TS_State.x;
  phys->y = TS_State.y;

  WaitForTouchState(release);

  LCD_ShapeIndirect(log.x,log.y,LCD_Circle, 50,50, SetColorBoldFrame(WHITE,0), RED, BLACK);
}

void Touchscreen_Calibration(void)
{
	uint8_t status = 0;


	XY_Touch_Struct pos[] = {
		#define X(a,b,c,d) {b,c},
			ALA
		#undef X
	};

	int sizeMACRO = sizeof(pos)/sizeof(&pos[0]);



//		XY_Touch_Struct pos[CIRCLE_NUMBER] = {
//				{CIRCLE_1_POS},
//				{CIRCLE_2_POS} };


//DbgVar(1,300,"\r\nAAAAAAAA: %s %s %s %d %s",__TIME__, __DATE__, __FILE__, __LINE__,

	 char bufff[][40]={
		#define X(a,b,c,d) a,
			 	 ALA
		#undef X
	 };


	 DbgVar(1,300,"\r\nAAAAAAAA: %s %s %s",bufff[0], bufff[1], bufff[2]);






	XY_Touch_Struct phys[CIRCLE_NUMBER] = {0};

	DeleteTouchLcdTask();

	//SCREEN_ResetAllParameters();
	LCD_SetCircleAA(RATIO_AA_VALUE_MAX,RATIO_AA_VALUE_MAX);

	LCD_Clear(RGB2INT(0,0,0));  LCD_Show();

	status = BSP_TS_Init(LCD_GetXSize(), LCD_GetYSize());

	if(status)
		Dbg(DEBUG_ON, DEBUG_TEXT_1);
	else
	{
	   SetLogXY(pos);

	   for (int i = 0; i < CIRCLE_NUMBER; i++)
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
