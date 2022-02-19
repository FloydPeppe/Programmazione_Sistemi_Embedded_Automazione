/*
 * control_parameters.h
 *
 *  Generated from MATLAB 
 *  19-Feb-2022 21:56:39
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
	1.0000000000, -1.9684327965, 0.9684298884, \
}\

#define MA_L (float[1]) {\
	0.0003736916, \
}\

#define AR_R (float[3]) {\
	1.0000000000, -1.9619862779, 0.9619791914, \
}\

#define MA_R (float[1]) {\
	0.0004486890, \
}\


#endif /* INC_IDENTIFICATION_DATA_H_ */
