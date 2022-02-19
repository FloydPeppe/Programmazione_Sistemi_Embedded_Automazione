/*
 * ultrasonic_sensor.c
 *
 *  Created on: Sep 30, 2021
 *      Author: peppe
 */


#include "my_lib/ultrasonic_sensor.h"
#include "my_lib/tim_utils.h"
#include <stdlib.h>

// Flag struct used to manage callbacks
volatile ULTRASONIC_Flag_Typedef flag[MAX_SENSORS];

// Global index associated with each sensor
int8_t glbl_idx= 0;


ULTRASONIC_StatusTypeDef ULTRASONIC_Init(ULTRASONIC_HandleTypeDef* hultrsnc){

	// Check if it's possible to initialize a new sensor
	if( glbl_idx < MAX_SENSORS )
		// Assign sensor index and increment global index
		hultrsnc->sensor_idx= glbl_idx++;
	else
		// Return exception
		return ULTRASONIC_ERROR;

	// Init fields
	hultrsnc->htim=		  		      hultrsnc->Init.htim;
	hultrsnc->io.trig_GPIOx= 		  hultrsnc->Init.io_trig_GPIOx;
	hultrsnc->io.trig_Pin=  		  hultrsnc->Init.io_trig_Pin;
	hultrsnc->io.echo_GPIOx= 		  hultrsnc->Init.io_echo_GPIOx;
	hultrsnc->io.echo_Pin=  		  hultrsnc->Init.io_echo_Pin;
	hultrsnc->io.echo_IC_tim_channel= hultrsnc->Init.io_echo_IC_tim_channel;
	uint16_t n=          			  hultrsnc->Init.history_lenght;

	// Allocating history array
	hultrsnc->distance_history= (float*)calloc(n,sizeof(float));

	// Assigning timer callback
	hultrsnc->htim->PeriodElapsedCallback= ULTRASONIC_Timeout_Callback;
	hultrsnc->htim->IC_CaptureCallback=    ULTRASONIC_IC_Callback;

	// Computing prescaler to obtain a timer with frequency of 1MHz
	uint32_t prescaler= __MYLIB_GET_TIMER_CLK(hultrsnc->htim)/1e6 - 1;

	// Set counter with 1MHz frequency (period of one microsecond)
	__HAL_TIM_SET_PRESCALER(hultrsnc->htim,prescaler);

	// Set counter autoreload period (is the maximum microsecond echo time allowed by sensor (11662),
	// plus a small margin )
	__HAL_TIM_SET_AUTORELOAD(hultrsnc->htim,11662+1000);

	return ULTRASONIC_OK;

}


void  ULTRASONIC_DeInit(ULTRASONIC_HandleTypeDef* hultrsnc){

	free(hultrsnc->distance_history);

}


ULTRASONIC_StatusTypeDef ULTRASONIC_CaptureMeasure(ULTRASONIC_HandleTypeDef* hultrsnc){

	TIM_HandleTypeDef* htim= hultrsnc->htim;

	ULTRASONIC_IO_TypeDef io= hultrsnc->io;

	uint32_t echo_time = 0;

	/* Starting Ultrasonic Sensor measure */

	// Start Timer
	HAL_TIM_Base_Start(htim);

	// Pull TRIG pin low for at least 2 us
	HAL_GPIO_WritePin(io.trig_GPIOx, io.trig_Pin, GPIO_PIN_RESET);
	ULTRASONIC_Delay_us(hultrsnc, 2);

	// Keep TRIG pin High for 10 us to start sensor
	HAL_GPIO_WritePin(io.trig_GPIOx, io.trig_Pin, GPIO_PIN_SET);
	ULTRASONIC_Delay_us(hultrsnc, 10);

	// Then keep low
	HAL_GPIO_WritePin(io.trig_GPIOx, io.trig_Pin, GPIO_PIN_RESET);

	// Stop Timer
	HAL_TIM_Base_Stop(htim);

	/* Measure time for which the ECHO pin is high */

	// Index of current sensor
	uint8_t idx= hultrsnc->sensor_idx;

	// Reset input_capture flag
	flag[idx].input_capture= false;
	flag[idx].timeout= false;

	// Start period elapsed interrupt to handle timeout events
	HAL_TIM_Base_Start_IT(htim);

	// Enabling current callback flag
	flag[idx].callback_enable= true;

	// Wait for the ECHO pin to go high
	while( !HAL_GPIO_ReadPin(io.echo_GPIOx, io.echo_Pin) && !flag[idx].timeout )
		__NOP();

	// Reset counter
	__HAL_TIM_SET_COUNTER(htim, 0);

	// Start input capture interrupt
	HAL_TIM_IC_Start_IT(htim, io.echo_IC_tim_channel);

	// Wait for capture input interrupt
	while ( !(flag[idx].input_capture) && !flag[idx].timeout )
			__NOP();

	// Stop IC interruot
	HAL_TIM_IC_Stop_IT(htim, io.echo_IC_tim_channel);

	// Stop Base interrupt
	HAL_TIM_Base_Stop_IT(htim);

	// Disabling current callback flag
	flag[idx].callback_enable= false;

	// Measured Time
	if( flag[idx].timeout )
		// If timeout occurred return a sensor error
		return ULTRASONIC_ERROR;
	else
		// Else get the measured time from timer Compare Register
		echo_time= __HAL_TIM_GET_COMPARE(htim, io.echo_IC_tim_channel);


	// Computing distance in cm (sound speed on air is about 0.0343 cm/us, us := microseconds)
	float distance  = (float)echo_time * (float)(0.0343)/2;

	// Saving current distance
	hultrsnc->distance= distance;

	return ULTRASONIC_OK;
}


void ULTRASONIC_Delay_us(ULTRASONIC_HandleTypeDef* hultrsnc, uint16_t us){

	TIM_HandleTypeDef* htim= hultrsnc->htim;

	// Reset Counter
	__HAL_TIM_SET_COUNTER(htim, 0);

	// Wait until timer reach target delay (counter frequency is 1Mhz, so 1us every time step)
	while ((__HAL_TIM_GET_COUNTER(htim)) < us)
		;

}


void ULTRASONIC_IC_Callback(TIM_HandleTypeDef* htim){

	// Checking which sensor is enabled
	for(int idx= 0; idx<MAX_SENSORS; idx++)
		if( flag[idx].callback_enable )
			flag[idx].input_capture= true;

}

void ULTRASONIC_Timeout_Callback(TIM_HandleTypeDef* htim){

	// Checking which sensor is enabled
	for(int idx= 0; idx<MAX_SENSORS; idx++)
		if( flag[idx].callback_enable )
			flag[idx].timeout= true;

}
