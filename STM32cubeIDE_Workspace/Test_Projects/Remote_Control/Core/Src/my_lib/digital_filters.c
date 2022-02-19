/*
 * digital_filters.c
 *
 *  Created on: May 30, 2021
 *      Author: peppe
 */

#include "my_lib/digital_filters.h"
#include <stdlib.h>

// Moving Average Filter Methods --------------------------------------------------------------------- //

// MAF Create
MAF_HandleTypeDef MAF_Create(const int buffsize, float *weights){
	MAF_HandleTypeDef tmp;

	tmp.buffsize = buffsize;
	tmp.buffer = (float*)calloc(buffsize,sizeof(float));
	tmp.sum = 0;
	tmp.avg = 0;
	tmp.idx = 0;
	tmp.weights= weights;


	return tmp;
}

void MAF_Reset(MAF_HandleTypeDef* filter, float init_val){

	filter->sum = init_val;
	filter->avg = init_val;
	filter->idx = 0;

	for(int i=0; i < filter->buffsize; i++)
		filter->buffer[i]= init_val;
}

// Maf update. Arithmetic or Weighted. If latter the weights array are ordered from oldest to newest
void MAF_Update(MAF_HandleTypeDef* filter, const float data){

	// Weighted
	if(filter->weights != NULL){
		// Store current measure
		filter->buffer[filter->idx] = data;

		// Sum variable
		float sum_tmp = 0;

		// Index variable to scan the circular buffer
		int idx_tmp;

		// Computing average from oldest to newest (from 1 to size+1), in circular buffer
		// the oldest are the next value from the current
		for(int i=1; i<filter->buffsize+1; i++){
			// Current buffer index in the for loop (the mod '%' serve for the circular index)
			idx_tmp = (filter->idx+i)%(filter->buffsize);
			// Current sum
			sum_tmp += (filter->buffer[idx_tmp])*(filter->weights[i-1]) 	;
		}
		filter->avg = sum_tmp;
	}
	// Arithmetic
	else{
		// Remove oldest value from circular buffer
		filter->sum -= filter->buffer[filter->idx];

		// Add current measure
		filter->sum += data;

		// Calculate Average
		filter->avg = (float) filter->sum/filter->buffsize;

		// Store current measure
		filter->buffer[filter->idx] = data;
	}

	// Increase index (the mod '%' serve for the circular buffer)
	filter->idx = (filter->idx+1)%(filter->buffsize);

}
