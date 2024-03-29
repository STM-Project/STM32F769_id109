/*
 * debug.h
 *
 *  Created on: 02.04.2021
 *      Author: Elektronika RM
 */

#ifndef GENERAL_UTILITIES_DEBUG_H_
#define GENERAL_UTILITIES_DEBUG_H_

void DEBUG_Init(void);
void Dbg(int on, char *txt);
void DbgMulti(int on, char *startTxt, char *txt, char *endTxt);
void DbgVar(int on, unsigned int buffLen, const char *fmt, ...);
void DEBUG_RxFullBuffService(void);
int DEBUG_RcvStr(char *txt);

#endif /* GENERAL_UTILITIES_DEBUG_H_ */
