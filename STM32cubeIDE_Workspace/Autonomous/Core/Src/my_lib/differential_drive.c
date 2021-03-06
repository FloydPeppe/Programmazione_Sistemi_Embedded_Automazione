/*
 * differential_drive.c
 *
 *  Created on: Feb 8, 2022
 *      Author: peppe
 */

// Includes
#include "my_lib/differential_drive.h"
#include "my_lib/tim_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

/* Global variables */

// Global handle
DIFFDRIVE_HandleTypeDef* hdiffdrv;

#ifdef DIFFDRIVE_RTOS_V_

/* Definitions for diffDrive_semaphore */
const osSemaphoreAttr_t diffDrive_semaphore_attributes = {
  .name = "diffDrive_semaphore"
};

#endif

/* Functions Implementation */

// Initialization
void DIFFDRIVE_Init(DIFFDRIVE_HandleTypeDef* init_hdiffdrv){

	// Initializing global handle
	hdiffdrv= init_hdiffdrv;

	// Assigning control struct
	DIFFDRIVE_CONTROL_TypeDef *hctrl= hdiffdrv->control;

	// Initializing handle structure values from Init field
	hdiffdrv->timing.htim=       		   hdiffdrv->Init.timing_htim;
	hdiffdrv->timing.time_step=   		   hdiffdrv->Init.timing_time_step;
	hdiffdrv->mech_prmtr.r=     		   hdiffdrv->Init.mech_prmtr_r;
	hdiffdrv->mech_prmtr.l=    			   hdiffdrv->Init.mech_prmtr_l;
	hdiffdrv->mech_prmtr.max_wheel_speed=  hdiffdrv->Init.mech_prmtr_max_wheel_speed;

	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

		// Initializing feedforward Control handle structure values from Init field
		hctrl[side].fdfrwrd.Init.arma_n_ar=  hdiffdrv->Init.control_fdfrwrd_arma_n_ar[side];
		hctrl[side].fdfrwrd.Init.arma_n_ma=  hdiffdrv->Init.control_fdfrwrd_arma_n_ma[side];
		hctrl[side].fdfrwrd.Init.arma_AR=    hdiffdrv->Init.control_fdfrwrd_arma_AR[side];
		hctrl[side].fdfrwrd.Init.arma_MA=    hdiffdrv->Init.control_fdfrwrd_arma_MA[side];
		hctrl[side].fdfrwrd.Init.arma_tau=   hdiffdrv->Init.control_fdfrwrd_arma_tau[side];

		// Init feedforward struct
		FEEDFORWARD_Init(&hctrl[side].fdfrwrd);

		// Initializing pid Control handle structure values from Init field
		hctrl[side].pid.Init.P=  hdiffdrv->Init.control_pid_P[side];
		hctrl[side].pid.Init.I=  hdiffdrv->Init.control_pid_I[side];
		hctrl[side].pid.Init.D=  hdiffdrv->Init.control_pid_D[side];
		hctrl[side].pid.Init.Ts= hdiffdrv->timing.time_step;

		// Init feedforward struct
		PID_Init(&hctrl[side].pid);

		// Motors
		hdiffdrv->motor[side].supply_voltage= hdiffdrv->Init.motor_supply_voltage[side];
		hdiffdrv->motor[side].max_voltage=    hdiffdrv->Init.motor_max_voltage[side];
		hdiffdrv->motor[side].min_voltage=    hdiffdrv->Init.motor_min_voltage[side];

		// Starting Motor Timer
		for(DIFFDRIVE_Direction_Enum dir=forward; dir<=backward; dir++){
			hdiffdrv->motor[side].htim[dir]= hdiffdrv->Init.motor_htim[side][dir];

			hdiffdrv->motor[side].pwm_channel[dir]= hdiffdrv->Init.motor_pwm_channel[side][dir];
			HAL_TIM_PWM_Start(hdiffdrv->motor[side].htim[dir], hdiffdrv->motor[side].pwm_channel[dir]);
		}

	    // Computing duty beat ranges
		float supply_volt= hdiffdrv->motor[side].supply_voltage;
		float min_volt=	   hdiffdrv->motor[side].min_voltage;
		float max_volt=    hdiffdrv->motor[side].max_voltage;

		// Forward or backward are the same in this case, I use timers with the same parameters
		hdiffdrv->motor[side].duty_beat_min= __HAL_TIM_GET_AUTORELOAD(hdiffdrv->motor[side].htim[forward])*min_volt/supply_volt;
		hdiffdrv->motor[side].duty_beat_max= __HAL_TIM_GET_AUTORELOAD(hdiffdrv->motor[side].htim[forward])*max_volt/supply_volt;

		// Encoder Sensors
		hdiffdrv->encoder[side].htim=               hdiffdrv->Init.encoder_htim[side];
		hdiffdrv->encoder[side].tim_channel=        hdiffdrv->Init.encoder_tim_channel[side];
		hdiffdrv->encoder[side].resolution=         hdiffdrv->Init.encoder_resolution[side];

		// AutoReloadRegister bit resolution
		hdiffdrv->encoder[side].tim_arr_resolution= hdiffdrv->Init.encoder_tim_arr_resolution[side];

		// Setting up encoders' timers
		__HAL_TIM_SET_PRESCALER(hdiffdrv->encoder[side].htim, 0);

		// Set max autoreload value
		if( hdiffdrv->encoder[side].tim_arr_resolution == 32 )
			// 32 bit case
			__HAL_TIM_SET_AUTORELOAD(hdiffdrv->encoder[side].htim,0xffffffffu);
		else
			// Default 16 bit case
			__HAL_TIM_SET_AUTORELOAD(hdiffdrv->encoder[side].htim,0xffffu);

		// Starting Encoder Sensors
		HAL_TIM_Encoder_Start(hdiffdrv->encoder[side].htim, hdiffdrv->encoder[side].tim_channel);

	}

	// Initialize pose
	hdiffdrv->state.pose.x_a   =  0;
	hdiffdrv->state.pose.y_a   =  0;
	hdiffdrv->state.pose.theta =  0;

	// Allocating history' arrays
	hdiffdrv->history.max_sample= hdiffdrv->Init.history_max_sample;

	// Allocate all variables with zeros (calloc)
	hdiffdrv->history.state_target=  (DIFFDRIVE_STATE_TypeDef*)calloc(hdiffdrv->history.max_sample,sizeof(DIFFDRIVE_STATE_TypeDef));
	hdiffdrv->history.state_measure= (DIFFDRIVE_STATE_TypeDef*)calloc(hdiffdrv->history.max_sample,sizeof(DIFFDRIVE_STATE_TypeDef));

	// Initializing time index
	hdiffdrv->history.index= 0;
	hdiffdrv->history.sample_cnt= 0;


	/* Computing prescaler ad autoreload period to obtain an update event with frequency 1/timing.time_step Hertz */

	DIFFDRIVE_TIMING_TypeDef* timing= &(hdiffdrv->timing);

	uint16_t prescaler=  __MYLIB_GET_TIMER_CLK(timing->htim)*10/1e6 - 1;
	// Guessing is a 16bit autoreload timer to avoid troubles
	uint16_t autoreload= __MYLIB_GET_TIMER_CLK(timing->htim)/( (prescaler+1)*1/timing->time_step ) - 1;

	// Set counter with computed prescaler and autoreload period
	__HAL_TIM_SET_PRESCALER(timing->htim, prescaler);
	__HAL_TIM_SET_AUTORELOAD(timing->htim, autoreload);

	// Assigning callback for timing purpose
	timing->htim->PeriodElapsedCallback= DIFFDRIVE_TIMING_PeriodElapsedCallback;

#ifdef DIFFDRIVE_RTOS_V_
	  /* Creation of diffDrive_semaphore */
	  timing->semaphore_id = osSemaphoreNew(1, 1, &diffDrive_semaphore_attributes);
#else
	// Reset timing flag
	timing->flag= false;
#endif

}


void DIFFDRIVE_DeInit(void){

	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

		// Stop Sensors
		HAL_TIM_Encoder_Stop(hdiffdrv->encoder[side].htim, hdiffdrv->encoder[side].tim_channel);

		// DeInit Feed-feedforard control
		FEEDFORWARD_DeInit(&hdiffdrv->control[side].fdfrwrd);

		// Stop Motor Timer
		for(DIFFDRIVE_Direction_Enum dir=forward; dir<=backward; dir++){
			hdiffdrv->motor[side].pwm_channel[dir]= hdiffdrv->Init.motor_pwm_channel[side][dir];
			HAL_TIM_PWM_Stop(hdiffdrv->motor[side].htim[dir], hdiffdrv->motor[side].pwm_channel[dir]);
		}
	}

	// Deallocating buffers
	free(hdiffdrv->history.state_measure);
	free(hdiffdrv->history.state_target);

#ifdef DIFFDRIVE_RTOS_V_
	osSemaphoreDelete(hdiffdrv->timing.semaphore_id);
#endif
}


void DIFFDRIVE_MOTOR_Stop(){

	// Set motors to 0 volt
	DIFFDRIVE_MOTOR_SetPWMs((float[2]){0,0});

}


void DIFFDRIVE_MOTOR_SetPWMs(float* input){

	DIFFDRIVE_MOTOR_TypeDef *tmp_mtr= hdiffdrv->motor;
	float tmp_input[2];
	float supply_volt;

	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

		tmp_input[side]= input[side];

		supply_volt= tmp_mtr[side].supply_voltage;

		if(tmp_input[side] > 0){
			// Set direction
			tmp_mtr[side].direction= forward;
		}
		else
		if(tmp_input[side] < 0){
			// Set direction
			tmp_mtr[side].direction= backward;
			// Abs of input
			tmp_input[side]= -tmp_input[side];
		}

		// Duty beat proportional to input voltage plus a minimum duty beat (so, if input is near zero
		// duty beat is near its minimum. DC motors need to reach a tension threshold before moving)
		// Also: forward or backward are the same in this case, I use timers with the same parameters
		tmp_mtr[side].duty_beat  = __HAL_TIM_GET_AUTORELOAD(tmp_mtr[side].htim[forward])*tmp_input[side]/supply_volt;
		tmp_mtr[side].duty_beat += tmp_mtr[side].duty_beat_min;

		// Upper Saturation of duty cycle
		if( tmp_mtr[side].duty_beat > tmp_mtr[side].duty_beat_max )
			tmp_mtr[side].duty_beat= tmp_mtr[side].duty_beat_max;
		else
		// Lower Saturation of duty cycle. If reach lower bound, set to zero to be sure to stop motors
		if( tmp_mtr[side].duty_beat == tmp_mtr[side].duty_beat_min )
			tmp_mtr[side].duty_beat= 0;
	}

	// Set PWM signal
	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

		// Set opposite direction (!direction) pin to 0
		__HAL_TIM_SET_COMPARE(tmp_mtr[side].htim[!(tmp_mtr[side].direction)],
							  tmp_mtr[side].pwm_channel[!(tmp_mtr[side].direction)],
							  0  );

		// Set direction pin with the current duty cycle on PWM signal
		__HAL_TIM_SET_COMPARE(tmp_mtr[side].htim[(tmp_mtr[side].direction)],
							  tmp_mtr[side].pwm_channel[(tmp_mtr[side].direction)],
							  tmp_mtr[side].duty_beat  );

		}

}


DIFFDRIVE_StatusTypeDef DIFFDRIVE_ENCODER_CaptureMeasure(void){

	DIFFDRIVE_ENCODER_TypeDef* tmpsens=  hdiffdrv->encoder;
	int32_t tmp_32, counter_max_32;
	int16_t tmp_16, counter_max_16;

	for(DIFFDRIVE_Wheel_Enum side= left; side<=right; side ++){
		// 32 bit timer
		if( tmpsens[side].tim_arr_resolution == 32 ){
			// Counter max int32 bit value, minus a secure threshold of one encoder_resolution, i.e. one full wheel revolution
			counter_max_32= 0x7fffffff-tmpsens[side].resolution;

			// Saving previous value
			tmpsens[side].previous_value= tmpsens[side].current_value;

			// Updating value. The counter connected to sensor are 32 bit unsigned integer so I cast in
			// 32 bit integer (signed) to have positive and negative values.
			tmp_32= (int32_t)__HAL_TIM_GET_COUNTER(tmpsens[side].htim);

			// If counter reached threshold value return an exception
			if( tmp_32 <= -(int32_t)counter_max_32 || tmp_32 >= (int32_t)counter_max_32  )
				return DIFFDRIVE_ERROR;

			// Else convert integer in radiant
			tmpsens[side].current_value = (float)tmp_32*(2*M_PI)/tmpsens[side].resolution;
		}
		// 16 bit timer
		else{
			// Counter max int16 bit value, minus a secure threshold of one encoder_resolution, i.e. one full wheel revolution
			counter_max_16= 0x7fff-tmpsens[side].resolution;

			// Saving previous value
			tmpsens[side].previous_value= tmpsens[side].current_value;

			// Updating value. The counter connected to sensor are 16 bit unsigned integer so I cast in
			// 16 bit integer (signed) to have positive and negative values.
			tmp_16= (int16_t)__HAL_TIM_GET_COUNTER(tmpsens[side].htim);

			// If counter reached threshold value return an exception
			if( tmp_16 <= -(int16_t)counter_max_16 || tmp_16 >= (int16_t)counter_max_16  )
				return DIFFDRIVE_ERROR;

			// Else convert integer in radiant
			tmpsens[side].current_value = (float)tmp_16*(2*M_PI)/tmpsens[side].resolution;
		}
	}

	return DIFFDRIVE_OK;
}


void DIFFDRIVE_ENCODER_Reset(void){

	DIFFDRIVE_ENCODER_TypeDef* tmpsens= hdiffdrv->encoder;

	// Reset Wheel encoders
	for(DIFFDRIVE_Wheel_Enum side= left; side<=right; side ++){
		__HAL_TIM_SET_COUNTER(tmpsens[side].htim,0);
		tmpsens[side].current_value= 0;
	}

}


void DIFFDRIVE_HISTORY_Start(void){

	// Enable history saving and reset time_index counter
	hdiffdrv->history.sample_cnt= 0;
	hdiffdrv->history.Enabled= true;

}


void DIFFDRIVE_HISTORY_Resume(void){

	// Enable history again
	hdiffdrv->history.Enabled= true;

}


void DIFFDRIVE_HISTORY_Stop(void){

	// Disable history saving
	hdiffdrv->history.Enabled= false;

}


void DIFFDRIVE_HISTORY_Suspend(void){

	// Disable history saving
	hdiffdrv->history.Enabled= false;

}


/* ---- Saving only a fixed number of samples in a circular buffer, if history is enabled  ------------------- */
void DIFFDRIVE_HISTORY_Update(float v_target, float om_target){

	DIFFDRIVE_HISTORY_TypeDef* tmphist  = &(hdiffdrv->history);
	DIFFDRIVE_CONTROL_TypeDef* tmpctrl  =   hdiffdrv->control;
	DIFFDRIVE_STATE_TypeDef    tmpstate =   hdiffdrv->state;

	float theta;

	// Sample time
	float Ts= hdiffdrv->timing.time_step;


	// Current index (using operator % to enroll k between 0 and max_sample; sample_cnt, instead, is unbounded)
	uint32_t k_curr= tmphist->sample_cnt%tmphist->max_sample;

	// Update index
	tmphist->index= k_curr;

	// Previous index
	uint32_t k_prev;
	if( tmphist->sample_cnt != 0 )
		k_prev= (tmphist->sample_cnt-1)%tmphist->max_sample;

	// Update history if is enabled
	if( tmphist->Enabled ){

		/* ------ Target history ------ */

		for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

			// Target input, without PID contribute and saturation limits
			tmphist->state_target[k_curr].input[side]= hdiffdrv->control[side].fdfrwrd.current_input;

			// Target wheel values
			tmphist->state_target[k_curr].wheel[side]= __FEEDFORWARD_GET_CURRENT_TARGET(&(tmpctrl[side].fdfrwrd));
			if( tmphist->sample_cnt != 0 ){
				// This is the law: wheel(k)= wheel(k-1) + wheel_speed(k-1)*Ts, because is a discrete time Dynamic System
				tmphist->state_target[k_prev].wheel_speed[side]=  ( tmphist->state_target[k_curr].wheel[side]
												                   -tmphist->state_target[k_prev].wheel[side])*Ts;
				// To have continuity I set also the current variable at same value. Next step it will be update
				tmphist->state_target[k_curr].wheel_speed[side]= tmphist->state_target[k_prev].wheel_speed[side];
			}
			else
				tmphist->state_target[k_curr].wheel_speed[side]= 0;
		}

		// Compute current target pose
		if( tmphist->sample_cnt != 0 ){
			theta= tmphist->state_target[k_prev].pose.theta;
			tmphist->state_target[k_curr].pose.x_a   =  tmphist->state_target[k_prev].pose.x_a   + v_target*Ts*cos(theta);
			tmphist->state_target[k_curr].pose.y_a   =  tmphist->state_target[k_prev].pose.y_a   + v_target*Ts*sin(theta);
			tmphist->state_target[k_curr].pose.theta =  tmphist->state_target[k_prev].pose.theta + om_target*Ts;
			// The law is: pose(k)= pose(k-1) + TrajSpeed(k-1)*Ts, because is a discrete time Dynamic System
			// so here I update previous speed
			tmphist->state_target[k_prev].pose.v= v_target;
			tmphist->state_target[k_prev].pose.om= om_target;
			// To have continuity I set also the current variable at same value. Next step it will be update
			tmphist->state_target[k_curr].pose.v= v_target;
			tmphist->state_target[k_curr].pose.om= om_target;
		}else{
			theta= 0;
			tmphist->state_target[k_curr].pose.x_a   =  v_target*Ts*cos(theta);
			tmphist->state_target[k_curr].pose.y_a   =  v_target*Ts*sin(theta);
			tmphist->state_target[k_curr].pose.theta =  om_target*Ts;

		}

		/* ------ State history ------ */

		// State measure, extracted from state register
		tmphist->state_measure[k_curr]= tmpstate;
		// Assigning PREVIOUS speed, because the law is:
		// wheel(k)= wheel(k-1) + wheel_speed(k-1)*Ts, because is a discrete time Dynamic System
		if( tmphist->sample_cnt != 0 ){
			tmphist->state_measure[k_prev].wheel_speed[left ]= tmpstate.wheel_speed[left ];
			tmphist->state_measure[k_prev].wheel_speed[right]= tmpstate.wheel_speed[right];
			// Same for trajectory speed
			tmphist->state_measure[k_prev].pose.v=  tmpstate.pose.v;
			tmphist->state_measure[k_prev].pose.om= tmpstate.pose.om;

		}

		/* Increment sample counter */
		tmphist->sample_cnt++;
	}
}


/* ---- Init time index to properly read the history circular buffer ----- */
void DIFFDRIVE_HISTORY_Get_Init(void){

	// Disabling history recording
	DIFFDRIVE_HISTORY_Suspend();

	DIFFDRIVE_HISTORY_TypeDef* tmphist= &(hdiffdrv->history);

	// Initialize current time index to start read from oldest saved values
	if( tmphist->sample_cnt >= tmphist->max_sample )
		tmphist->index= (tmphist->sample_cnt-tmphist->max_sample)%tmphist->max_sample;
	else
		tmphist->index= 0;
}


/* --- Read state sample from History --- */
void DIFFDRIVE_HISTORY_Get_Sample(DIFFDRIVE_STATE_TypeDef* state_measure, DIFFDRIVE_STATE_TypeDef* state_target){

	DIFFDRIVE_HISTORY_TypeDef* tmphist= &(hdiffdrv->history);

	if( state_measure != NULL )
		*state_measure= tmphist->state_measure[tmphist->index];
	if( state_target != NULL )
		*state_target = tmphist->state_target[tmphist->index];

	// If history is disabled (it means that I read history after the movement happened) update index.
	// Otherwise just read current sample.
	if( !(tmphist->Enabled) ){
 		// Update index for next step reading
		if( tmphist->sample_cnt >= tmphist->max_sample )
			tmphist->index= (tmphist->index+1)%tmphist->max_sample;
		else
			tmphist->index++;
	}
}


void DIFFDRIVE_CONTROL_Start(void){

	// Reset Encoders
	DIFFDRIVE_ENCODER_Reset();

	// Reset Control
	DIFFDRIVE_CONTROL_Reset();

	// Starting and enabling history
	DIFFDRIVE_HISTORY_Start();

	// Start control timing
	DIFFDRIVE_TIMING_Start();

}

void DIFFDRIVE_CONTROL_Resume(void){

	// Reset Control
	DIFFDRIVE_CONTROL_Reset();

	// Enabling again history
	DIFFDRIVE_HISTORY_Resume();

	// Restart control timing
	DIFFDRIVE_TIMING_Start();

}

void DIFFDRIVE_CONTROL_Stop(void){

	// Stop and disabling history
	DIFFDRIVE_HISTORY_Stop();

	// Stop timing, control finished
	DIFFDRIVE_TIMING_Stop();

	// Set motor to 0 volt
	DIFFDRIVE_MOTOR_Stop();

}

void DIFFDRIVE_CONTROL_Suspend(void){

	// Disabling history
	DIFFDRIVE_HISTORY_Suspend();

	// Stop timing, control finished
	DIFFDRIVE_TIMING_Stop();

	// Set motor to 0 volt
	DIFFDRIVE_MOTOR_Stop();

}


void DIFFDRIVE_CONTROL_Reset(void){

	DIFFDRIVE_CONTROL_TypeDef* hctrl=    hdiffdrv->control;
	DIFFDRIVE_HISTORY_TypeDef* tmphist= &hdiffdrv->history;

	// Capture current wheel measure
	DIFFDRIVE_ENCODER_CaptureMeasure();

	// Update diffDrive state (input is {0,0} because at restart i guess input voltage is zero)
	DIFFDRIVE_STATE_Update((float[2]){0,0});

	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

		// Reset feed-forward buffer with initial values the same with measured ones
		FEEDFORWARD_Reset(&hctrl[side].fdfrwrd, hdiffdrv->state.wheel[side]);
		// Reset PID
		PID_Reset(&hctrl[side].pid);
	}

	// If a reset occurred let converge pose target and pose measure. Because it's like that control start again
	// from a new initial point.
	uint32_t k_curr= tmphist->index;
	for(DIFFDRIVE_Wheel_Enum side=left; side <= right; side++)
		tmphist->state_target[k_curr].wheel[side]= tmphist->state_measure[k_curr].wheel[side];
	tmphist->state_target[k_curr].pose.theta= tmphist->state_measure[k_curr].pose.theta;
	tmphist->state_target[k_curr].pose.x_a=   tmphist->state_measure[k_curr].pose.x_a;
	tmphist->state_target[k_curr].pose.y_a=   tmphist->state_measure[k_curr].pose.y_a;

}


void DIFFDRIVE_TIMING_Start(void){

#ifdef DIFFDRIVE_RTOS_V_
	// At starting take the only one available token
	osStatus_t stat= osSemaphoreAcquire(hdiffdrv->timing.semaphore_id, 0);
	(void)stat;
#endif
	// Starting timer in interrupt mode for control law timing
	HAL_TIM_Base_Start_IT(hdiffdrv->timing.htim);

}


void DIFFDRIVE_TIMING_Stop(void){

	// Stop timer in interrupt mode
	HAL_TIM_Base_Stop_IT(hdiffdrv->timing.htim);

#ifdef DIFFDRIVE_RTOS_V_
	// Try to release a token when timing is being stopped (if the token was already released, by callback, it doesn't mind)
	osSemaphoreRelease(hdiffdrv->timing.semaphore_id);
#endif

}


/* ----------- Timing wait for control step enable ------------ */

#ifdef DIFFDRIVE_DEBUG_TIMING_
	// Variable used to check system timing
	uint32_t tic=0, toc[100], i_=0;
#endif

void DIFFDRIVE_TIMING_Wait(void){

#ifdef DIFFDRIVE_RTOS_V_

	// Wait for an available token (TIMING_Callback function will release the only token when invoked by ISR)
	osStatus_t stat= osSemaphoreAcquire(hdiffdrv->timing.semaphore_id, osWaitForever);
	(void)stat;

#ifdef DIFFDRIVE_DEBUG_TIMING_

			// Check if control timing is really working
			if( i_ < sizeof(toc)/sizeof(uint32_t) )
				toc[i_]= osKernelGetTickCount()-tic;
			i_++;

			tic= osKernelGetTickCount();

#endif /* DIFFDRIVE_DEBUG_TIMING_ */


#else /* ifndef DIFFDRIVE_RTOS_V */

	// Wait until the timing flag is true
	while( !(hdiffdrv->timing.flag) )
		__NOP();

	// Reset timing flag
	hdiffdrv->timing.flag= false;

#ifdef DIFFDRIVE_DEBUG_TIMING_

			// Check if control timing is really working
			if( i_ < sizeof(toc)/sizeof(uint32_t) )
				toc[i_]= HAL_GetTick()-tic;
			i_++;

			tic=  HAL_GetTick();

#endif /* DIFFDRIVE_DEBUG_TIMING_ */
#endif /* DIFFDRIVE_RTOS_V */

}


void DIFFDRIVE_STATE_Update(float* input){

	DIFFDRIVE_STATE_TypeDef* tmpstate= &(hdiffdrv->state);

	float v, om, theta;

	float min_volt;
	float max_volt;

	// Sample time
	float Ts= hdiffdrv->timing.time_step;

	// Updating state
	for(DIFFDRIVE_Wheel_Enum side= left; side<=right; side++){
		tmpstate->wheel[side]=        hdiffdrv->encoder[side].current_value;
		tmpstate->wheel_speed[side]= (hdiffdrv->encoder[side].current_value-hdiffdrv->encoder[side].previous_value)/Ts;

		min_volt= hdiffdrv->motor[side].min_voltage;
		max_volt= hdiffdrv->motor[side].max_voltage;

		// Input Saturation limits, for "measured" tension-------------------------
		if( input[side] > (max_volt-min_volt))
			tmpstate->input[side]= max_volt-min_volt;
		else
		if( input[side] < -(max_volt-min_volt))
			tmpstate->input[side]= -(max_volt-min_volt);
		else
			tmpstate->input[side]= input[side];
		// ------------------------------------------------------------------------
	}

	// Compute and update current pose an speed
	DIFFDRIVE_WheelSpeed2TrajectorySpeed(&v, &om,  tmpstate->wheel_speed);

	theta= tmpstate->pose.theta;
	tmpstate->pose.x_a   +=  v*Ts*cos(theta);
	tmpstate->pose.y_a   +=  v*Ts*sin(theta);
	tmpstate->pose.theta +=  om*Ts;
	tmpstate->pose.v=  v;
	tmpstate->pose.om= om;
}


void DIFFDRIVE_TrajectorySpeed2WheelAngle(float* wheel, float v, float om){

	// Sample time
	float Ts= hdiffdrv->timing.time_step;

	float r= hdiffdrv->mech_prmtr.r;
	float l= hdiffdrv->mech_prmtr.l;

	// Wheel target computation (it's affect input var "wheel" because is a pointer.
	// Also note the "+=" symbol and not "=")
	if( wheel != NULL){
		wheel[left ] +=  (v/r - l*om/r)*Ts;
		wheel[right] +=  (v/r + l*om/r)*Ts;
	}

}

void DIFFDRIVE_WheelSpeed2TrajectorySpeed(float* v_p, float* om_p, float* wheel_speed){

	float r= hdiffdrv->mech_prmtr.r;
	float l= hdiffdrv->mech_prmtr.l;

	// Trajectory Variations computation (it's affect input vars v_p and om_p because are pointers)
	if( v_p != NULL)
		*v_p= (wheel_speed[right] + wheel_speed[left])*r/2;

	if( om_p != NULL )
		*om_p= (wheel_speed[right] - wheel_speed[left])*r/(2*l);

}


void DIFFDRIVE_CONTROL_WheelStep(float* newest_target){

	DIFFDRIVE_CONTROL_TypeDef* hctrl=  hdiffdrv->control;
	DIFFDRIVE_StatusTypeDef status;

	float curr_target[2], curr_measure, input[2];

	/* Do control wheels */

	// Waiting for step control enabling
	DIFFDRIVE_TIMING_Wait();

	// Then get wheel step measure If counters are not near overflow (look at CaptureMeasure function)
	if( (status= DIFFDRIVE_ENCODER_CaptureMeasure()) == DIFFDRIVE_ERROR ){
		// If exception, reset control
		DIFFDRIVE_CONTROL_Reset();
		// Reset target temporary variable
		newest_target[left ]= 0;
		newest_target[right]= 0;
	}

	// Compute input
	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++){

		// Computing input using inversion control
		input[side]  = FEEDFORWARD_InversionControl(&(hctrl[side].fdfrwrd), newest_target[side]);

		// Wheel angle read from sensor
		curr_measure= __DIFFDRIVE_ENCODER_GET_CURRENT_VALUE(hdiffdrv,side);

		// Current target value saved in the feed-forward controller struct (you must call before the
		// Inversion Control function and then to extract this value. Look at feed-forward library to
		// better understand why)
		curr_target[side]= __FEEDFORWARD_GET_CURRENT_TARGET(&(hctrl[side].fdfrwrd));

		// Computing PID input
		input[side] += PID_Control(&(hctrl[side].pid), curr_target[side], curr_measure);

	}

	// Set new PWMs signals
	DIFFDRIVE_MOTOR_SetPWMs(input);

	// Update differential drive state structure
	DIFFDRIVE_STATE_Update(input);

}

void DIFFDRIVE_TrackingStart(void){

	// Start control
	DIFFDRIVE_CONTROL_Start();

	// First step wheels control (starts with zeros, robot still)
	DIFFDRIVE_TrackingStep(0,0);

}

void DIFFDRIVE_TrackingResume(void){

	// Resume control
	DIFFDRIVE_CONTROL_Resume();

}

void DIFFDRIVE_TrackingStop(void){

	// Stop control
	DIFFDRIVE_CONTROL_Stop();

}

void DIFFDRIVE_TrackingSuspend(void){

	// Suspend control
	DIFFDRIVE_CONTROL_Suspend();

}

void DIFFDRIVE_TrackingStep(float v, float om){

	DIFFDRIVE_CONTROL_TypeDef* tmpctrl= hdiffdrv->control;
	float newest_target[2];

	// Extracting previous target
	for(DIFFDRIVE_Wheel_Enum side=left; side<=right; side++)
		newest_target[side]= __FEEDFORWARD_GET_NEWEST_TARGET(&tmpctrl[side].fdfrwrd);

	// Computing newest target
	DIFFDRIVE_TrajectorySpeed2WheelAngle(newest_target, v, om);

	// Make a wheel step motion
	DIFFDRIVE_CONTROL_WheelStep(newest_target);

	// Updating History
	DIFFDRIVE_HISTORY_Update(v, om);

}

void DIFFDRIVE_TrackingArray(int N, float* delta_a, float* delta_theta){

	// Trajectory tracking
	for(int i= 0; i<N; i++)
		DIFFDRIVE_TrackingStep(delta_a[i], delta_theta[i]);

}


void DIFFDRIVE_InputArray(int N, float **input ){

	// Reset Encoders
	DIFFDRIVE_ENCODER_Reset();

	// Enabling history
	DIFFDRIVE_HISTORY_Start();

	// Start control timing
	DIFFDRIVE_TIMING_Start();

	float tmpin[2];

	for(int i=0; i<N; i++){

		// Wait the right moment
		DIFFDRIVE_TIMING_Wait();

		tmpin[left ]= input[left ][i];
		tmpin[right]= input[right][i];

		DIFFDRIVE_ENCODER_CaptureMeasure();

		DIFFDRIVE_MOTOR_SetPWMs(tmpin);

		// Update differential drive state structure
		DIFFDRIVE_STATE_Update(tmpin);

		// Update History
		DIFFDRIVE_HISTORY_Update(0, 0);
	}

	// Stop control timing
	DIFFDRIVE_TIMING_Stop();

	// Set motor to 0 volt
	DIFFDRIVE_MOTOR_Stop();

}


void DIFFDRIVE_TIMING_PeriodElapsedCallback(TIM_HandleTypeDef* htim){

#ifdef DIFFDRIVE_RTOS_V_
	// Release the token
	osSemaphoreRelease(hdiffdrv->timing.semaphore_id);
#else
	// Enabling timing flag
	hdiffdrv->timing.flag= true;
#endif

}


