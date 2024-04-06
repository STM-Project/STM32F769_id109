################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/LCD/Touch/stmpe811.c \
../Application/LCD/Touch/touch.c 

OBJS += \
./Application/LCD/Touch/stmpe811.o \
./Application/LCD/Touch/touch.o 

C_DEPS += \
./Application/LCD/Touch/stmpe811.d \
./Application/LCD/Touch/touch.d 


# Each subdirectory must supply rules for building sources it contributes
Application/LCD/Touch/%.o Application/LCD/Touch/%.su Application/LCD/Touch/%.cyclo: ../Application/LCD/Touch/%.c Application/LCD/Touch/subdir.mk
	arm-none-eabi-gcc -c "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"C:/Projekty_id109/Stm32F769bit6/Application" -I"C:/Projekty_id109/Stm32F769bit6/Application/General_Utilities" -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -I"C:/Projekty_id109/Stm32F769bit6/Application/LCD" -I"C:/Projekty_id109/Stm32F769bit6/Application/LCD/Screens" -I"C:/Projekty_id109/Stm32F769bit6/Application/Lang" -I"C:/Projekty_id109/Stm32F769bit6/Application/LCD/Touch" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-LCD-2f-Touch

clean-Application-2f-LCD-2f-Touch:
	-$(RM) ./Application/LCD/Touch/stmpe811.cyclo ./Application/LCD/Touch/stmpe811.d ./Application/LCD/Touch/stmpe811.o ./Application/LCD/Touch/stmpe811.su ./Application/LCD/Touch/touch.cyclo ./Application/LCD/Touch/touch.d ./Application/LCD/Touch/touch.o ./Application/LCD/Touch/touch.su

.PHONY: clean-Application-2f-LCD-2f-Touch

