/*
 * Keyboard.c
 *
 *  Created on: Sep 8, 2024
 *      Author: maraf
 */

#include "Keyboard.h"
//#include "LCD_fonts_images.h"
//#include "LCD_BasicGaphics.h"
#include "touch.h"

#define MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY		20

KEYBOARD_SETTINGS s[MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY]={0}, c={0};


void _deleteAllTouchs(void){
	for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
		for(int i=0; i<s[j].nmbTouch; ++i)
			LCD_TOUCH_DeleteSelectTouch(s[j].startTouchIdx+i);
	}
}
void _deleteAllTouchParams(void){
	for(int j=0; j<MAX_NUMBER_OPENED_KEYBOARD_SIMULTANEOUSLY; ++j){
		s[j].forTouchIdx = 0;  //NoTouch
		s[j].nmbTouch = 0;
	}
}
void _deleteTouchs(int nr){
	for(int i=0; i<s[nr].nmbTouch; ++i)
		LCD_TOUCH_DeleteSelectTouch(s[nr].startTouchIdx+i);
}
void _deleteTouchParams(int nr){
	s[nr].forTouchIdx = 0;  //NoTouch
	s[nr].nmbTouch = 0;
}
