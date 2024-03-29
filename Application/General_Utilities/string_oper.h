/*
 * string_oper.h
 *
 *  Created on: 07.05.2021
 *      Author: RafalMar
 */

#ifndef GENERAL_UTILITIES_STRING_OPER_H_
#define GENERAL_UTILITIES_STRING_OPER_H_

typedef enum{
	Sign_plusMinus,
	Sign_minus,
	Sign_plus,
	Sign_none,
}Int2Str_plusminus;

char* Int2Str(int value, char freeSign, int maxDigits, int plusMinus);
char* Float2Str(float value, char freeSign, int maxDigits, int plusMinus, int dec_digits);
char* StrAll(int a, ...);

void SwapUint16(uint16_t *a, uint16_t *b);

#endif /* GENERAL_UTILITIES_STRING_OPER_H_ */
