/*
 * differential_drive.h
 *
 *  Created on: Feb 8, 2022
 *      Author: peppe
 */

#ifndef INC_DIFFERENTIAL_DRIVE_H_
#define INC_DIFFERENTIAL_DRIVE_H_

// Choose if use RTOS version or not (Set or not in project build settings, DEFINE symbols section)
//#define DIFFDRIVE_RTOS_V_

// Choose if debug control timing
#define DIFFDRIVE_DEBUG_TIMING_


/* Includes */

#include "stm32f4xx_hal.h"
#include "feedforward_controller.h"
#include "pid_controller.h"
#include <stdbool.h>

#ifdef DIFFDRIVE_RTOS_V_
#include "cmsis_os.h"
#endif


/* Defines and macros */

// Maximun Number of Samples saved by History
#define MAX_HISTORY_SAMPLE 1400

// Extracting wheel value from state register
#define __DIFFDRIVE_ENCODER_GET_CURRENT_VALUE(__HANDLE__,__SIDE__) (float)(__HANDLE__)->encoder[__SIDE__].current_value

/* Types Definition */

typedef enum _DIFFDRIVE_Wheel_Enum{
	left=  0x00U,
	right= 0x01U
} DIFFDRIVE_Wheel_Enum;


typedef enum _DIFFDRIVE_Direction_Enum{
	forward=    0x00U,
	backward=   0x01U,
} DIFFDRIVE_Direction_Enum;

typedef enum _DIFFDRIVE_StatusTypeDef{
	DIFFDRIVE_OK=     0x00U,
	DIFFDRIVE_ERROR=  0x01U
}DIFFDRIVE_StatusTypeDef;

typedef struct _DIFFDRIVE_Mechanical_Parameter{
	float r;
	float l;
	float max_wheel_speed;
} DIFFDRIVE_Mechanical_Parameter;


typedef struct _DIFFDRIVE_TIMING_TypeDef{
	TIM_HandleTypeDef* htim;
	float              time_step;
#ifdef DIFFDRIVE_RTOS_V_
	osSemaphoreId_t    semaphore_id;
#else
	volatile bool 	   flag;
#endif
}DIFFDRIVE_TIMING_TypeDef;


typedef struct _DIFFDRIVE_MOTOR_TypeDef{
	TIM_HandleTypeDef* 		 htim[2];
	uint32_t                 pwm_channel[2];
	float					 supply_voltage;
	float					 max_voltage;
	float					 min_voltage;
	uint32_t          		 duty_beat_min;
	uint32_t         		 duty_beat_max;
	uint32_t          		 duty_beat;
	DIFFDRIVE_Direction_Enum direction;
}DIFFDRIVE_MOTOR_TypeDef;


typedef struct _DIFFDRIVE_ENCODER_TypeDef{
	TIM_HandleTypeDef* htim;
	uint32_t           tim_channel;
	uint8_t            tim_arr_resolution;
	float              resolution;
	float 		 	   current_value;
	float 			   previous_value;
}DIFFDRIVE_ENCODER_TypeDef;


typedef struct _DIFFDRIVE_POSE_TypeDef{
	float	x_a;
	float   y_a;
	float   theta;
	float   v;
	float   om;
}DIFFDRIVE_POSE_TypeDef;


typedef struct _DIFFDRIVE_STATE_TypeDef{
	DIFFDRIVE_POSE_TypeDef pose;
	float                  wheel[2];
	float 				   wheel_speed[2];
	float                  input[2];
}DIFFDRIVE_STATE_TypeDef;


typedef struct _DIFFDRIVE_CONTROL_TypeDef{
	FEEDFORWARD_HandleTypeDef fdfrwrd;
	PID_HandleTypeDef         pid;
}DIFFDRIVE_CONTROL_TypeDef;


typedef struct _DIFFDRIVE_HISTORY_TypeDef{
	bool        Enabled;
	uint32_t    sample_cnt;
	uint32_t    max_sample;
	uint32_t    index;
	DIFFDRIVE_STATE_TypeDef* state_target;
	DIFFDRIVE_STATE_TypeDef* state_measure;
}DIFFDRIVE_HISTORY_TypeDef;


typedef struct _DIFFDRIVE_InitTypeDef{
	TIM_HandleTypeDef*			   motor_htim[2][2];
	uint32_t 		  			   motor_pwm_channel[2][2];
	float						   motor_supply_voltage[2];
	float						   motor_max_voltage[2];
	float						   motor_min_voltage[2];
	TIM_HandleTypeDef* 			   encoder_htim[2];
	uint32_t 		   			   encoder_tim_channel[2];
	uint8_t		                   encoder_tim_arr_resolution[2];
	TIM_HandleTypeDef*			   timing_htim;
	float 			   			   timing_time_step;
#ifdef DIFFDRIVE_RTOS_V_
	osSemaphoreId_t    			   timing_semaphore_id;
#endif
	float                          mech_prmtr_r;
	float                          mech_prmtr_l;
	float                          mech_prmtr_max_wheel_speed;
	float						   encoder_resolution[2];
	uint16_t 					   control_fdfrwrd_arma_n_ar[2];
	uint16_t 					   control_fdfrwrd_arma_n_ma[2];
	uint16_t 					   control_fdfrwrd_arma_tau[2];
	float*						   control_fdfrwrd_arma_AR[2];
	float* 						   control_fdfrwrd_arma_MA[2];
	float						   control_pid_P[2];
	float						   control_pid_I[2];
	float						   control_pid_D[2];
	int    						   history_max_sample;
}DIFFDRIVE_InitTypeDef;


typedef struct _DIFFDRIVE_HandleTypeDef{
	DIFFDRIVE_InitTypeDef     	   Init;
	DIFFDRIVE_TIMING_TypeDef  	   timing;
	DIFFDRIVE_MOTOR_TypeDef  	   motor[2];
	DIFFDRIVE_ENCODER_TypeDef 	   encoder[2];
	DIFFDRIVE_CONTROL_TypeDef      control[2];
	DIFFDRIVE_STATE_TypeDef		   state;
	DIFFDRIVE_Mechanical_Parameter mech_prmtr;
	DIFFDRIVE_HISTORY_TypeDef 	   history;
}DIFFDRIVE_HandleTypeDef;



/* Function Prototypes */

void DIFFDRIVE_Init(DIFFDRIVE_HandleTypeDef* init_hdiffdrv);
void DIFFDRIVE_DeInit(void);

void DIFFDRIVE_MOTOR_SetPWMs(float* input);
void DIFFDRIVE_MOTOR_Stop(void);

void DIFFDRIVE_ENCODER_Reset(void);
DIFFDRIVE_StatusTypeDef DIFFDRIVE_ENCODER_CaptureMeasure(void);

void DIFFDRIVE_HISTORY_Start(void);
void DIFFDRIVE_HISTORY_Resume(void);
void DIFFDRIVE_HISTORY_Stop(void);
void DIFFDRIVE_HISTORY_Suspend(void);
void DIFFDRIVE_HISTORY_Update(float delta_a_target, float delta_th_target);
void DIFFDRIVE_HISTORY_Get_Init();
void DIFFDRIVE_HISTORY_Get_Sample(DIFFDRIVE_STATE_TypeDef* state_measure, DIFFDRIVE_STATE_TypeDef* state_target);

void DIFFDRIVE_TIMING_Start(void);
void DIFFDRIVE_TIMING_Stop(void);
void DIFFDRIVE_TIMING_Wait(void);
void DIFFDRIVE_TIMING_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

void DIFFDRIVE_STATE_Update(float*);

void DIFFDRIVE_CONTROL_Start(void);
void DIFFDRIVE_CONTROL_Stop(void);
void DIFFDRIVE_CONTROL_Suspend(void);
void DIFFDRIVE_CONTROL_Resume(void);
void DIFFDRIVE_CONTROL_Reset(void);
void DIFFDRIVE_CONTROL_WheelStep(float* newest_target);

void DIFFDRIVE_InputArray(int N, float** input);

void DIFFDRIVE_TrackingStart(void);
void DIFFDRIVE_TrackingStop(void);
void DIFFDRIVE_TrackingSuspend(void);
void DIFFDRIVE_TrackingResume(void);
void DIFFDRIVE_TrackingStep(float v, float om);
void DIFFDRIVE_TrackingArray(int N, float* v, float* om);
void DIFFDRIVE_TrajectorySpeed2WheelAngle(float* wheel, float v, float om);
void DIFFDRIVE_WheelSpeed2TrajectorySpeed(float* v_p, float* om_p, float* wheel_speed);


#endif /* INC_DIFFERENTIAL_DRIVE_H_ */
