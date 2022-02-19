/*
 * feedforward_controller.c
 *
 *  Created on: Sep 13, 2021
 *      Author: peppe
 */

#include <stdlib.h>
#include "my_lib/feedforward_controller.h"


void FEEDFORWARD_Init(FEEDFORWARD_HandleTypeDef* hctrl){

	// Initializing handle structure values from Init field
	hctrl->arma.n_ar=  hctrl->Init.arma_n_ar;
	hctrl->arma.n_ma=  hctrl->Init.arma_n_ma;
	hctrl->arma.tau=   hctrl->Init.arma_tau;

	hctrl->arma.AR= (float*)calloc(hctrl->arma.n_ar,sizeof(float));
	for(int i=0; i < hctrl->arma.n_ar; i++)
		hctrl->arma.AR[i]= hctrl->Init.arma_AR[i];

	hctrl->arma.MA= (float*)calloc(hctrl->arma.n_ma,sizeof(float));
	for(int i=0; i < hctrl->arma.n_ma; i++)
		hctrl->arma.MA[i]= hctrl->Init.arma_MA[i];

	// Allocating history' arrays
	hctrl->target_buffer= (float*)calloc(hctrl->arma.n_ar,sizeof(float));
	hctrl->input_buffer=  (float*)calloc(hctrl->arma.n_ma,sizeof(float));

}


void FEEDFORWARD_DeInit(FEEDFORWARD_HandleTypeDef* hctrl){

	// Deallocating dynamic arrays
	free(hctrl->arma.AR);
	free(hctrl->arma.MA);
	free(hctrl->target_buffer);
	free(hctrl->input_buffer);

}

void FEEDFORWARD_Reset(FEEDFORWARD_HandleTypeDef* hctrl){

	// Reset feed-forward buffer (with NULL value in second argument the buffers are initialized with zeros)
	FEEDFORWARD_BufferInit(hctrl, NULL);

}

void FEEDFORWARD_BufferInit(FEEDFORWARD_HandleTypeDef* hctrl, float *target_init){

	FEEDFORWARD_ARMA_Parameter arma= hctrl->arma;

	uint16_t n_ar= arma.n_ar;
	uint16_t n_ma= arma.n_ma;
	uint16_t  tau= arma.tau;

	// If target pointer is equal to NULL the buffers take zero values
	if( (void *)target_init == NULL ){

		for(int i= 0; i < tau; i++)
			hctrl->target_buffer[(n_ar-1)-i]= 0;

		// Complete first values with the same initial value
		for(int i= (n_ar-1)-tau; i >= 0; i--)
			hctrl->target_buffer[i]= 0;

	}
	// Else I use the input values
	else{
		for(int i= 0; i < tau; i++)
			hctrl->target_buffer[(n_ar-1)-i]= target_init[tau-i];

		// Complete first values with the same initial value
		for(int i= (n_ar-1)-tau; i >= 0; i--)
			hctrl->target_buffer[i]= target_init[0];
	}

	// Set current values (see the formula on header file to understand the index below)
	hctrl->current_target= hctrl->target_buffer[n_ar-1-tau];
	hctrl->current_input=  hctrl->input_buffer[n_ma-1];

	// Updating newest target (it's an useful information)
	hctrl->newest_target= hctrl->target_buffer[arma.n_ar-1];
}


float FEEDFORWARD_InversionControl(FEEDFORWARD_HandleTypeDef* hctrl, float newest_target){

	float input= 0;
	FEEDFORWARD_ARMA_Parameter arma= hctrl->arma;

	int oldest_ar_sample= 0;
	int newest_ar_sample= arma.n_ar-1;  // Last minus one because
								    	//
	int oldest_ma_sample= 0;			//
	int newest_ma_sample= arma.n_ma-1;  // C arrays start from 0

	// Assigning the newest target value
	hctrl->target_buffer[newest_ar_sample]= newest_target;

	// Compute feed-forward input (inversion control)
	for(int i= oldest_ar_sample; i <= newest_ar_sample; i++)
		input += arma.AR[newest_ar_sample-i]*hctrl->target_buffer[i];
	// I know inputs until the newest_ma_sample-1
	for(int j= oldest_ma_sample; j <= newest_ma_sample-1; j++)
		input += -arma.MA[newest_ma_sample-j]*hctrl->input_buffer[j];
	// Last input is computed here
	input /= arma.MA[0];

	// Saving newest input
	hctrl->input_buffer[newest_ma_sample]= input;

	// Updating Buffer for next step
	FEEDFORWARD_BufferUpdate(hctrl);

	return input;
}


void FEEDFORWARD_BufferUpdate(FEEDFORWARD_HandleTypeDef* hctrl){

	FEEDFORWARD_ARMA_Parameter arma= hctrl->arma;

	// Updating current values (see the formula on header file to understand the index below)
	hctrl->current_target= hctrl->target_buffer[arma.n_ar-1-arma.tau];
	hctrl->current_input=  hctrl->input_buffer[arma.n_ma-1];

	// Updating newest target (it's an useful information)
	hctrl->newest_target= hctrl->target_buffer[arma.n_ar-1];

	// Then slide buffers ( look, loops go from idx=0 to idx=n-2 )
	for(int i=0; i < arma.n_ar-1; i++)
		hctrl->target_buffer[i]= hctrl->target_buffer[i+1];
	for(int i=0; i < arma.n_ma-1; i++)
		hctrl->input_buffer[i]= hctrl->input_buffer[i+1];

}

