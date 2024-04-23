/*
 * LCD_Hardware.h
 *
 *  Created on: 10.04.2021
 *      Author: Elektronika RM
 */

#ifndef LCD_LCD_HARDWARE_H_
#define LCD_LCD_HARDWARE_H_

uint32_t LCD_GetXSize(void);
uint32_t LCD_GetYSize(void);
void LCD_DisplayBuff(uint32_t Xpos, uint32_t Ypos, uint32_t width, uint32_t height, uint32_t *pbmp);
void BSP_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_Clear(uint32_t Color);

#endif /* LCD_LCD_HARDWARE_H_ */
