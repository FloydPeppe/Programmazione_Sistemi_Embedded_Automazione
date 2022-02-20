/*
 * control_parameters.h
 *
 *  Generated from MATLAB 
 *  20-Feb-2022 18:53:24
 *      
 */

#ifndef INC_MY_DATA_IDENTIFICATION_DATA_H_
#define INC_IDENTIFICATION_DATA_H_


#define PID_P_L  25.000000f
#define PID_I_L  8.000000f
#define PID_D_L  0.000000f
#define PID_P_R  25.000000f
#define PID_I_R  8.000000f
#define PID_D_R  0.000000f


#define N_AR_L  3
#define N_MA_L  1
#define  TAU_L  2
#define N_AR_R  3
#define N_MA_R  1
#define  TAU_R  2


#define AR_L (float[3]) {\
	1.0000000000, -1.9625218583, 0.9625167558, \
}\

#define MA_L (float[1]) {\
	0.0006863963, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9603975029, 0.9603890668, \
}\

#define MA_R (float[1]) {\
	0.0007507160, \
}\


#endif /* INC_IDENTIFICATION_DATA_H_ */
