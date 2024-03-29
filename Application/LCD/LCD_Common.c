/*
 * LCD_Common.c
 *
 *  Created on: 11.04.2021
 *      Author: Elektronika RM
 */

#include "stm32f7xx_hal.h"
#include "freeRTOS.h"
#include "task.h"
#include "LCD_Common.h"
#include "common.h"

#define MAX_ELEMENTS_REFRESH_IN_SCREEN	40

portTickType refreshScreenVar[MAX_ELEMENTS_REFRESH_IN_SCREEN];

void LCD_AllRefreshScreenClear(void){
	for(int i=0; i<MAX_ELEMENTS_REFRESH_IN_SCREEN; ++i)
		refreshScreenVar[i] = xTaskGetTickCount();
}
void LCD_RefreshScreenClear(int nrRefresh){
	refreshScreenVar[nrRefresh] = xTaskGetTickCount();
}
int LCD_IsRefreshScreenTimeout(int nrRefresh, int timeout){
	if((xTaskGetTickCount()-refreshScreenVar[nrRefresh])>timeout){
		LCD_RefreshScreenClear(nrRefresh);
		return 1;
	}
	else return 0;
}



//float dfeee = GetTransitionCoeff(WHITE, uint32_t colorTo, GetTransitionColor(WHITE, MYGRAY, 0.43));

uint32_t GetTransitionColor(uint32_t colorFrom, uint32_t colorTo, float transCoeff)
{
		  if(1.0<=transCoeff) return colorTo;
	else if(0.0>=transCoeff) return colorFrom;

	uint8_t R_1= (colorFrom>>16)&0x000000FF;
	uint8_t G_1= (colorFrom>>8) &0x000000FF;
	uint8_t B_1=  colorFrom     &0x000000FF;

	uint8_t R_2= (colorTo>>16)&0x000000FF;
	uint8_t G_2= (colorTo>>8) &0x000000FF;
	uint8_t B_2=  colorTo     &0x000000FF;

	uint8_t R_delta= ABS((int)R_1-(int)R_2);
	uint8_t G_delta= ABS((int)G_1-(int)G_2);
	uint8_t B_delta= ABS((int)B_1-(int)B_2);

	uint8_t R_coeff=  (uint8_t)((float)R_delta*transCoeff);
	uint8_t G_coeff=  (uint8_t)((float)G_delta*transCoeff);
	uint8_t B_coeff=  (uint8_t)((float)B_delta*transCoeff);

	uint8_t R_out = R_1>R_2 ? R_1-R_coeff : R_1+R_coeff;
	uint8_t G_out = G_1>G_2 ? G_1-G_coeff : G_1+G_coeff;
	uint8_t B_out = B_1>B_2 ? B_1-B_coeff : B_1+B_coeff;

	return RGB2INT(R_out,G_out,B_out);
}

float GetTransitionCoeff(uint32_t colorFrom, uint32_t colorTo, uint32_t colorTrans)
{
	int div=0;
	float average=0;

	uint8_t R_1= (colorFrom>>16)&0x000000FF;
	uint8_t G_1= (colorFrom>>8) &0x000000FF;
	uint8_t B_1=  colorFrom     &0x000000FF;

	uint8_t R_2= (colorTo>>16)&0x000000FF;
	uint8_t G_2= (colorTo>>8) &0x000000FF;
	uint8_t B_2=  colorTo     &0x000000FF;

	uint8_t R_tr= (colorTrans>>16)&0x000000FF;
	uint8_t G_tr= (colorTrans>>8) &0x000000FF;
	uint8_t B_tr=  colorTrans     &0x000000FF;

	float R_delta= ABS((int)R_1-(int)R_2);	 if(R_delta>0) div++;
	float G_delta= ABS((int)G_1-(int)G_2);	 if(G_delta>0) div++;
	float B_delta= ABS((int)B_1-(int)B_2);	 if(B_delta>0) div++;

	if(R_delta>0) average += (float)ABS(R_tr-R_1)/R_delta;
	if(G_delta>0) average += (float)ABS(G_tr-G_1)/G_delta;
	if(B_delta>0) average += (float)ABS(B_tr-B_1)/B_delta;

	return (average/div);
}
