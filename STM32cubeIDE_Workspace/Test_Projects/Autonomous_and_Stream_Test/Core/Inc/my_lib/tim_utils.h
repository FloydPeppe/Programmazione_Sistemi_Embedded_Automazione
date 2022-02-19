/*
 * tim_utils.h
 *
 *  Created on: Oct 7, 2021
 *      Author: peppe
 */

#ifndef INC_MY_LIB_TIM_UTILS_H_
#define INC_MY_LIB_TIM_UTILS_H_


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_rcc.h"


// Macros to get timers clock source

#define __MYLIB_GET_APB1TIM_CLK																\
	/* Is PCLK1 prescaler equal to 1? If yes clock value is PCLK1 else is 2*PCLK1  */ 		\
	((RCC->CFGR & RCC_CFGR_PPRE1) == 0) ? HAL_RCC_GetPCLK1Freq() : HAL_RCC_GetPCLK1Freq()*2 \

#define __MYLIB_GET_APB2TIM_CLK																\
	/* Is PCLK2 prescaler equal to 1? If yes clock value is PCLK2 else is 2*PCLK2  */ 		\
	((RCC->CFGR & RCC_CFGR_PPRE2) == 0) ? HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK2Freq()*2 \

/* You should look at Reference Manual to know which bus the timer belong to */
#define __MYLIB_GET_TIMER_CLK(__HANDLE__) 												   	\
	(((__HANDLE__)->Instance==TIM1   || (__HANDLE__)->Instance==TIM8  ||    			    \
	  (__HANDLE__)->Instance==TIM9   || (__HANDLE__)->Instance==TIM10 ||     				\
	  (__HANDLE__)->Instance==TIM11 ) ?	__MYLIB_GET_APB2TIM_CLK : __MYLIB_GET_APB1TIM_CLK ) \


#endif /* INC_MY_LIB_TIM_UTILS_H_ */
