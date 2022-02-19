/*
 * pid_controller.c
 *
 *  Created on: Sep 23, 2021
 *      Author: peppe
 */

#include "my_lib/pid_controller.h"


void PID_Init(PID_HandleTypeDef* hpid){

	// Init parameters
	hpid->P=  hpid->Init.P;
	hpid->I=  hpid->Init.I;
	hpid->D=  hpid->Init.D;
	hpid->Ts= hpid->Init.Ts;

	// Init Input contributes variables
	hpid->u_I= 0;
	hpid->u_D= 0;
	hpid->u_P= 0;

	// Init Previous variables
	hpid->u_prev= 0;
	hpid->e_prev= 0;
}



// Reset input buffer and the contributes variables
void PID_Reset(PID_HandleTypeDef* hpid){

	hpid->u_I= 0;
	hpid->u_D= 0;
	hpid->u_P= 0;

	hpid->u_prev= 0;
	hpid->e_prev= 0;



}

float PID_Control(PID_HandleTypeDef* hpid, float target, float measure){

	float e_prev,e_curr, Ts, u;

	// Time step
	Ts= hpid->Ts;

	// Following error
	e_curr= target-measure;
	e_prev= hpid->e_prev;

	// Proportional
	hpid->u_P= hpid->P*e_curr;

	// Integral
	hpid->u_I += hpid->I*e_prev*Ts;

	// Derivative
	hpid->u_D = hpid->D*(e_curr-e_prev)/Ts;

	// Input
	u= hpid->u_P + hpid->u_I + hpid->u_D;

	// Updating struct
	hpid->u_prev= u;
	hpid->e_prev= e_curr;

	return u;

}

