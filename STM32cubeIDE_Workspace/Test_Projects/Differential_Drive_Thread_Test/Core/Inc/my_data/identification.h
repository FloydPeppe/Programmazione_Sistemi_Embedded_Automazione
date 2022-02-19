/*
 * identification.h
 *
 *  Created on: 14-Dec-2021 11:05:38
 *      Author: peppe
 */

#ifndef INC_IDENTIFICATION_DATA_H_
#define INC_IDENTIFICATION_DATA_H_

#define N_AR_L  3


#define N_MA_L  1


#define TAU_L  2


#define N_AR_R  3


#define N_MA_R  1


#define TAU_R  2


#define AR_L (float[3]) {\
	1.0000000000, -1.9594133333, 0.9594094026, \
}\

#define MA_L (float[1]) {\
	0.0007645193, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9625582521, 0.9625535212, \
}\

#define MA_R (float[1]) {\
	0.0007445920, \
}\

#endif /* INC_IDENTIFICATION_DATA_H_ */
