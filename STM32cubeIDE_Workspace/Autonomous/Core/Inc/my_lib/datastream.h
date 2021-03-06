/*
 * datastream.h
 *
 *  Created on: Aug 7, 2021
 *      Author: peppe
 */

#ifndef INC_MY_LIB_DATASTREAM_H_
#define INC_MY_LIB_DATASTREAM_H_


// Includes ---------------------------------------------

#include "stm32f4xx_hal.h"
#include <stdbool.h>

// Macros and defines

#define __DATASTREAM_RX_DMA_CPLT(__HANDLE__) (__HANDLE__)->RxCpltFlag

#define MAX_STREAMS 4

// Typedef  ---------------------------------------------

// Status typedef
typedef enum _DATASTREAM_StatusTypeDef{
	DATASTREAM_OK=     0x00U,
	DATASTREAM_ERROR=  0x01U
}DATASTREAM_StatusTypeDef;

// Flag enum to choose streaming mode
typedef enum DATASTREAM_PrintMode{
	int_csv,
	float_csv,
	int_binary,
	float_binary
} DATASTREAM_PrintMode;

// --- Datastream types definition --

typedef struct _DATASTREAM_InitTypeDef{
	UART_HandleTypeDef*   huart;
	bool 			      DMA_Rx_Enable;
	int32_t               DMA_Rx_Stream_IRQn;
}DATASTREAM_InitTypeDef;

typedef struct _DATASTREAM_HandleTypeDef{
	DATASTREAM_InitTypeDef  Init;
	UART_HandleTypeDef*     huart;
	bool 			        DMA_Rx_Enable;
	int32_t                 DMA_Rx_Stream_IRQn;
	volatile bool           RxCpltFlag;
	uint8_t				    id;
}DATASTREAM_HandleTypeDef;


// Functions prototype -----------------------------------

// Init
DATASTREAM_StatusTypeDef DATASTREAM_Init(DATASTREAM_HandleTypeDef* h);
void 					 DATASTREAM_DeInit(void);

// Variadic Function to send data on serial, in blocking mode
void DATASTREAM_Print(DATASTREAM_HandleTypeDef* h, DATASTREAM_PrintMode mode, int vargn, ...);

// Receive in DMA mode
DATASTREAM_StatusTypeDef DATASTREAM_ReceiveDMA(DATASTREAM_HandleTypeDef* h,uint8_t *data, uint32_t data_size);
DATASTREAM_StatusTypeDef DATASTREAM_ReceiveDMA_Reset(DATASTREAM_HandleTypeDef* h);

// Callback functions
void DATASTREAM_RxCpltCallback(UART_HandleTypeDef *huart);
void DATASTREAM_ErrorCallback(UART_HandleTypeDef *huart);

#endif /* INC_MY_LIB_DATASTREAM_H_ */
