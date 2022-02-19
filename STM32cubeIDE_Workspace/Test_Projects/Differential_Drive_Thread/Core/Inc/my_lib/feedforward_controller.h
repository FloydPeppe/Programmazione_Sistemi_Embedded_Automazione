/*
 * feedforward_controller.h
 *
 *  Created on: Sep 13, 2021
 *      Author: peppe
 */

#include <stdint.h>

#ifndef INC_MY_LIB_FEEDFORWARD_CONTROLLER_H_
#define INC_MY_LIB_FEEDFORWARD_CONTROLLER_H_


/* ARMA model typedef (AutoRegressive Moving Average).
 *
 * MODEL EXAMPLE:
 *
 * 		y(k)*(ar[0] + ar[1]*z^-1 + ... + ar[n-1]*z^-n) = u(k)*(ma[0]*z^-tau + ma[1]*z^(-1-tau) + ... + ma[m-1]*z^(-m-tau))
 *
 *	with z^-i := delay of i steps, so y(k)*z^-1 = y(k-1). In the source functions newer samples have bigger index number
 *	than older samples so, for example, y(k)*z^0 has the biggest index and y(k)*z^-n has index=0, the smallest.
 *
 * 	n+1 := n_ar  ==> n = n_ar-1
 *  m+1 := n_ma  ==> m = n_ma-1
 * 	tau := lag (integer index) that input (u) needs to affect the output (y)
 *
 *
 *
 */
typedef struct _FEEDFORWARD_ARMA_Parameter{
	uint16_t n_ar;
	uint16_t n_ma;
	uint16_t tau;
	float*   AR;
	float*   MA;
} FEEDFORWARD_ARMA_Parameter;


typedef struct _FEEDFORWARD_InitTypeDef{
	uint16_t arma_n_ar;
	uint16_t arma_n_ma;
	uint16_t arma_tau;
	float*   arma_AR;
	float*   arma_MA;
}FEEDFORWARD_InitTypeDef;


typedef struct _FEEDFORWARD_HandleTypeDef{
	FEEDFORWARD_InitTypeDef    Init;
	FEEDFORWARD_ARMA_Parameter arma;
	float*                     input_buffer;
	float*                     target_buffer;
	float					   current_input;
	float					   current_target;
}FEEDFORWARD_HandleTypeDef;


void  FEEDFORWARD_Init(FEEDFORWARD_HandleTypeDef* hctrl);
void  FEEDFORWARD_DeInit(FEEDFORWARD_HandleTypeDef* hctrl);
void  FEEDFORWARD_Buffer_Init(FEEDFORWARD_HandleTypeDef* hctrl,float *target_init);
float FEEDFORWARD_Inversion_Control(FEEDFORWARD_HandleTypeDef* hctrl, float newest_target);
void  FEEDFORWARD_Buffer_Update(FEEDFORWARD_HandleTypeDef* hctrl);


#endif /* INC_MY_LIB_FEEDFORWARD_CONTROLLER_H_ */
