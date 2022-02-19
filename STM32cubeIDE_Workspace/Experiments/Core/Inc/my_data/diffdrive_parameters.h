/*
 * diffdrive_parameters.h
 *
 *  Created on: Feb 16, 2022
 *      Author: peppe
 */

#ifndef INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_
#define INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_

// ------ Voltage constants --------- //

#define SUPPLY_VOLTAGE     7.5f

#define MOTOR_MIN_VOLTAGE  0.0f
#define MOTOR_MAX_VOLTAGE  SUPPLY_VOLTAGE


// --------- Mechanical parameters --------------------------------------------------------------------- //

#define WHEEL_DISTANCE 0.155f/2.0f  // Distance between wheels in meters.
#define WHEEL_RADIUS   0.069f/2.0f  // Wheels radius in meters.

#define WHEEL_MAX_SPEED 10.0f  // Radiant each seconds (rad/s). DC motors and gears specifics,
							  // on about the working voltage (~ 6-7V), is 160 rpm (~ 16 rad/s), with
							 // no load, so here I use a smaller value. Below, the web link of the
							// motor specifics (on STM32CubeIDE press CTRL and then click):
						   //
  						  // https://wiki.dfrobot.com/Micro_DC_Motor_with_Encoder-SJ02_SKU__FIT0458
						 //

#endif /* INC_MY_DATA_DIFFDRIVE_PARAMETERS_H_ */
