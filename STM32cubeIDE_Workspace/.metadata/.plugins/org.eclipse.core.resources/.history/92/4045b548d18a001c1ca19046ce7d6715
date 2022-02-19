/*
 * servo.h
 *
 *  Created on: Jul 26, 2021
 *      Author: peppe
 *  Description:
 * 		Simple library to control a servo motor with 50Hz working frequency
 *
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_


/* Includes */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"


/* Defines */

// This defines depends on servo in use

// MIN duty cycle percentage
#define SERVO_MIN_PERC	(float) 3.5
// MAX duty cycle percentage
#define SERVO_MAX_PERC	(float) 13.1
// Working frequency (Hz)
#define SERVO_FREQ 50


/* Types Definition */

typedef struct _SERVO_InitTypeDef{
	TIM_HandleTypeDef* htim;
	uint32_t           pwm_channel;
}SERVO_InitTypeDef;

typedef struct _SERVO_HandleTypeDef{
	// Init field
	SERVO_InitTypeDef  Init;
	// Timer
	TIM_HandleTypeDef* htim;
	uint32_t           pwm_channel;
	// Min and max duty cycle beats (they depend on Period parameter)
	int 			   min_duty_beats;
	int 			   max_duty_beats;
} SERVO_HandleTypeDef;


/* Function Prototypes */

void SERVO_Init(SERVO_HandleTypeDef* handle);
void SERVO_DeInit(SERVO_HandleTypeDef* handle);
void SERVO_SetDegree(SERVO_HandleTypeDef* handle, float degree);
void SERVO_SetBeats(SERVO_HandleTypeDef* handle, int pulse_width);


#endif /* INC_SERVO_H_ */
