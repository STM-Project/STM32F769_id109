/*
 * LCD_Hardware.c
 *
 *  Created on: 10.04.2021
 *      Author: Elektronika RM
 */

#include "stm32f7xx_hal.h"
#include "LCD_Hardware.h"
#include "ltdc.h"
#include "dma2d.h"

static uint32_t  ActiveLayer=0;

uint32_t LCD_GetXSize(void){
  return hltdc.LayerCfg[ActiveLayer].ImageWidth;
}
uint32_t LCD_GetYSize(void){
  return hltdc.LayerCfg[ActiveLayer].ImageHeight;
}
//static uint32_t LCD_GetAddress(uint32_t Xpos, uint32_t Ypos){
//  return hltdc.LayerCfg[ActiveLayer].FBStartAdress;
//}

static int LCD_GetBytesPerPixel(void)
{
	switch(hdma2d.Init.ColorMode)
	{
	case DMA2D_OUTPUT_RGB565:
		return 2;
	case DMA2D_OUTPUT_RGB888:
		return 3;
	default:
	case DMA2D_OUTPUT_ARGB8888:
		return 4;
	}
}

static uint32_t LCD_GetPositionAddress(uint32_t Xpos, uint32_t Ypos){
  return hltdc.LayerCfg[ActiveLayer].FBStartAdress + (((LCD_GetXSize()*Ypos) + Xpos)*LCD_GetBytesPerPixel());
}

static int LCD_SetOutputOffset(uint32_t width)
{
	hdma2d.Init.OutputOffset = LCD_GetXSize() - width;
	return HAL_DMA2D_Init(&hdma2d);
}

void LCD_DisplayBuff(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint32_t *pbmp)
{
	while(HAL_DMA2D_STATE_READY!=HAL_DMA2D_GetState(&hdma2d));
	LCD_SetOutputOffset(width);
	SCB_InvalidateDCache_by_Addr(pbmp, width*height*sizeof(uint32_t));
   HAL_DMA2D_Start_IT(&hdma2d, (uint32_t)pbmp, LCD_GetPositionAddress(Xpos,Ypos), width, height);
   while(HAL_DMA2D_STATE_READY!=HAL_DMA2D_GetState(&hdma2d));
}





static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex)
{
  /* Register to memory mode with ARGB8888 as color Mode */
  hdma2d.Init.Mode         = DMA2D_R2M;
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
	  hdma2d.Init.ColorMode    = DMA2D_RGB565;
  }
  else
  { /* ARGB8888 format */
	  hdma2d.Init.ColorMode    = DMA2D_RGB888;
  }
  hdma2d.Init.OutputOffset = OffLine;

  hdma2d.Instance = DMA2D;

  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, LayerIndex) == HAL_OK)
    {
      if (HAL_DMA2D_Start(&hdma2d, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      {
        /* Polling For DMA transfer */
        HAL_DMA2D_PollForTransfer(&hdma2d, 50);
      }
    }
  }
}

void BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint32_t  Xaddress = 0;  int color=0xFFFF0000;

  /* Get the line address */
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 2*(LCD_GetXSize()*Ypos + Xpos);
  }
  else
  { /* ARGB8888 format */
    Xaddress = (hltdc.LayerCfg[ActiveLayer].FBStartAdress) + 3*(LCD_GetXSize()*Ypos + Xpos);
  }

  /* Write line */
  LL_FillBuffer(ActiveLayer, (uint32_t *)Xaddress, Length, 1, 0, color);
}

void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t RGB_Code)
{
  /* Write data value to all SDRAM memory */
  if(hltdc.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */
    *(__IO uint16_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (2*(Ypos*LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
  }
  else
  { /* ARGB8888 format */
    *(__IO uint32_t*) (hltdc.LayerCfg[ActiveLayer].FBStartAdress + (3*(Ypos*LCD_GetXSize() + Xpos))) = RGB_Code;
  }
}

void BSP_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t   decision;    /* Decision Variable */
  uint32_t  current_x;   /* Current X Value */
  uint32_t  current_y;   /* Current Y Value */
  int color=0x00FF00;

  decision = 3 - (Radius << 1);
  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y)
  {
    BSP_LCD_DrawPixel((Xpos + current_x), (Ypos - current_y), color);

    BSP_LCD_DrawPixel((Xpos - current_x), (Ypos - current_y), color);

    BSP_LCD_DrawPixel((Xpos + current_y), (Ypos - current_x), color);

    BSP_LCD_DrawPixel((Xpos - current_y), (Ypos - current_x), color);

    BSP_LCD_DrawPixel((Xpos + current_x), (Ypos + current_y), color);

    BSP_LCD_DrawPixel((Xpos - current_x), (Ypos + current_y), color);

    BSP_LCD_DrawPixel((Xpos + current_y), (Ypos + current_x), color);

    BSP_LCD_DrawPixel((Xpos - current_y), (Ypos + current_x), color);

    if (decision < 0)
    {
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }
}

void BSP_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  decision;     /* Decision Variable */
  uint32_t  current_x;   /* Current X Value */
  uint32_t  current_y;   /* Current Y Value */

  decision = 3 - (Radius << 1);

  current_x = 0;
  current_y = Radius;

  while (current_x <= current_y)
  {
    if(current_y > 0)
    {
      BSP_LCD_DrawHLine(Xpos - current_y, Ypos + current_x, 2*current_y);
      BSP_LCD_DrawHLine(Xpos - current_y, Ypos - current_x, 2*current_y);
    }

    if(current_x > 0)
    {
      BSP_LCD_DrawHLine(Xpos - current_x, Ypos - current_y, 2*current_x);
      BSP_LCD_DrawHLine(Xpos - current_x, Ypos + current_y, 2*current_x);
    }
    if (decision < 0)
    {
      decision += (current_x << 2) + 6;
    }
    else
    {
      decision += ((current_x - current_y) << 2) + 10;
      current_y--;
    }
    current_x++;
  }

  BSP_LCD_DrawCircle(Xpos, Ypos, Radius);
}

void BSP_LCD_Clear(uint32_t Color)
{
  LL_FillBuffer(ActiveLayer, (uint32_t *)(hltdc.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}
