/*
 * datastream.c
 *
 *  Created on: Aug 7, 2021
 *      Author: peppe
 */

// Includes ---------------------------------------------

#include <my_lib/datastream.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


// Functions definition ----------------------------------

// Setting up streaming
void DATASTREAM_Init(UART_HandleTypeDef *huart){

	// Setting StdIO Stream
	RetargetInit(huart);


}

// Do send values into serial, using a Variadic function
void DATASTREAM_Send_Value(DATASTREAM_mode mode, int vargn, ...){

	// Initializing variadic arguments, using "stdarg.h" macros
	va_list vargp;
	va_start( vargp, vargn );

	switch(mode){

		// ---- Streaming values for Matlab script --------------------
		// (Using this mode I send "n" values separated by tab character,
		//  I end the stream with a newline)
		case stream_on_matlab:

			for(int i=0; i<vargn; i++)
				// Reading value from vargp and sending to serial
				printf("%f\t",va_arg(vargp, double));

			va_arg(vargp, double[2][2]);
			// Deleting last tab and adding a newline
			printf("\b\n");

			break;

		// ---- Streaming values to be printed in a terminal ----------
		// (In this case the function act like a printf with multiple
	    //	format-values pairs)
		case stream_on_terminal:

			// Clearing terminal
			printf("\033[50A");
			printf("\033[2J");
			printf("\r");

			for(int i=0; i<vargn/2; i++)
				// Reading value format-value pair from vargp and sending to serial
				printf(va_arg(vargp, char*), va_arg(vargp,double));

			break;
	}

	// DeInit variables arguments
	va_end( vargp );

}

