/**
  ******************************************************************************
  * @file    stmpe811.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    15-December-2014
  * @brief   This file provides a set of functions needed to manage the STMPE811
  *          IO Expander devices.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stmpe811.h"
//#include "Settings.h"
//#include "dbg.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */ 
  
/** @defgroup STMPE811
  * @{
  */   

/** @defgroup TS_Driver_structure  Touch Sensor Driver structure
  * @{
  */
/* Chip IDs */
#define STMPE811_ID                     0x0811

/* Identification registers & System Control */
#define STMPE811_REG_CHP_ID_LSB         0x00
#define STMPE811_REG_CHP_ID_MSB         0x01
#define STMPE811_REG_ID_VER             0x02

/* Global interrupt Enable bit */
#define STMPE811_GIT_EN                 0x01

/* IO expander functionalities */
#define STMPE811_ADC_FCT                0x01
#define STMPE811_TS_FCT                 0x02
#define STMPE811_IO_FCT                 0x04
#define STMPE811_TEMPSENS_FCT           0x08

/* Global Interrupts definitions */
#define STMPE811_GIT_IO                 0x80  /* IO interrupt                   */
#define STMPE811_GIT_ADC                0x40  /* ADC interrupt                  */
#define STMPE811_GIT_TEMP               0x20  /* Not implemented                */
#define STMPE811_GIT_FE                 0x10  /* FIFO empty interrupt           */
#define STMPE811_GIT_FF                 0x08  /* FIFO full interrupt            */
#define STMPE811_GIT_FOV                0x04  /* FIFO overflowed interrupt      */
#define STMPE811_GIT_FTH                0x02  /* FIFO above threshold interrupt */
#define STMPE811_GIT_TOUCH              0x01  /* Touch is detected interrupt    */
#define STMPE811_ALL_GIT                0x1F  /* All global interrupts          */
#define STMPE811_TS_IT                  (STMPE811_GIT_TOUCH | STMPE811_GIT_FTH |  STMPE811_GIT_FOV | STMPE811_GIT_FF | STMPE811_GIT_FE) /* Touch screen interrupts */

/* General Control Registers */
#define STMPE811_REG_SYS_CTRL1          0x03
#define STMPE811_REG_SYS_CTRL2          0x04
#define STMPE811_REG_SPI_CFG            0x08

/* Interrupt system Registers */
#define STMPE811_REG_INT_CTRL           0x09
#define STMPE811_REG_INT_EN             0x0A
#define STMPE811_REG_INT_STA            0x0B
#define STMPE811_REG_IO_INT_EN          0x0C
#define STMPE811_REG_IO_INT_STA         0x0D

/* IO Registers */
#define STMPE811_REG_IO_SET_PIN         0x10
#define STMPE811_REG_IO_CLR_PIN         0x11
#define STMPE811_REG_IO_MP_STA          0x12
#define STMPE811_REG_IO_DIR             0x13
#define STMPE811_REG_IO_ED              0x14
#define STMPE811_REG_IO_RE              0x15
#define STMPE811_REG_IO_FE              0x16
#define STMPE811_REG_IO_AF              0x17

/* ADC Registers */
#define STMPE811_REG_ADC_INT_EN         0x0E
#define STMPE811_REG_ADC_INT_STA        0x0F
#define STMPE811_REG_ADC_CTRL1          0x20
#define STMPE811_REG_ADC_CTRL2          0x21
#define STMPE811_REG_ADC_CAPT           0x22
#define STMPE811_REG_ADC_DATA_CH0       0x30
#define STMPE811_REG_ADC_DATA_CH1       0x32
#define STMPE811_REG_ADC_DATA_CH2       0x34
#define STMPE811_REG_ADC_DATA_CH3       0x36
#define STMPE811_REG_ADC_DATA_CH4       0x38
#define STMPE811_REG_ADC_DATA_CH5       0x3A
#define STMPE811_REG_ADC_DATA_CH6       0x3B
#define STMPE811_REG_ADC_DATA_CH7       0x3C

/* Touch Screen Registers */
#define STMPE811_REG_TSC_CTRL           0x40
#define STMPE811_REG_TSC_CFG            0x41
#define STMPE811_REG_WDM_TR_X           0x42
#define STMPE811_REG_WDM_TR_Y           0x44
#define STMPE811_REG_WDM_BL_X           0x46
#define STMPE811_REG_WDM_BL_Y           0x48
#define STMPE811_REG_FIFO_TH            0x4A
#define STMPE811_REG_FIFO_STA           0x4B
#define STMPE811_REG_FIFO_SIZE          0x4C
#define STMPE811_REG_TSC_DATA_X         0x4D
#define STMPE811_REG_TSC_DATA_Y         0x4F
#define STMPE811_REG_TSC_DATA_Z         0x51
#define STMPE811_REG_TSC_DATA_XYZ       0x52
#define STMPE811_REG_TSC_FRACT_XYZ      0x56
#define STMPE811_REG_TSC_DATA_INC       0x57
#define STMPE811_REG_TSC_DATA_NON_INC   0xD7
#define STMPE811_REG_TSC_I_DRIVE        0x58
#define STMPE811_REG_TSC_SHIELD         0x59

/* Touch Screen Pins definition */
#define STMPE811_TOUCH_YD               STMPE811_PIN_7
#define STMPE811_TOUCH_XD               STMPE811_PIN_6
#define STMPE811_TOUCH_YU               STMPE811_PIN_5
#define STMPE811_TOUCH_XU               STMPE811_PIN_4
#define STMPE811_TOUCH_IO_ALL           (uint32_t)(STMPE811_TOUCH_YD | STMPE811_TOUCH_XD | STMPE811_TOUCH_YU | STMPE811_TOUCH_XU)

/* IO Pins definition */
#define STMPE811_PIN_0                  0x01
#define STMPE811_PIN_1                  0x02
#define STMPE811_PIN_2                  0x04
#define STMPE811_PIN_3                  0x08
#define STMPE811_PIN_4                  0x10
#define STMPE811_PIN_5                  0x20
#define STMPE811_PIN_6                  0x40
#define STMPE811_PIN_7                  0x80
#define STMPE811_PIN_ALL                0xFF

/* IO Pins directions */
#define STMPE811_DIRECTION_IN           0x00
#define STMPE811_DIRECTION_OUT          0x01

/* IO IT types */
#define STMPE811_TYPE_LEVEL             0x00
#define STMPE811_TYPE_EDGE              0x02

/* IO IT polarity */
#define STMPE811_POLARITY_LOW           0x00
#define STMPE811_POLARITY_HIGH          0x04

/* IO Pin IT edge modes */
#define STMPE811_EDGE_FALLING           0x01
#define STMPE811_EDGE_RISING            0x02

/* TS registers masks */
#define STMPE811_TS_CTRL_ENABLE         0x01
#define STMPE811_TS_CTRL_STATUS         0x80

/** @defgroup STMPE811_Private_Types_Definitions
  * @{
  */

/** @defgroup STMPE811_Private_Defines
  * @{
  */
#define I2C_TIMEOUT  100 /*<! Value of Timeout when I2C communication fails */
#define STMPE811_MAX_INSTANCE         2

#define TS_I2C_ADDRESS                        0x82

#define TS_SWAP_NONE                    0x00
#define TS_SWAP_X                       0x01
#define TS_SWAP_Y                       0x02
#define TS_SWAP_XY                      0x04


#define TOUCH_RESOLUTION	2
/**
  * @}
  */

/** @defgroup STMPE811_Private_Macros
  * @{
  */
typedef enum
{
   IO_MODE_INPUT = 0,   /* input floating */
   IO_MODE_OUTPUT,      /* output Push Pull */
   IO_MODE_IT_RISING_EDGE,   /* float input - irq detect on rising edge */
   IO_MODE_IT_FALLING_EDGE,  /* float input - irq detect on falling edge */
   IO_MODE_IT_LOW_LEVEL,     /* float input - irq detect on low level */
   IO_MODE_IT_HIGH_LEVEL,    /* float input - irq detect on high level */
   /* following modes only available on MFX*/
   IO_MODE_ANALOG,           /* analog mode */
   IO_MODE_OFF,              /* when pin isn't used*/
   IO_MODE_INPUT_PU,         /* input with internal pull up resistor */
   IO_MODE_INPUT_PD,         /* input with internal pull down resistor */
   IO_MODE_OUTPUT_OD,          /* Open Drain output without internal resistor */
   IO_MODE_OUTPUT_OD_PU,       /* Open Drain output with  internal pullup resistor */
   IO_MODE_OUTPUT_OD_PD,       /* Open Drain output with  internal pulldown resistor */
   IO_MODE_OUTPUT_PP,          /* PushPull output without internal resistor */
   IO_MODE_OUTPUT_PP_PU,       /* PushPull output with  internal pullup resistor */
   IO_MODE_OUTPUT_PP_PD,       /* PushPull output with  internal pulldown resistor */
   IO_MODE_IT_RISING_EDGE_PU,   /* push up resistor input - irq on rising edge  */
   IO_MODE_IT_RISING_EDGE_PD,   /* push dw resistor input - irq on rising edge  */
   IO_MODE_IT_FALLING_EDGE_PU,  /* push up resistor input - irq on falling edge */
   IO_MODE_IT_FALLING_EDGE_PD,  /* push dw resistor input - irq on falling edge */
   IO_MODE_IT_LOW_LEVEL_PU,     /* push up resistor input - irq detect on low level */
   IO_MODE_IT_LOW_LEVEL_PD,     /* push dw resistor input - irq detect on low level */
   IO_MODE_IT_HIGH_LEVEL_PU,    /* push up resistor input - irq detect on high level */
   IO_MODE_IT_HIGH_LEVEL_PD,    /* push dw resistor input - irq detect on high level */

}IO_ModeTypedef;
/**
  * @}
  */

/** @defgroup STMPE811_Private_Variables
  * @{
  */
extern I2C_HandleTypeDef hi2c1;

/**
  * @}
  */

/** @defgroup STMPE811_Exported_Macros
  * @{
  */
/**
  * @}
  */
/** @defgroup STMPE811_Exported_Functions
  * @{
  */

/**
  * @brief STMPE811 Control functions
  */
static void     stmpe811_Init(uint16_t DeviceAddr);
static void     stmpe811_Reset(uint16_t DeviceAddr);
static uint16_t stmpe811_ReadID(uint16_t DeviceAddr);
static void     stmpe811_EnableGlobalIT(uint16_t DeviceAddr);
static void     stmpe811_DisableGlobalIT(uint16_t DeviceAddr);
static void     stmpe811_EnableITSource(uint16_t DeviceAddr, uint8_t Source);
static void     stmpe811_DisableITSource(uint16_t DeviceAddr, uint8_t Source);
static void     stmpe811_SetITPolarity(uint16_t DeviceAddr, uint8_t Polarity);
static void     stmpe811_SetITType(uint16_t DeviceAddr, uint8_t Type);
static uint8_t  stmpe811_GlobalITStatus(uint16_t DeviceAddr, uint8_t Source);
static uint8_t  stmpe811_ReadGITStatus(uint16_t DeviceAddr, uint8_t Source);
static void     stmpe811_ClearGlobalIT(uint16_t DeviceAddr, uint8_t Source);

/**
  * @brief STMPE811 IO functionalities functions
  */
static void     stmpe811_IO_Start(uint16_t DeviceAddr, uint32_t IO_Pin);
static uint8_t  stmpe811_IO_Config(uint16_t DeviceAddr, uint32_t IO_Pin, IO_ModeTypedef IO_Mode);
static void     stmpe811_IO_InitPin(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t Direction);
static void     stmpe811_IO_EnableAF(uint16_t DeviceAddr, uint32_t IO_Pin);
static void     stmpe811_IO_DisableAF(uint16_t DeviceAddr, uint32_t IO_Pin);
static void     stmpe811_IO_SetEdgeMode(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t Edge);
static void     stmpe811_IO_WritePin(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t PinState);
static uint32_t stmpe811_IO_ReadPin(uint16_t DeviceAddr, uint32_t IO_Pin);
static void     stmpe811_IO_EnableIT(uint16_t DeviceAddr);
static void     stmpe811_IO_DisableIT(uint16_t DeviceAddr);
static void     stmpe811_IO_EnablePinIT(uint16_t DeviceAddr, uint32_t IO_Pin);
static void     stmpe811_IO_DisablePinIT(uint16_t DeviceAddr, uint32_t IO_Pin);
static uint32_t stmpe811_IO_ITStatus(uint16_t DeviceAddr, uint32_t IO_Pin);
static void     stmpe811_IO_ClearIT(uint16_t DeviceAddr, uint32_t IO_Pin);

/**
  * @brief STMPE811 Touch screen functionalities functions
  */
static void     stmpe811_TS_Start(uint16_t DeviceAddr);
static uint8_t  stmpe811_TS_DetectTouch(uint16_t DeviceAddr);
static void     stmpe811_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y);
static void     stmpe811_TS_EnableIT(uint16_t DeviceAddr);
static void     stmpe811_TS_DisableIT(uint16_t DeviceAddr);
static uint8_t  stmpe811_TS_ITStatus (uint16_t DeviceAddr);
static void     stmpe811_TS_ClearIT (uint16_t DeviceAddr);

static void     IOE_Init(void);
static void     IOE_ITConfig (void);
static void     IOE_Delay(uint32_t delay);
static void     IOE_Write(uint8_t addr, uint8_t reg, uint8_t value);
static uint8_t  IOE_Read(uint8_t addr, uint8_t reg);
static uint16_t IOE_ReadMultiple(uint8_t addr, uint8_t reg, uint8_t *buffer, uint16_t length);


typedef enum
{
  IO_PIN_RESET = 0,
  IO_PIN_SET
}IO_PinState;

/** @defgroup IO_Driver_structure  IO Driver structure
  * @{
  */
typedef struct
{
  void       (*Init)(uint16_t);
  uint16_t   (*ReadID)(uint16_t);
  void       (*Reset)(uint16_t);

  void       (*Start)(uint16_t, uint32_t);
  uint8_t    (*Config)(uint16_t, uint32_t, IO_ModeTypedef);
  void       (*WritePin)(uint16_t, uint32_t, uint8_t);
  uint32_t   (*ReadPin)(uint16_t, uint32_t);

  void       (*EnableIT)(uint16_t);
  void       (*DisableIT)(uint16_t);
  uint32_t    (*ITStatus)(uint16_t, uint32_t);
  void       (*ClearIT)(uint16_t, uint32_t);

}IO_DrvTypeDef;


typedef struct
{
  void       (*Init)(uint16_t);
  uint16_t   (*ReadID)(uint16_t);
  void       (*Reset)(uint16_t);
  void       (*Start)(uint16_t);
  uint8_t    (*DetectTouch)(uint16_t);
  void       (*GetXY)(uint16_t, uint16_t*, uint16_t*);
  void       (*EnableIT)(uint16_t);
  void       (*ClearIT)(uint16_t);
  uint8_t    (*GetITStatus)(uint16_t);
  void       (*DisableIT)(uint16_t);
}TS_DrvTypeDef;

static TS_DrvTypeDef *ts_driver;
static uint16_t ts_x_boundary, ts_y_boundary;
static uint8_t  ts_orientation;

TS_StateTypeDef  TS_State;

/* Touch screen driver structure initialization */
TS_DrvTypeDef stmpe811_ts_drv =
{
  stmpe811_Init,
  stmpe811_ReadID,
  stmpe811_Reset,
  stmpe811_TS_Start,
  stmpe811_TS_DetectTouch,
  stmpe811_TS_GetXY,
  stmpe811_TS_EnableIT,
  stmpe811_TS_ClearIT,
  stmpe811_TS_ITStatus,
  stmpe811_TS_DisableIT,
};

/* IO driver structure initialization */
IO_DrvTypeDef stmpe811_io_drv =
{
  stmpe811_Init,
  stmpe811_ReadID,
  stmpe811_Reset,
  stmpe811_IO_Start,
  stmpe811_IO_Config,
  stmpe811_IO_WritePin,
  stmpe811_IO_ReadPin,
  stmpe811_IO_EnableIT,
  stmpe811_IO_DisableIT,
  stmpe811_IO_ITStatus,
  stmpe811_IO_ClearIT,
};

/* stmpe811 instances by address */
uint8_t stmpe811[STMPE811_MAX_INSTANCE] = {0};
/**
  * @}
  */ 

/** @defgroup STMPE811_Private_Function_Prototypes
  * @{
  */
static uint8_t stmpe811_GetInstance(uint16_t DeviceAddr); 
/**
  * @}
  */ 

/** @defgroup STMPE811_Private_Functions
  * @{
  */

/**
  * @brief  IOE delay.
  * @param  Delay: Delay in ms
  * @retval None
  */
static void IOE_Delay(uint32_t Delay)
{
  HAL_Delay(Delay);
}

/**
  * @brief  Manages error callback by re-initializing I2C.
  * @param  Addr: I2C Address
  * @retval None
  */
static void I2Cx_Error(uint8_t Addr)
{
  /* De-initialize the IOE comunication BUS */
  HAL_I2C_DeInit(&hi2c1);

  /* Re-Initiaize the IOE comunication BUS */
  //MX_I2C1_Init();
}

/**
  * @brief  Reads a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Data to be read
  */
static uint8_t I2Cx_Read(uint8_t Addr, uint8_t Reg)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint8_t Value = 0;

  status = HAL_I2C_Mem_Read(&hi2c1, Addr, Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2C_TIMEOUT);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* Execute user timeout callback */
    I2Cx_Error(Addr);
  }

  return Value;
}

/**
  * @brief  Writes a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
static void I2Cx_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Write(&hi2c1, Addr, (uint16_t)Reg, I2C_MEMADD_SIZE_8BIT, &Value, 1, I2C_TIMEOUT);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* I2C error occured */
    I2Cx_Error(Addr);
  }
}

/**
  * @brief  IOE writes single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Value: Data to be written
  * @retval None
  */
void IOE_Write(uint8_t Addr, uint8_t Reg, uint8_t Value)
{
  I2Cx_Write(Addr, Reg, Value);
}

/**
  * @brief  IOE reads single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @retval Read data
  */
uint8_t IOE_Read(uint8_t Addr, uint8_t Reg)
{
  return I2Cx_Read(Addr, Reg);
}

/**
  * @brief  Reads multiple data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
static HAL_StatusTypeDef I2Cx_ReadMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddress, uint8_t *Buffer, uint16_t Length)
{
  HAL_StatusTypeDef status = HAL_OK;

  status = HAL_I2C_Mem_Read(&hi2c1, Addr, (uint16_t)Reg, MemAddress, Buffer, Length, I2C_TIMEOUT);

  /* Check the communication status */
  if(status != HAL_OK)
  {
    /* I2C error occured */
    I2Cx_Error(Addr);
  }
  return status;
}

/**
  * @brief  IOE reads multiple data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address
  * @param  Buffer: Pointer to data buffer
  * @param  Length: Length of the data
  * @retval Number of read data
  */
uint16_t IOE_ReadMultiple(uint8_t Addr, uint8_t Reg, uint8_t *Buffer, uint16_t Length)
{
 return I2Cx_ReadMultiple(Addr, Reg, I2C_MEMADD_SIZE_8BIT, Buffer, Length);
}

/**
  * @brief  Configures I2C Interrupt.
  * @param  None
  * @retval None
  */
static void I2Cx_ITConfig(void)
{

}

/**
  * @brief  Configures IOE low level Interrupt.
  * @param  None
  * @retval None
  */
void IOE_ITConfig(void)
{
  I2Cx_ITConfig();
}

/**
  * @brief  Initializes I2C HAL.
  * @param  None
  * @retval None
  */
static void I2Cx_Init(void)
{
  if(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_RESET)
  {
	  hi2c1.Instance = I2C1;
	  hi2c1.Init.Timing = 0x20404768;
	  hi2c1.Init.OwnAddress1 = 0;
	  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	  hi2c1.Init.OwnAddress2 = 0;
	  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    /* Init the I2C */
	HAL_I2C_MspInit(&hi2c1);
    HAL_I2C_Init(&hi2c1);
  }
}

/**
  * @brief  Initializes IOE low level.
  * @param  None
  * @retval None
  */
static void IOE_Init(void)
{
	//I2Cx_Init();
}

/**
  * @brief  Initialize the stmpe811 and configure the needed hardware resources
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval None
  */
static void stmpe811_Init(uint16_t DeviceAddr)
{
  uint8_t instance;
  uint8_t empty;
  
  /* Check if device instance already exists */
  instance = stmpe811_GetInstance(DeviceAddr);
  
  /* To prevent double initialization */
  if(instance == 0xFF)
  {
    /* Look for empty instance */
    empty = stmpe811_GetInstance(0);
    
    if(empty < STMPE811_MAX_INSTANCE)
    {
      /* Register the current device instance */
      stmpe811[empty] = DeviceAddr;
      
      /* Initialize IO BUS layer */
      IOE_Init(); 
      
      /* Generate stmpe811 Software reset */
      stmpe811_Reset(DeviceAddr);
    }
  }
}
 
/**
  * @brief  Reset the stmpe811 by Software.
  * @param  DeviceAddr: Device address on communication Bus.  
  * @retval None
  */
static void stmpe811_Reset(uint16_t DeviceAddr)
{
  /* Power Down the stmpe811 */  
  IOE_Write(DeviceAddr, STMPE811_REG_SYS_CTRL1, 2);

  /* Wait for a delay to ensure registers erasing */
  IOE_Delay(10); 
  
  /* Power On the Codec after the power off => all registers are reinitialized */
  IOE_Write(DeviceAddr, STMPE811_REG_SYS_CTRL1, 0);
  
  /* Wait for a delay to ensure registers erasing */
  IOE_Delay(2); 
}

/**
  * @brief  Read the stmpe811 IO Expander device ID.
  * @param  DeviceAddr: Device address on communication Bus.  
  * @retval The Device ID (two bytes).
  */
static uint16_t stmpe811_ReadID(uint16_t DeviceAddr)
{
  /* Initialize IO BUS layer */
  IOE_Init(); 
  
  /* Return the device ID value */
  return ((IOE_Read(DeviceAddr, STMPE811_REG_CHP_ID_LSB) << 8) |\
          (IOE_Read(DeviceAddr, STMPE811_REG_CHP_ID_MSB)));
}

/**
  * @brief  Enable the Global interrupt.
  * @param  DeviceAddr: Device address on communication Bus.       
  * @retval None
  */
static void stmpe811_EnableGlobalIT(uint16_t DeviceAddr)
{
  uint8_t tmp = 0;
  
  /* Read the Interrupt Control register  */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_INT_CTRL);
  
  /* Set the global interrupts to be Enabled */    
  tmp |= (uint8_t)STMPE811_GIT_EN;
  
  /* Write Back the Interrupt Control register */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_CTRL, tmp); 
}

/**
  * @brief  Disable the Global interrupt.
  * @param  DeviceAddr: Device address on communication Bus.      
  * @retval None
  */
static void stmpe811_DisableGlobalIT(uint16_t DeviceAddr)
{
  uint8_t tmp = 0;
  
  /* Read the Interrupt Control register  */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_INT_CTRL);

  /* Set the global interrupts to be Disabled */    
  tmp &= ~(uint8_t)STMPE811_GIT_EN;
 
  /* Write Back the Interrupt Control register */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_CTRL, tmp);
    
}

/**
  * @brief  Enable the interrupt mode for the selected IT source
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param Source: The interrupt source to be configured, could be:
  *   @arg  STMPE811_GIT_IO: IO interrupt 
  *   @arg  STMPE811_GIT_ADC : ADC interrupt    
  *   @arg  STMPE811_GIT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  STMPE811_GIT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  STMPE811_GIT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  STMPE811_GIT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  STMPE811_GIT_TOUCH : Touch Screen Controller Touch Detected interrupt  
  * @retval None
  */
static void stmpe811_EnableITSource(uint16_t DeviceAddr, uint8_t Source)
{
  uint8_t tmp = 0;
  
  /* Get the current value of the INT_EN register */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_INT_EN);

  /* Set the interrupts to be Enabled */    
  tmp |= Source; 
  
  /* Set the register */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_EN, tmp);   
}

/**
  * @brief  Disable the interrupt mode for the selected IT source
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param  Source: The interrupt source to be configured, could be:
  *   @arg  STMPE811_GIT_IO: IO interrupt 
  *   @arg  STMPE811_GIT_ADC : ADC interrupt    
  *   @arg  STMPE811_GIT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  STMPE811_GIT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  STMPE811_GIT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  STMPE811_GIT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  STMPE811_GIT_TOUCH : Touch Screen Controller Touch Detected interrupt  
  * @retval None
  */
static void stmpe811_DisableITSource(uint16_t DeviceAddr, uint8_t Source)
{
  uint8_t tmp = 0;
  
  /* Get the current value of the INT_EN register */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_INT_EN);

  /* Set the interrupts to be Enabled */    
  tmp &= ~Source; 
  
  /* Set the register */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_EN, tmp);   
}

/**
  * @brief  Set the global interrupt Polarity.
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param  Polarity: the IT mode polarity, could be one of the following values:
  *   @arg  STMPE811_POLARITY_LOW: Interrupt line is active Low/Falling edge      
  *   @arg  STMPE811_POLARITY_HIGH: Interrupt line is active High/Rising edge              
  * @retval None
  */
static void stmpe811_SetITPolarity(uint16_t DeviceAddr, uint8_t Polarity)
{
  uint8_t tmp = 0;
  
  /* Get the current register value */ 
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_INT_CTRL);
  
  /* Mask the polarity bits */
  tmp &= ~(uint8_t)0x04;
    
  /* Modify the Interrupt Output line configuration */
  tmp |= Polarity;
  
  /* Set the new register value */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_CTRL, tmp);
 
}

/**
  * @brief  Set the global interrupt Type. 
  * @param  DeviceAddr: Device address on communication Bus.      
  * @param  Type: Interrupt line activity type, could be one of the following values:
  *   @arg  STMPE811_TYPE_LEVEL: Interrupt line is active in level model         
  *   @arg  STMPE811_TYPE_EDGE: Interrupt line is active in edge model           
  * @retval None
  */
static void stmpe811_SetITType(uint16_t DeviceAddr, uint8_t Type)
{
  uint8_t tmp = 0;
  
  /* Get the current register value */ 
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_INT_CTRL);
  
  /* Mask the type bits */
  tmp &= ~(uint8_t)0x02;
    
  /* Modify the Interrupt Output line configuration */
  tmp |= Type;
  
  /* Set the new register value */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_CTRL, tmp);
 
}

/**
  * @brief  Check the selected Global interrupt source pending bit
  * @param  DeviceAddr: Device address on communication Bus. 
  * @param  Source: the Global interrupt source to be checked, could be:
  *   @arg  STMPE811_GIT_IO: IO interrupt 
  *   @arg  STMPE811_GIT_ADC : ADC interrupt    
  *   @arg  STMPE811_GIT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  STMPE811_GIT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  STMPE811_GIT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  STMPE811_GIT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  STMPE811_GIT_TOUCH : Touch Screen Controller Touch Detected interrupt      
  * @retval The checked Global interrupt source status.
  */
static uint8_t stmpe811_GlobalITStatus(uint16_t DeviceAddr, uint8_t Source)
{
  /* Return the global IT source status */
  return((IOE_Read(DeviceAddr, STMPE811_REG_INT_STA) & Source) == Source);
}

/**
  * @brief  Return the Global interrupts status
  * @param  DeviceAddr: Device address on communication Bus. 
  * @param  Source: the Global interrupt source to be checked, could be:
  *   @arg  STMPE811_GIT_IO: IO interrupt 
  *   @arg  STMPE811_GIT_ADC : ADC interrupt    
  *   @arg  STMPE811_GIT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  STMPE811_GIT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  STMPE811_GIT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  STMPE811_GIT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  STMPE811_GIT_TOUCH : Touch Screen Controller Touch Detected interrupt      
  * @retval The checked Global interrupt source status.
  */
static uint8_t stmpe811_ReadGITStatus(uint16_t DeviceAddr, uint8_t Source)
{
  /* Return the global IT source status */
  return((IOE_Read(DeviceAddr, STMPE811_REG_INT_STA) & Source));
}

/**
  * @brief  Clear the selected Global interrupt pending bit(s)
  * @param  DeviceAddr: Device address on communication Bus. 
  * @param  Source: the Global interrupt source to be cleared, could be any combination
  *         of the following values:        
  *   @arg  STMPE811_GIT_IO: IO interrupt 
  *   @arg  STMPE811_GIT_ADC : ADC interrupt    
  *   @arg  STMPE811_GIT_FE : Touch Screen Controller FIFO Error interrupt
  *   @arg  STMPE811_GIT_FF : Touch Screen Controller FIFO Full interrupt      
  *   @arg  STMPE811_GIT_FOV : Touch Screen Controller FIFO Overrun interrupt     
  *   @arg  STMPE811_GIT_FTH : Touch Screen Controller FIFO Threshold interrupt   
  *   @arg  STMPE811_GIT_TOUCH : Touch Screen Controller Touch Detected interrupt 
  * @retval None
  */
static void stmpe811_ClearGlobalIT(uint16_t DeviceAddr, uint8_t Source)
{
  /* Write 1 to the bits that have to be cleared */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_STA, Source);
}

/**
  * @brief  Start the IO functionality use and disable the AF for selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param  IO_Pin: The IO pin(s) to put in AF. This parameter can be one 
  *         of the following values:
  *   @arg  STMPE811_PIN_x: where x can be from 0 to 7.
  * @retval None
  */
static void stmpe811_IO_Start(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  uint8_t mode;
  
  /* Get the current register value */
  mode = IOE_Read(DeviceAddr, STMPE811_REG_SYS_CTRL2);
  
  /* Set the Functionalities to be Disabled */    
  mode &= ~(STMPE811_IO_FCT | STMPE811_ADC_FCT);  
  
  /* Write the new register value */  
  IOE_Write(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode); 

  /* Disable AF for the selected IO pin(s) */
  stmpe811_IO_DisableAF(DeviceAddr, (uint8_t)IO_Pin);
}

/**
  * @brief  Configures the IO pin(s) according to IO mode structure value.
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param  IO_Pin: The output pin to be set or reset. This parameter can be one 
  *         of the following values:   
  *   @arg  STMPE811_PIN_x: where x can be from 0 to 7.
  * @param  IO_Mode: The IO pin mode to configure, could be one of the following values:
  *   @arg  IO_MODE_INPUT
  *   @arg  IO_MODE_OUTPUT
  *   @arg  IO_MODE_IT_RISING_EDGE
  *   @arg  IO_MODE_IT_FALLING_EDGE
  *   @arg  IO_MODE_IT_LOW_LEVEL
  *   @arg  IO_MODE_IT_HIGH_LEVEL            
  * @retval 0 if no error, IO_Mode if error
  */
static uint8_t stmpe811_IO_Config(uint16_t DeviceAddr, uint32_t IO_Pin, IO_ModeTypedef IO_Mode)
{
  uint8_t error_code = 0;

  /* Configure IO pin according to selected IO mode */
  switch(IO_Mode)
  {
  case IO_MODE_INPUT: /* Input mode */
    stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN);
    break;
    
  case IO_MODE_OUTPUT: /* Output mode */
    stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_OUT);
    break;
  
  case IO_MODE_IT_RISING_EDGE: /* Interrupt rising edge mode */
    stmpe811_IO_EnableIT(DeviceAddr);
    stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
    stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN); 
    stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_EDGE);      
    stmpe811_IO_SetEdgeMode(DeviceAddr, IO_Pin, STMPE811_EDGE_RISING); 
    break;
  
  case IO_MODE_IT_FALLING_EDGE: /* Interrupt falling edge mode */
    stmpe811_IO_EnableIT(DeviceAddr);
    stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
    stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN); 
    stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_EDGE);    
    stmpe811_IO_SetEdgeMode(DeviceAddr, IO_Pin, STMPE811_EDGE_FALLING); 
    break;
  
  case IO_MODE_IT_LOW_LEVEL: /* Low level interrupt mode */
    stmpe811_IO_EnableIT(DeviceAddr);
    stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
    stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN); 
    stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_LEVEL);
    stmpe811_SetITPolarity(DeviceAddr, STMPE811_POLARITY_LOW);      
    break;
    
  case IO_MODE_IT_HIGH_LEVEL: /* High level interrupt mode */
    stmpe811_IO_EnableIT(DeviceAddr);
    stmpe811_IO_EnablePinIT(DeviceAddr, IO_Pin);
    stmpe811_IO_InitPin(DeviceAddr, IO_Pin, STMPE811_DIRECTION_IN); 
    stmpe811_SetITType(DeviceAddr, STMPE811_TYPE_LEVEL);
    stmpe811_SetITPolarity(DeviceAddr, STMPE811_POLARITY_HIGH);  
    break;    

  default:
    error_code = (uint8_t) IO_Mode;
    break;
  } 
  return error_code;
}

/**
  * @brief  Initialize the selected IO pin direction.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: The IO pin to be configured. This parameter could be any 
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: Where x can be from 0 to 7.   
  * @param  Direction: could be STMPE811_DIRECTION_IN or STMPE811_DIRECTION_OUT.      
  * @retval None
  */
static void stmpe811_IO_InitPin(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t Direction)
{
  uint8_t tmp = 0;   
  
  /* Get all the Pins direction */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_IO_DIR);
  
  /* Set the selected pin direction */
  if (Direction != STMPE811_DIRECTION_IN)
  {
    tmp |= (uint8_t)IO_Pin;
  }  
  else 
  {
    tmp &= ~(uint8_t)IO_Pin;
  }
  
  /* Write the register new value */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_DIR, tmp);   
}

/**
  * @brief  Disable the AF for the selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param  IO_Pin: The IO pin to be configured. This parameter could be any 
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: Where x can be from 0 to 7.        
  * @retval None
  */
static void stmpe811_IO_DisableAF(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  uint8_t tmp = 0;
  
  /* Get the current state of the IO_AF register */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_IO_AF);

  /* Enable the selected pins alternate function */
  tmp |= (uint8_t)IO_Pin;

  /* Write back the new value in IO AF register */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_AF, tmp);
  
}

/**
  * @brief  Enable the AF for the selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param  IO_Pin: The IO pin to be configured. This parameter could be any 
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: Where x can be from 0 to 7.       
  * @retval None
  */
static void stmpe811_IO_EnableAF(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  uint8_t tmp = 0;
  
  /* Get the current register value */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_IO_AF);

  /* Enable the selected pins alternate function */   
  tmp &= ~(uint8_t)IO_Pin;   
  
  /* Write back the new register value */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_AF, tmp); 
}

/**
  * @brief  Configure the Edge for which a transition is detectable for the
  *         selected pin.
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: The IO pin to be configured. This parameter could be any 
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: Where x can be from 0 to 7.  
  * @param  Edge: The edge which will be detected. This parameter can be one or
  *         a combination of following values: STMPE811_EDGE_FALLING and STMPE811_EDGE_RISING .
  * @retval None
  */
static void stmpe811_IO_SetEdgeMode(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t Edge)
{
  uint8_t tmp1 = 0, tmp2 = 0;   
  
  /* Get the current registers values */
  tmp1 = IOE_Read(DeviceAddr, STMPE811_REG_IO_FE);
  tmp2 = IOE_Read(DeviceAddr, STMPE811_REG_IO_RE);

  /* Disable the Falling Edge */
  tmp1 &= ~(uint8_t)IO_Pin;
  
  /* Disable the Falling Edge */
  tmp2 &= ~(uint8_t)IO_Pin;

  /* Enable the Falling edge if selected */
  if (Edge & STMPE811_EDGE_FALLING)
  {
    tmp1 |= (uint8_t)IO_Pin;
  }

  /* Enable the Rising edge if selected */
  if (Edge & STMPE811_EDGE_RISING)
  {
    tmp2 |= (uint8_t)IO_Pin;
  }

  /* Write back the new registers values */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_FE, tmp1);
  IOE_Write(DeviceAddr, STMPE811_REG_IO_RE, tmp2);
}

/**
  * @brief  Write a new IO pin state.
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param IO_Pin: The output pin to be set or reset. This parameter can be one 
  *        of the following values:
  *   @arg  STMPE811_PIN_x: where x can be from 0 to 7. 
  * @param PinState: The new IO pin state.
  * @retval None
  */
static void stmpe811_IO_WritePin(uint16_t DeviceAddr, uint32_t IO_Pin, uint8_t PinState)
{
  /* Apply the bit value to the selected pin */
  if (PinState != 0)
  {
    /* Set the register */
    IOE_Write(DeviceAddr, STMPE811_REG_IO_SET_PIN, (uint8_t)IO_Pin);
  }
  else
  {
    /* Set the register */
    IOE_Write(DeviceAddr, STMPE811_REG_IO_CLR_PIN, (uint8_t)IO_Pin);
  } 
}

/**
  * @brief  Return the state of the selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.  
  * @param IO_Pin: The output pin to be set or reset. This parameter can be one 
  *        of the following values:
  *   @arg  STMPE811_PIN_x: where x can be from 0 to 7. 
  * @retval IO pin(s) state.
  */
static uint32_t stmpe811_IO_ReadPin(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  return((uint32_t)(IOE_Read(DeviceAddr, STMPE811_REG_IO_MP_STA) & (uint8_t)IO_Pin));
}

/**
  * @brief  Enable the global IO interrupt source.
  * @param  DeviceAddr: Device address on communication Bus.  
  * @retval None
  */
static void stmpe811_IO_EnableIT(uint16_t DeviceAddr)
{ 
  IOE_ITConfig();
  
  /* Enable global IO IT source */
  stmpe811_EnableITSource(DeviceAddr, STMPE811_GIT_IO);
  
  /* Enable global interrupt */
  stmpe811_EnableGlobalIT(DeviceAddr); 
}

/**
  * @brief  Disable the global IO interrupt source.
  * @param  DeviceAddr: Device address on communication Bus.   
  * @retval None
  */
static void stmpe811_IO_DisableIT(uint16_t DeviceAddr)
{
  /* Disable the global interrupt */
  stmpe811_DisableGlobalIT(DeviceAddr);
  
  /* Disable global IO IT source */
  stmpe811_DisableITSource(DeviceAddr, STMPE811_GIT_IO);    
}

/**
  * @brief  Enable interrupt mode for the selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: The IO interrupt to be enabled. This parameter could be any 
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: where x can be from 0 to 7.
  * @retval None
  */
static void stmpe811_IO_EnablePinIT(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  uint8_t tmp = 0;
  
  /* Get the IO interrupt state */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_IO_INT_EN);
  
  /* Set the interrupts to be enabled */    
  tmp |= (uint8_t)IO_Pin;
  
  /* Write the register new value */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_INT_EN, tmp);  
}

/**
  * @brief  Disable interrupt mode for the selected IO pin(s).
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: The IO interrupt to be disabled. This parameter could be any 
  *         combination of the following values:
  *   @arg  STMPE811_PIN_x: where x can be from 0 to 7.
  * @retval None
  */
static void stmpe811_IO_DisablePinIT(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  uint8_t tmp = 0;
  
  /* Get the IO interrupt state */
  tmp = IOE_Read(DeviceAddr, STMPE811_REG_IO_INT_EN);
  
  /* Set the interrupts to be Disabled */    
  tmp &= ~(uint8_t)IO_Pin;
  
  /* Write the register new value */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_INT_EN, tmp);   
}

/**
  * @brief  Check the status of the selected IO interrupt pending bit
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: The IO interrupt to be checked could be:
  *   @arg  STMPE811_PIN_x Where x can be from 0 to 7.             
  * @retval Status of the checked IO pin(s).
  */
static uint32_t stmpe811_IO_ITStatus(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  /* Get the Interrupt status */
  return(IOE_Read(DeviceAddr, STMPE811_REG_IO_INT_STA) & (uint8_t)IO_Pin); 
}

/**
  * @brief  Clear the selected IO interrupt pending bit(s).
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  IO_Pin: the IO interrupt to be cleared, could be:
  *   @arg  STMPE811_PIN_x: Where x can be from 0 to 7.            
  * @retval None
  */
static void stmpe811_IO_ClearIT(uint16_t DeviceAddr, uint32_t IO_Pin)
{
  /* Clear the global IO IT pending bit */
  stmpe811_ClearGlobalIT(DeviceAddr, STMPE811_GIT_IO);
  
  /* Clear the IO IT pending bit(s) */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_INT_STA, (uint8_t)IO_Pin);  
  
  /* Clear the Edge detection pending bit*/
  IOE_Write(DeviceAddr, STMPE811_REG_IO_ED, (uint8_t)IO_Pin);
  
  /* Clear the Rising edge pending bit */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_RE, (uint8_t)IO_Pin);
  
  /* Clear the Falling edge pending bit */
  IOE_Write(DeviceAddr, STMPE811_REG_IO_FE, (uint8_t)IO_Pin); 
}

/**
  * @brief  Configures the touch Screen Controller (Single point detection)
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval None.
  */
static void stmpe811_TS_Start(uint16_t DeviceAddr)
{
  uint8_t mode;
  
  /* Get the current register value */
  mode = IOE_Read(DeviceAddr, STMPE811_REG_SYS_CTRL2);
  
  /* Set the Functionalities to be Enabled */    
  mode &= ~(STMPE811_IO_FCT);  
  
  /* Write the new register value */  
  IOE_Write(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode); 

  /* Select TSC pins in TSC alternate mode */  
  stmpe811_IO_EnableAF(DeviceAddr, STMPE811_TOUCH_IO_ALL);
  
  /* Set the Functionalities to be Enabled */    
  mode &= ~(STMPE811_TS_FCT | STMPE811_ADC_FCT);  
  
  /* Set the new register value */  
  IOE_Write(DeviceAddr, STMPE811_REG_SYS_CTRL2, mode); 
  
  /* Select Sample Time, bit number and ADC Reference */
  IOE_Write(DeviceAddr, STMPE811_REG_ADC_CTRL1, 0x49);
  
  /* Wait for 2 ms */
  IOE_Delay(2); 
  
  /* Select the ADC clock speed: 3.25 MHz */
  IOE_Write(DeviceAddr, STMPE811_REG_ADC_CTRL2, 0x01);
  
  /* Select 2 nF filter capacitor */
  /* Configuration: 
     - Touch average control    : 4 samples
     - Touch delay time         : 500 uS
     - Panel driver setting time: 500 uS 
  */
  IOE_Write(DeviceAddr, STMPE811_REG_TSC_CFG, 0x9A); 
  
  /* Configure the Touch FIFO threshold: single point reading */
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_TH, 0x01);
  
  /* Clear the FIFO memory content. */
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
  
  /* Put the FIFO back into operation mode  */
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
  
  /* Set the range and accuracy pf the pressure measurement (Z) : 
     - Fractional part :7 
     - Whole part      :1 
  */
  IOE_Write(DeviceAddr, STMPE811_REG_TSC_FRACT_XYZ, 0x01);
  
  /* Set the driving capability (limit) of the device for TSC pins: 50mA */
  IOE_Write(DeviceAddr, STMPE811_REG_TSC_I_DRIVE, 0x01);
  
  /* Touch screen control configuration (enable TSC):
     - No window tracking index
     - XYZ acquisition mode
   */
  IOE_Write(DeviceAddr, STMPE811_REG_TSC_CTRL, 0x01);
  
  /*  Clear all the status pending bits if any */
  IOE_Write(DeviceAddr, STMPE811_REG_INT_STA, 0xFF);

  /* Wait for 2 ms delay */
  IOE_Delay(2); 
}

/**
  * @brief  Return if there is touch detected or not.
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval Touch detected state.
  */
static uint8_t stmpe811_TS_DetectTouch(uint16_t DeviceAddr)
{
  uint8_t state;
  uint8_t ret = 0;
  
  state = ((IOE_Read(DeviceAddr, STMPE811_REG_TSC_CTRL) & (uint8_t)STMPE811_TS_CTRL_STATUS) == (uint8_t)0x80);
  
  if(state > 0)
  {
    if(IOE_Read(DeviceAddr, STMPE811_REG_FIFO_SIZE) > 0)
    {
      ret = 1;
    }
  }
  else
  {
    /* Reset FIFO */
    IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
    /* Enable the FIFO again */
    IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
  }
  
  return ret;
}

/**
  * @brief  Get the touch screen X and Y positions values
  * @param  DeviceAddr: Device address on communication Bus.
  * @param  X: Pointer to X position value
  * @param  Y: Pointer to Y position value   
  * @retval None.
  */
static void stmpe811_TS_GetXY(uint16_t DeviceAddr, uint16_t *X, uint16_t *Y)
{
  uint8_t  dataXYZ[4];
  uint32_t uldataXYZ;
  
  IOE_ReadMultiple(DeviceAddr, STMPE811_REG_TSC_DATA_NON_INC, dataXYZ, sizeof(dataXYZ)) ;
  
  /* Calculate positions values */
  uldataXYZ = (dataXYZ[0] << 24)|(dataXYZ[1] << 16)|(dataXYZ[2] << 8)|(dataXYZ[3] << 0);     
  *X = (uldataXYZ >> 20) & 0x00000FFF;     
  *Y = (uldataXYZ >>  8) & 0x00000FFF;     
  
  /* Reset FIFO */
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
  /* Enable the FIFO again */
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
}

/**
  * @brief  Configure the selected source to generate a global interrupt or not
  * @param  DeviceAddr: Device address on communication Bus.  
  * @retval None
  */
static void stmpe811_TS_EnableIT(uint16_t DeviceAddr)
{
  IOE_ITConfig();
  
  /* Enable global TS IT source */
  stmpe811_EnableITSource(DeviceAddr, STMPE811_TS_IT); 
  
  /* Enable global interrupt */
  stmpe811_EnableGlobalIT(DeviceAddr);
}

/**
  * @brief  Configure the selected source to generate a global interrupt or not
  * @param  DeviceAddr: Device address on communication Bus.    
  * @retval None
  */
static void stmpe811_TS_DisableIT(uint16_t DeviceAddr)
{
  /* Disable global interrupt */
  stmpe811_DisableGlobalIT(DeviceAddr);
  
  /* Disable global TS IT source */
  stmpe811_DisableITSource(DeviceAddr, STMPE811_TS_IT); 
}

/**
  * @brief  Configure the selected source to generate a global interrupt or not
  * @param  DeviceAddr: Device address on communication Bus.    
  * @retval TS interrupts status
  */
static uint8_t stmpe811_TS_ITStatus(uint16_t DeviceAddr)
{
  /* Return TS interrupts status */
  return(stmpe811_ReadGITStatus(DeviceAddr, STMPE811_TS_IT));
}

/**
  * @brief  Configure the selected source to generate a global interrupt or not
  * @param  DeviceAddr: Device address on communication Bus.  
  * @retval None
  */
static void stmpe811_TS_ClearIT(uint16_t DeviceAddr)
{
  /* Clear the global TS IT source */
  stmpe811_ClearGlobalIT(DeviceAddr, STMPE811_TS_IT);
}

/**
  * @brief  Check if the device instance of the selected address is already registered
  *         and return its index  
  * @param  DeviceAddr: Device address on communication Bus.
  * @retval Index of the device instance if registered, 0xFF if not.
  */
static uint8_t stmpe811_GetInstance(uint16_t DeviceAddr)
{
  uint8_t idx = 0;
  
  /* Check all the registered instances */
  for(idx = 0; idx < STMPE811_MAX_INSTANCE ; idx ++)
  {
    if(stmpe811[idx] == DeviceAddr)
    {
      return idx; 
    }
  }
  
  return 0xFF;
}

/**
  * @brief  Initializes and configures the touch screen functionalities and
  *         configures all necessary hardware resources (GPIOs, clocks..).
  * @param  xSize: Maximum X size of the TS area on LCD
  *         ySize: Maximum Y size of the TS area on LCD
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_Init(uint16_t xSize, uint16_t ySize)
{
  uint8_t ret = TS_ERROR;

  if(stmpe811_ts_drv.ReadID(TS_I2C_ADDRESS) == STMPE811_ID)
  {
    /* Initialize the TS driver structure */
    ts_driver = &stmpe811_ts_drv;

    /* Initialize x and y positions boundaries */
    ts_x_boundary  = xSize;
    ts_y_boundary  = ySize;
    ts_orientation = TS_SWAP_Y;
    ret = TS_OK;
  }

  if(ret == TS_OK)
  {
    /* Initialize the LL TS Driver */
    ts_driver->Init(TS_I2C_ADDRESS);
    ts_driver->Start(TS_I2C_ADDRESS);
    ts_driver->ClearIT(TS_I2C_ADDRESS);
    ts_driver->EnableIT(TS_I2C_ADDRESS);
  }

  return ret;
}

/**
  * @brief  Returns status and positions of the touch screen.
  * @param  TS_State: Pointer to touch screen current state structure
  * @retval TS_OK if all initializations are OK. Other value if error.
  */
uint8_t BSP_TS_GetState(TS_StateTypeDef *TS_State)
{
  static uint32_t _x = 0, _y = 0, x_p=0, y_p=0;
  uint16_t xDiff, yDiff , x , y;
  uint16_t swap;

  TS_State->TouchDetected = ts_driver->DetectTouch(TS_I2C_ADDRESS);

  if(TS_State->TouchDetected)
  {
    ts_driver->GetXY(TS_I2C_ADDRESS, &x, &y);

    if(ts_orientation & TS_SWAP_X)
    {
      x = 4096 - x;
    }

    if(ts_orientation & TS_SWAP_Y)
    {
      y = 4096 - y;
    }

    if(ts_orientation & TS_SWAP_XY)
    {
      swap = y;
      y = x;
      x = swap;
    }

    _x = (ts_x_boundary * x) >> 12;
    _y = (ts_y_boundary * y) >> 12;

    xDiff = x_p > _x? (x_p - _x): (_x - x_p);
    yDiff = y_p > _y? (y_p - _y): (_y - y_p);

    if (xDiff + yDiff > TOUCH_RESOLUTION)
    {
      x_p = _x;
      y_p = _y;
    }

    TS_State->x = x_p;
    TS_State->y = y_p;
  }

  return TS_OK;
}

void BSP_TS_ClearIT(void)
{
    ts_driver = &stmpe811_ts_drv;
    ts_driver->ClearIT(TS_I2C_ADDRESS);
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
