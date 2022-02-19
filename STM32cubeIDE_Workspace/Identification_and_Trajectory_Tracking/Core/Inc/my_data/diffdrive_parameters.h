/*
 * diffdrive_parameters.h
 *
 *  Generated from MATLAB 
 *  17-Feb-2022 00:41:29
 *      
 */

#ifndef INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_
#define INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_

/*
 Half the distance between wheels, in meters
*/
#define WHEEL_DISTANCE 0.077500f

/*
 Wheels radius in meters
*/
#define WHEEL_RADIUS 0.034500f

/*
 Radiant each seconds (rad/s). DC motors and gears specifics, on
 about the working voltage (~ 6-7V), is 160 rpm (~ 16 rad/s), with
 no load, so here I use a smaller value. Below, the web link of the
 motor specifics:

 https://wiki.dfrobot.com/Micro_DC_Motor_with_Encoder-SJ02_SKU__FIT0458
*/
#define WHEEL_MAX_SPEED 10.000000f

/*
 Supply Voltage 
*/
#define SUPPLY_VOLTAGE 7.500000f

/*
 Minimum voltage 
*/
#define MOTOR_MIN_VOLTAGE 0.000000f

/*
 Maximum voltage 
*/
#define MOTOR_MAX_VOLTAGE 7.500000f

/*
 Control law time step
*/
#define TIME_STEP 0.010000f



#endif /* INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_ */
