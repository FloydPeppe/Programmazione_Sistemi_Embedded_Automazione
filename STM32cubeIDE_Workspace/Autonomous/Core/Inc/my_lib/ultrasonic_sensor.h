/*
 * ultrasonic_sensor.h
 *
 *  Created on: Sep 30, 2021
 *      Author: peppe
 */

#ifndef INC_MY_LIB_ULTRASONIC_SENSOR_H_
#define INC_MY_LIB_ULTRASONIC_SENSOR_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>

// Max sensor distance (in centimeters)
#define ULTRASONIC_MAX_DISTANCE 400.0f // 4m
// Min sensor distance (in centimeters)
#define ULTRASONIC_MIN_DISTANCE 3.0f   // 3cm
// Sound speed on air is about 0.0343 cm/us (us := microseconds)
#define SOUND_SPEED_cm_us 0.0343f

// Max number of simultaneous connected ultrasonic sensors
#define MAX_SENSORS 4

// Macros to get measured value
#define __ULTRASONIC_GET_DISTANCE(__handle__) (__handle__)->distance


// Types definition

typedef enum _ULTRASONIC_StatusTypeDef{
	ULTRASONIC_OK=      0x00U,
	ULTRASONIC_ERROR=   0x01U
}ULTRASONIC_StatusTypeDef;

typedef struct _ULTRASONIC_Flag_Typedef{
	bool timeout;
	bool input_captured;
} ULTRASONIC_Flag_Typedef;


typedef struct _ULTRASONIC_IO_TypeDef{
	GPIO_TypeDef* trig_GPIOx;
	uint32_t      trig_Pin;
	GPIO_TypeDef* echo_GPIOx;
	uint32_t      echo_Pin;
	uint32_t      echo_IC_tim_channel;
} ULTRASONIC_IO_TypeDef;


typedef struct _ULTRASONIC_InitTypeDef{
	TIM_HandleTypeDef* htim;
	uint32_t		   history_lenght;
	GPIO_TypeDef*      io_trig_GPIOx;
	uint32_t           io_trig_Pin;
	GPIO_TypeDef*      io_echo_GPIOx;
	uint32_t           io_echo_Pin;
	uint32_t           io_echo_IC_tim_channel;
} ULTRASONIC_InitTypeDef;


typedef struct _ULTRASONIC_HandleTypeDef{
	ULTRASONIC_InitTypeDef 			 Init;
	TIM_HandleTypeDef* 	   	   	     htim;
	float 			   	   	    	 distance;
	float* 			   	   			 distance_history;
	uint32_t						 hist_idx;
	ULTRASONIC_IO_TypeDef 			 io;
	volatile ULTRASONIC_Flag_Typedef flag;
	uint8_t							 id;
}ULTRASONIC_HandleTypeDef;


// Functions Prototype

ULTRASONIC_StatusTypeDef ULTRASONIC_Init(ULTRASONIC_HandleTypeDef*);
void 					 ULTRASONIC_DeInit(void);
ULTRASONIC_StatusTypeDef ULTRASONIC_CaptureMeasure(ULTRASONIC_HandleTypeDef*);
void  					 ULTRASONIC_Delay_us(ULTRASONIC_HandleTypeDef*, uint16_t);

void  ULTRASONIC_IC_Callback(TIM_HandleTypeDef*);
void  ULTRASONIC_Timeout_Callback(TIM_HandleTypeDef*);

#endif /* INC_MY_LIB_ULTRASONIC_SENSOR_H_ */
