
#ifndef __STMPE811_H
#define __STMPE811_H

typedef enum
{
   TS_OK       = 0x00,
   TS_ERROR    = 0x01,
   TS_TIMEOUT  = 0x02
}TS_StatusTypeDef;

typedef struct
{
  uint16_t TouchDetected;
  uint16_t x;
  uint16_t y;
  uint16_t z;
}TS_StateTypeDef;

extern TS_StateTypeDef  TS_State;

uint8_t BSP_TS_GetState	(TS_StateTypeDef *TS_State);
void 	  BSP_TS_ClearIT	(void);
uint8_t BSP_TS_Init		(uint16_t xSize, uint16_t ySize);

#endif
