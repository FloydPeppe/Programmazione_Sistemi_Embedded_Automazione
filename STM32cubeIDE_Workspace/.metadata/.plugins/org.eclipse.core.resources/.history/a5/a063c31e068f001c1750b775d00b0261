/*
 * ultrasonic_sensor.c
 *
 *  Created on: Sep 30, 2021
 *      Author: peppe
 */


#include "my_lib/ultrasonic_sensor.h"
#include "my_lib/tim_utils.h"
#include <stdlib.h>

// Global array of handles pointers
ULTRASONIC_HandleTypeDef** hultrasonic;

// Global index associated with each sensor (here static is for limiting this variable in this src file)
static uint8_t glbl_idx= 0;

ULTRASONIC_StatusTypeDef ULTRASONIC_Init(ULTRASONIC_HandleTypeDef* hultrsnc){

	// Check if it's possible to initialize a new sensor
	if( glbl_idx < MAX_SENSORS ){

		// Realloc array of handles pointers
		hultrasonic= (ULTRASONIC_HandleTypeDef**)realloc(hultrasonic, glbl_idx*sizeof(ULTRASONIC_HandleTypeDef*));

		// Link current handle with input handle using pointers
		hultrasonic[glbl_idx]= hultrsnc;
	}
	else
		// Return exception
		return ULTRASONIC_ERROR;

	// Assign sensor id and increment global index
	hultrsnc->id= glbl_idx++;

	// Init fields
	hultrsnc->htim=		  		      hultrsnc->Init.htim;
	hultrsnc->io.trig_GPIOx= 		  hultrsnc->Init.io_trig_GPIOx;
	hultrsnc->io.trig_Pin=  		  hultrsnc->Init.io_trig_Pin;
	hultrsnc->io.echo_GPIOx= 		  hultrsnc->Init.io_echo_GPIOx;
	hultrsnc->io.echo_Pin=  		  hultrsnc->Init.io_echo_Pin;
	hultrsnc->io.echo_IC_tim_channel= hultrsnc->Init.io_echo_IC_tim_channel;
	uint32_t n=          			  hultrsnc->Init.history_lenght;
	hultrsnc->hist_idx= 0;

	// Default flags value is reset
	hultrsnc->flag.input_captured= false;
	hultrsnc->flag.timeout=        false;

	// Allocating history array
	hultrsnc->distance_history= (float*)calloc(n,sizeof(float));

	// Assigning timer callback
	hultrsnc->htim->PeriodElapsedCallback= ULTRASONIC_Timeout_Callback;
	hultrsnc->htim->IC_CaptureCallback=    ULTRASONIC_IC_Callback;

	// Computing prescaler to obtain a timer with frequency of 1MHz
	uint32_t prescaler= __MYLIB_GET_TIMER_CLK(hultrsnc->htim)/1e6 - 1;

	// Set counter with 1MHz update counter frequency (period of one microsecond)
	__HAL_TIM_SET_PRESCALER(hultrsnc->htim,prescaler);

	// Set counter autoreload period (is the maximum microsecond echo time allowed by sensor, plus a small margin )
	__HAL_TIM_SET_AUTORELOAD(hultrsnc->htim,(uint32_t)(ULTRASONIC_MAX_DISTANCE/SOUND_SPEED_cm_us)+1000);

	// Return success
	return ULTRASONIC_OK;

}


void ULTRASONIC_DeInit(void){

	// Free ultrasonic distance histories
	for(int idx= 0; idx < glbl_idx; idx++)
		free(hultrasonic[idx]->distance_history);

	// Free array of handles pointers
	free(hultrasonic);

}


ULTRASONIC_StatusTypeDef ULTRASONIC_CaptureMeasure(ULTRASONIC_HandleTypeDef* hultrsnc){

	TIM_HandleTypeDef* htim= hultrsnc->htim;

	ULTRASONIC_IO_TypeDef io= hultrsnc->io;

	uint32_t echo_time = 0;

	/* Starting Ultrasonic Sensor measure */

	// Reset flags
	hultrsnc->flag.input_captured= false;
	hultrsnc->flag.timeout=        false;

	// Start period elapsed interrupt to handle timeout events
	HAL_TIM_Base_Start_IT(htim);

	// Start input capture interrupt, needed to measure ECHO high pulse time
	HAL_TIM_IC_Start_IT(htim, io.echo_IC_tim_channel);

	// Start Timer in order to use ULTRASONIC_Delay_us
	HAL_TIM_Base_Start(htim);

	// Pull TRIG pin low for at least 2 us
	HAL_GPIO_WritePin(io.trig_GPIOx, io.trig_Pin, GPIO_PIN_RESET);
	ULTRASONIC_Delay_us(hultrsnc, 2);

	// Keep TRIG pin High for 10 us to start sensor
	HAL_GPIO_WritePin(io.trig_GPIOx, io.trig_Pin, GPIO_PIN_SET);
	ULTRASONIC_Delay_us(hultrsnc, 10);

	// Then keep low
	HAL_GPIO_WritePin(io.trig_GPIOx, io.trig_Pin, GPIO_PIN_RESET);
/*
	// Stop Timer
	HAL_TIM_Base_Stop(htim);

	// Measure time for which the ECHO pin is high //

	// Reset flags
	hultrsnc->flag.input_captured= false;
	hultrsnc->flag.timeout=        false;

	// Start period elapsed interrupt to handle timeout events
	HAL_TIM_Base_Start_IT(htim);

	// Start input capture interrupt, needed to measure ECHO high pulse time
	HAL_TIM_IC_Start_IT(htim, io.echo_IC_tim_channel);
*/
	// Measure ECHO High pulse time, waiting twice for a captured input interrupt. Note that the IC callback function
	// reset the timer counter. THE TIMER MUST BE PREVIOUSLY CONFIGURED IN BOTH EDGES INPUT CAPTURE MODE!!
	for(int i=0; i<2; i++){
		// Wait for an input captured or timeout event
		while ( !(hultrsnc->flag.input_captured) && !hultrsnc->flag.timeout )
			__NOP();
		// Reset IC flag
		hultrsnc->flag.input_captured= false;
	}

	// Stop IC interrupt
	HAL_TIM_IC_Stop_IT(htim, io.echo_IC_tim_channel);

	// Stop Base interrupt
	HAL_TIM_Base_Stop_IT(htim);

	// Stop Timer
	HAL_TIM_Base_Stop(htim);

	// If timeout occurred return a sensor error
	if( hultrsnc->flag.timeout )
		return ULTRASONIC_ERROR;
	// Else, measured time from timer Compare Register
	else
		echo_time= __HAL_TIM_GET_COMPARE(htim, io.echo_IC_tim_channel);


	// Computing distance in cm (division by 2 because the wave travels double distance going back to sensor)
	float distance  = (float)echo_time*SOUND_SPEED_cm_us/2;

	// Saving current distance
	hultrsnc->distance= distance;

	// Updating History (as circular buffer)
	uint32_t hist_idx= hultrsnc->hist_idx++;
	hultrsnc->distance_history[hist_idx%(hultrsnc->Init.history_lenght)]= distance;

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

	// Checking which htim caused the callback and set flag
	for(int idx= 0; idx<glbl_idx; idx++)
		if( hultrasonic[idx]->htim == htim ){
			hultrasonic[idx]->flag.input_captured= true;
			// Reset counter
			__HAL_TIM_SET_COUNTER(hultrasonic[idx]->htim, 0);
			break;
		}

}

void ULTRASONIC_Timeout_Callback(TIM_HandleTypeDef* htim){

	// Checking which htim caused the callback and set flag
	for(int idx= 0; idx<glbl_idx; idx++)
		if( hultrasonic[idx]->htim == htim ){
			hultrasonic[idx]->flag.timeout= true;
			break;
		}

}

