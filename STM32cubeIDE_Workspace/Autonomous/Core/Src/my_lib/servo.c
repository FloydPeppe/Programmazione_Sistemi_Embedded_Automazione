/*
 * servo.c
 *
 *  Created on: Jul 26, 2021
 *      Author: peppe
 *  Description:
 * 		Simple library to control a servo motor
 *
 */


/* Includes */

#include "my_lib/servo.h"
#include "my_lib/tim_utils.h"


/*
 * Initialization
 */
void SERVO_Init(SERVO_HandleTypeDef* hservo){

	// Assign timer handler
	hservo->htim = hservo->Init.htim;

	/* Computing prescaler ad autoreload period to obtain an update event with frequency of SERVO_FREQ Hertz */

	uint16_t prescaler=  __MYLIB_GET_TIMER_CLK(hservo->htim)*1/1e6 - 1;
	uint16_t autoreload= __MYLIB_GET_TIMER_CLK(hservo->htim)/( (prescaler+1)*SERVO_FREQ ) - 1;

	// Set counter with computed prescaler and autoreload period
	__HAL_TIM_SET_PRESCALER(hservo->htim, prescaler);
	__HAL_TIM_SET_AUTORELOAD(hservo->htim, autoreload);

	// Compute min and max duty beats
	// min =  0  degree
	// max = 180 degree
	hservo->min_duty_beats= (__HAL_TIM_GET_AUTORELOAD(hservo->htim)+1)*SERVO_MIN_PERC/100;
	hservo->max_duty_beats= (__HAL_TIM_GET_AUTORELOAD(hservo->htim)+1)*SERVO_MAX_PERC/100;

	HAL_TIM_PWM_Start(hservo->htim, hservo->pwm_channel);

}


/*
 * De-Initialization
 */
void SERVO_DeInit(SERVO_HandleTypeDef* hservo){

	HAL_TIM_PWM_Stop(hservo->htim, hservo->pwm_channel);

}


/*
 * Set degree for SERVO (it can move from 0 to 180 degree):
 *
 * num_duty_beats = (max – min)*deg/180 + min
 *
 */
void SERVO_SetDegree(SERVO_HandleTypeDef* hservo, float degree){

	int num_beats = (hservo->max_duty_beats - hservo->min_duty_beats)*degree/180 + hservo->min_duty_beats;
	SERVO_SetBeats(hservo, num_beats);

}


/*
 * Set duty cycle beats for SERVO
 * 	  num_beats must be between MIN and MAX
 */
void SERVO_SetBeats(SERVO_HandleTypeDef* hservo, int num_beats){

	// Avoiding to set num_beats outside ranges
	if( num_beats < hservo->min_duty_beats )
		num_beats = hservo->min_duty_beats;
	else
	if( num_beats > hservo->max_duty_beats )
		num_beats = hservo->max_duty_beats;

	// Setting current PWM duty cycle (via CCR: Capture/Compare TIM's Register)
	__HAL_TIM_SET_COMPARE(hservo->htim,hservo->pwm_channel,num_beats);

}
