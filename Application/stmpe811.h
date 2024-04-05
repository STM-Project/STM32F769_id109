
#ifndef __STMPE811_H
#define __STMPE811_H

typedef struct
{
  uint16_t TouchDetected;
  uint16_t x;
  uint16_t y;
  uint16_t z;
}TS_StateTypeDef;

extern TS_StateTypeDef  TS_State;

#endif
