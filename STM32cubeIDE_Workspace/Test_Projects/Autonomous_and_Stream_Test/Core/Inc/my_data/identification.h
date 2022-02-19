/*
 * identification.h
 *
 *  Created on: 19-Dec-2021 23:37:00
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
	1.0000000000, -1.9619898716, 0.9619863820, \
}\

#define MA_L (float[1]) {\
	0.0006414863, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9647811445, 0.9647788920, \
}\

#define MA_R (float[1]) {\
	0.0006196722, \
}\

#endif /* INC_IDENTIFICATION_DATA_H_ */
