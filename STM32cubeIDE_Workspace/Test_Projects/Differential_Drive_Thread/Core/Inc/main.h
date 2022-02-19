/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define ULTRASONIC_TRIGGER_Pin GPIO_PIN_0
#define ULTRASONIC_TRIGGER_GPIO_Port GPIOA
#define LEFT_MOTOR_2_Pin GPIO_PIN_1
#define LEFT_MOTOR_2_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SUPPLY_ENABLE_Pin GPIO_PIN_4
#define SUPPLY_ENABLE_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define ULTRASONIC_ECHO_Pin GPIO_PIN_6
#define ULTRASONIC_ECHO_GPIO_Port GPIOA
#define RIGHT_MOTOR_2_Pin GPIO_PIN_2
#define RIGHT_MOTOR_2_GPIO_Port GPIOB
#define RIGHT_MOTOR_1_Pin GPIO_PIN_10
#define RIGHT_MOTOR_1_GPIO_Port GPIOB
#define SENSOR_RIGHT_UP_Pin GPIO_PIN_6
#define SENSOR_RIGHT_UP_GPIO_Port GPIOC
#define SENSOR_RIGHT_DOWN_Pin GPIO_PIN_7
#define SENSOR_RIGHT_DOWN_GPIO_Port GPIOC
#define SENSOR_LEFT_DOWN_Pin GPIO_PIN_8
#define SENSOR_LEFT_DOWN_GPIO_Port GPIOA
#define SENSOR_LEFT_UP_Pin GPIO_PIN_9
#define SENSOR_LEFT_UP_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define LEFT_MOTOR_1_Pin GPIO_PIN_15
#define LEFT_MOTOR_1_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/