/*
 * identification.h
 *
 *  Created on: 21-Jan-2022 20:23:57
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
	1.0000000000, -1.9628772750, 0.9628727293, \
}\

#define MA_L (float[1]) {\
	0.0006236748, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9640160285, 0.9640109005, \
}\

#define MA_R (float[1]) {\
	0.0006165877, \
}\

#endif /* INC_IDENTIFICATION_DATA_H_ */
