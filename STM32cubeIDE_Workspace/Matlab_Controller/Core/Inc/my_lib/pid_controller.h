/*
 * pid_controller.h
 *
 *  Created on: Sep 13, 2021
 *      Author: peppe
 */

#include <stdint.h>

#ifndef INC_MY_LIB_PID_CONTROLLER_H_
#define INC_MY_LIB_PID_CONTROLLER_H_

typedef struct _PID_InitTypeDef{
	float P;
	float I;
	float D;
	float Ts;
}PID_InitTypeDef;

typedef struct _PID_HandleTypeDef{
	PID_InitTypeDef Init;
	float P;
	float I;
	float D;
	float u_P;
	float u_I;
	float u_D;
	float u_prev;
	float e_prev;
	float Ts;
}PID_HandleTypeDef;


void  PID_Init(PID_HandleTypeDef* hpid);
void  PID_DeInit(PID_HandleTypeDef* hpid);
void  PID_Reset(PID_HandleTypeDef* hpid);
float PID_Control(PID_HandleTypeDef* hpid, float target, float measure);



#endif /* INC_MY_LIB_FPID_CONTROLLER_H_ */
