################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/LCD/LCD_BasicGaphics.c \
../Application/LCD/LCD_Common.c \
../Application/LCD/LCD_Hardware.c \
../Application/LCD/LCD_fonts_images.c 

OBJS += \
./Application/LCD/LCD_BasicGaphics.o \
./Application/LCD/LCD_Common.o \
./Application/LCD/LCD_Hardware.o \
./Application/LCD/LCD_fonts_images.o 

C_DEPS += \
./Application/LCD/LCD_BasicGaphics.d \
./Application/LCD/LCD_Common.d \
./Application/LCD/LCD_Hardware.d \
./Application/LCD/LCD_fonts_images.d 


# Each subdirectory must supply rules for building sources it contributes
Application/LCD/%.o Application/LCD/%.su: ../Application/LCD/%.c Application/LCD/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"C:/Projekty_id109/Stm32F769bit6/Application" -I"C:/Projekty_id109/Stm32F769bit6/Application/General_Utilities" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Projekty_id109/Stm32F769bit6/Application/LCD" -I"C:/Projekty_id109/Stm32F769bit6/Application/LCD/Screens" -I"C:/Projekty_id109/Stm32F769bit6/Application/Lang" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-LCD

clean-Application-2f-LCD:
	-$(RM) ./Application/LCD/LCD_BasicGaphics.d ./Application/LCD/LCD_BasicGaphics.o ./Application/LCD/LCD_BasicGaphics.su ./Application/LCD/LCD_Common.d ./Application/LCD/LCD_Common.o ./Application/LCD/LCD_Common.su ./Application/LCD/LCD_Hardware.d ./Application/LCD/LCD_Hardware.o ./Application/LCD/LCD_Hardware.su ./Application/LCD/LCD_fonts_images.d ./Application/LCD/LCD_fonts_images.o ./Application/LCD/LCD_fonts_images.su

.PHONY: clean-Application-2f-LCD

