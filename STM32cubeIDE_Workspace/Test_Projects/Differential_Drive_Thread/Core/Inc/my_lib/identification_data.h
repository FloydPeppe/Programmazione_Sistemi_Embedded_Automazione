/*
 * identification_data.h
 *
 *  Created on: Aug 26, 2021
 *      Author: peppe
 */

#ifndef INC_IDENTIFICATED_PARAMETER_H_
#define INC_IDENTIFICATED_PARAMETER_H_

#define N_AR_L  3


#define N_MA_L  1


#define TAU_L  2


#define N_AR_R  3


#define N_MA_R  1


#define TAU_R  2


#define AR_L (float[3]) {\
	1.0000000000, -1.9748699151, 0.9748774030, \
}\

#define MA_L (float[1]) {\
	0.0074055492, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9673678944, 0.9673734023, \
}\

#define MA_R (float[1]) {\
	0.0097458926, \
}\

#endif /* INC_IDENTIFICATED_PARAMETER_H_ */
