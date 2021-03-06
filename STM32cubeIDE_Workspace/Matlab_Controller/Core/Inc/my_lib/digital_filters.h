/*
 * digital_filters.h
 *
 *  Created on: May 30, 2021
 *      Author: peppe
 */

#ifndef INC_DIGITAL_FILTERS_H_
#define INC_DIGITAL_FILTERS_H_


// Macros and type definitions --------------------------------------------------------------------------------------- //

// Macros to get filtered value
#define __MAF_GET_VAL(__HANDLE__) (__HANDLE__)->avg


// Moving Average Filter Structure
typedef struct{
	int    buffsize; // Buffer Size
	float  sum;      // Current Sum
	float  avg;      // Average
	int    idx;		 // Index
	float* buffer;   // Buffer
	float* weights;  // Weights (optional)
} MAF_HandleTypeDef;


// Function prototypes ------------------------------------------------------------------------------------------ //

MAF_HandleTypeDef MAF_Create(const int buffsize, float *weights);
void MAF_Reset(MAF_HandleTypeDef* filter, float init_val);
void MAF_Update(MAF_HandleTypeDef* filter, const float data);

#endif /* INC_DIGITAL_FILTERS_H_ */
