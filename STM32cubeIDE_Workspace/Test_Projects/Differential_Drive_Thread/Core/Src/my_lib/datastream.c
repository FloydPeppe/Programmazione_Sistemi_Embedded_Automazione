/*
 * datastream.c
 *
 *  Created on: Aug 7, 2021
 *      Author: peppe
 */

// --- Includes ----- //

#include <my_lib/datastream.h>
#include <my_lib/datastream_utils.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

// Global array of handles pointers
DATASTREAM_HandleTypeDef** hdatastream;

// Global index used to set ids each stream (here static is for limiting this variable in this src file)
static int8_t glbl_idx= 0;


/* ----------------- Functions definition --------------------- */

// Setting up streaming
DATASTREAM_StatusTypeDef DATASTREAM_Init(DATASTREAM_HandleTypeDef* hstream){

	// Check if it's possible to initialize a new stream
	if( glbl_idx < MAX_STREAMS ){

		// Realloc array of handles pointers
		hdatastream= (DATASTREAM_HandleTypeDef**)realloc(hdatastream, glbl_idx*sizeof(DATASTREAM_HandleTypeDef*));

		// Connect current handle with input handle using pointers
		hdatastream[glbl_idx]= hstream;

	}
	else
		// Return failure
		return DATASTREAM_ERROR;

	// Assign handle id and increment global index
	hstream->id= glbl_idx++;

	// Local UART handle
	hstream->huart=              hstream->Init.huart;
	// DMA Enable
	hstream->DMA_Rx_Enable=      hstream->Init.DMA_Rx_Enable;

	if( hstream->DMA_Rx_Enable ){
		// DMA Interrupt Request "id"
		hstream->DMA_Rx_Stream_IRQn= hstream->Init.DMA_Rx_Stream_IRQn;
		// Assign Rx callback
		hstream->huart->RxCpltCallback= DATASTREAM_RxCpltCallback;
		// Assign default true value to flag
		hstream->RxCpltFlag= true;
	}

	// Assign Error callback
	hstream->huart->ErrorCallback=  DATASTREAM_ErrorCallback;

	// Return success
	return DATASTREAM_OK;
}


// De-Initialize handle
void DATASTREAM_DeInit(void){
	// Free array of handles pointers
	free(hdatastream);
}


// Receive Data in DMA mode, setting a flag when data is received
DATASTREAM_StatusTypeDef DATASTREAM_ReceiveDMA(DATASTREAM_HandleTypeDef* hstream, uint8_t *data, uint32_t data_size){

	// If flag is false it means that a previous DMA receive is still hanging. Also
	// if DMA is not enabled return error
	if( !hstream->RxCpltFlag || !hstream->DMA_Rx_Enable )
		return DATASTREAM_ERROR;

	// Assign false to Rx complete flag
	hstream->RxCpltFlag= false;

	// Start DMA receiving (if HAL error occurred return failure)
	if( HAL_UART_Receive_DMA(hstream->huart, data, data_size) != HAL_OK )
		return DATASTREAM_ERROR;

	// Return success
	return DATASTREAM_OK;
}


// Function to reset Rx with DMA communication
DATASTREAM_StatusTypeDef DATASTREAM_ReceiveDMA_Reset(DATASTREAM_HandleTypeDef* hstream){

	// If DMA is not enabled return error
	if( !hstream->DMA_Rx_Enable )
		return DATASTREAM_ERROR;

	// Assign default true to flag
	hstream->RxCpltFlag= true;

	// Try resetting uart Rx and DMA interrupt
	HAL_NVIC_DisableIRQ(hstream->DMA_Rx_Stream_IRQn);
	HAL_NVIC_EnableIRQ(hstream->DMA_Rx_Stream_IRQn);

	// Set Uart Rx Ready
	hstream->huart->RxState= HAL_UART_STATE_READY;

	return DATASTREAM_OK;
}


// Do send values into serial, using a Variadic function. This version is for freeRTOS that doesn't support
// printf and sprintf
void DATASTREAM_Print(DATASTREAM_HandleTypeDef* hstream, DATASTREAM_PrintMode mode, int vargn, ...){

	// Initializing variadic arguments, using "stdarg.h" macros
	va_list vargp;
	va_start( vargp, vargn );

	// Timeout of ms
	uint32_t timeout= 1;

	switch(mode){

		// ------------ Streaming values in csv format --------------------
		// (Using this mode I send N values separated by comma character,
		//  each stream is ended with a newline)
		case float_csv:

			for(int i=0; i<vargn; i++){
				char data_buff[15];
				// Init buffer
				for(int k=0; k < sizeof(data_buff)/sizeof(*data_buff); k++)
					data_buff[k]= '\0';

				// Float to Array!
				ftoa((float)va_arg(vargp, double), data_buff, 7);
				// Polling TX
				HAL_UART_Transmit(hstream->huart, (uint8_t *)data_buff, sizeof(data_buff), timeout);
				// Omit last comma
				if( i != vargn-1)
					HAL_UART_Transmit(hstream->huart, (uint8_t *)((char[1]){','}), sizeof(uint8_t), timeout);
			}
			// Adding a newline
			HAL_UART_Transmit(hstream->huart, (uint8_t *)((char[1]){'\n'}), 1, HAL_MAX_DELAY);

			break;

		case int_csv:

			for(int i=0; i<vargn; i++){
				char data_buff[15];
				// Init buffer
				for(int k=0; k < sizeof(data_buff)/sizeof(*data_buff); k++)
					data_buff[k]= '\0';

				// Float to Array!
				itostr(va_arg(vargp, int), data_buff, 0);
				// Polling TX
				HAL_UART_Transmit(hstream->huart, (uint8_t *)data_buff, sizeof(data_buff), timeout);
				// Omit last comma
				if( i != vargn-1)
					HAL_UART_Transmit(hstream->huart, (uint8_t *)((char[1]){','}), sizeof(uint8_t), timeout);
			}
			// Adding a newline
			HAL_UART_Transmit(hstream->huart, (uint8_t *)((char[1]){'\n'}), 1, HAL_MAX_DELAY);

			break;
/*
 *  DA scrivere LA VERSIONE "binary"
 */
		// ---- Streaming values in raw binary mode ----------
		// (In this case the function stream the raw binary
		// representation of the variable)

		case float_binary:
			break;

		case int_binary:
			break;

		default:
			break;

	}

	// DeInit variables arguments
	va_end( vargp );

}


// --- Rx Callback --- //

void DATASTREAM_RxCpltCallback(UART_HandleTypeDef *huart){

	// Checking which stream caused the callback, then assign true value to Rx complete flag
	for(int i=0; i<glbl_idx; i++)
		if(hdatastream[i]->huart == huart){
			hdatastream[i]->RxCpltFlag= true;
			break;
		}

}


// --- Error callback --- //

void DATASTREAM_ErrorCallback(UART_HandleTypeDef *huart){
	__NOP();
}
