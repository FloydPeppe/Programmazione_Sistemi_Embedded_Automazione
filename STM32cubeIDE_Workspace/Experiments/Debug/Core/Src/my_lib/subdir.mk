################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/my_lib/datastream.c \
../Core/Src/my_lib/datastream_utils.c \
../Core/Src/my_lib/differential_drive.c \
../Core/Src/my_lib/digital_filters.c \
../Core/Src/my_lib/feedforward_controller.c \
../Core/Src/my_lib/pid_controller.c \
../Core/Src/my_lib/servo.c \
../Core/Src/my_lib/ultrasonic_sensor.c 

OBJS += \
./Core/Src/my_lib/datastream.o \
./Core/Src/my_lib/datastream_utils.o \
./Core/Src/my_lib/differential_drive.o \
./Core/Src/my_lib/digital_filters.o \
./Core/Src/my_lib/feedforward_controller.o \
./Core/Src/my_lib/pid_controller.o \
./Core/Src/my_lib/servo.o \
./Core/Src/my_lib/ultrasonic_sensor.o 

C_DEPS += \
./Core/Src/my_lib/datastream.d \
./Core/Src/my_lib/datastream_utils.d \
./Core/Src/my_lib/differential_drive.d \
./Core/Src/my_lib/digital_filters.d \
./Core/Src/my_lib/feedforward_controller.d \
./Core/Src/my_lib/pid_controller.d \
./Core/Src/my_lib/servo.d \
./Core/Src/my_lib/ultrasonic_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/my_lib/%.o: ../Core/Src/my_lib/%.c Core/Src/my_lib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -DDIFFDRIVE_RTOS_V_ -c -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Drivers/CMSIS/DSP/Include -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Include -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-my_lib

clean-Core-2f-Src-2f-my_lib:
	-$(RM) ./Core/Src/my_lib/datastream.d ./Core/Src/my_lib/datastream.o ./Core/Src/my_lib/datastream_utils.d ./Core/Src/my_lib/datastream_utils.o ./Core/Src/my_lib/differential_drive.d ./Core/Src/my_lib/differential_drive.o ./Core/Src/my_lib/digital_filters.d ./Core/Src/my_lib/digital_filters.o ./Core/Src/my_lib/feedforward_controller.d ./Core/Src/my_lib/feedforward_controller.o ./Core/Src/my_lib/pid_controller.d ./Core/Src/my_lib/pid_controller.o ./Core/Src/my_lib/servo.d ./Core/Src/my_lib/servo.o ./Core/Src/my_lib/ultrasonic_sensor.d ./Core/Src/my_lib/ultrasonic_sensor.o

.PHONY: clean-Core-2f-Src-2f-my_lib

