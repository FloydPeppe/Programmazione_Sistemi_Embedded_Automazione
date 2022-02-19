################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_f32.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q15.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q31.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q7.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_f32.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q15.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q31.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q7.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.c \
../Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.c 

OBJS += \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_f32.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q15.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q31.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q7.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_f32.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q15.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q31.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q7.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.o \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.o 

C_DEPS += \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_f32.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q15.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q31.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q7.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_f32.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q15.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q31.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q7.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.d \
./Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_f32.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_f32.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q15.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q15.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q31.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q31.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q7.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q7.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_copy_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_f32.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_f32.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q15.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q15.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q31.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q31.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q7.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q7.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_fill_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_float_to_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_float.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q15_to_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_float.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q31_to_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_float.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.o: ../Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.c Drivers/DSP_Lib/Source/SupportFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/SupportFunctions/arm_q7_to_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

