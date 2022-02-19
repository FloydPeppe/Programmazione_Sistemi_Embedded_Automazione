/*
 * identification.h
 *
 *  Created on: 05-Jan-2022 00:45:20
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
	1.0000000000, -1.9631523778, 0.9631512344, \
}\

#define MA_L (float[1]) {\
	0.0006109930, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9642890838, 0.9642841550, \
}\

#define MA_R (float[1]) {\
	0.0006331908, \
}\

#endif /* INC_IDENTIFICATION_DATA_H_ */
