################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q7.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q15.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q31.c \
../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q7.c 

OBJS += \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q7.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q15.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q31.o \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q7.o 

C_DEPS += \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q7.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q15.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q31.d \
./Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q7.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_abs_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_add_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_dot_prod_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_mult_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_negate_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_offset_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_scale_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_shift_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_f32.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q15.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q15.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q15.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q31.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q31.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q31.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q7.o: ../Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q7.c Drivers/DSP_Lib/Source/BasicMathFunctions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/DSP_Lib/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/DSP_Lib/Source/BasicMathFunctions/arm_sub_q7.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

