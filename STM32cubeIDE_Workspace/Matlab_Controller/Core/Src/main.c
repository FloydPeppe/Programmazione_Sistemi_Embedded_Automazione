/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "my_lib/differential_drive.h"
#include "my_lib/datastream.h"
#include "my_lib/digital_filters.h"

#include "my_data/control_parameters.h"
#include "my_data/diffdrive_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* Definitions for DiffDriveTask */
osThreadId_t DiffDriveTaskHandle;
const osThreadAttr_t DiffDriveTask_attributes = {
  .name = "DiffDriveTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for TxTask */
osThreadId_t TxTaskHandle;
const osThreadAttr_t TxTask_attributes = {
  .name = "TxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for RxTask */
osThreadId_t RxTaskHandle;
const osThreadAttr_t RxTask_attributes = {
  .name = "RxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* USER CODE BEGIN PV */

#ifdef OBSTACLE_AVOIDANCE

/* Definitions for InfraredTask */
osThreadId_t InfraredTaskHandle;
const osThreadAttr_t InfraredTask_attributes = {
  .name = "InfraredTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};

// Entry function prototype
void InfraredSensor(void *argument);

#endif

// Handles of my libraries
DIFFDRIVE_HandleTypeDef hdiffdrive;
MAF_HandleTypeDef hfilter_v, hfilter_om, hfilter_infrared;
DATASTREAM_HandleTypeDef hstream;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM3_Init(void);
void MoveDifferentialDrive(void *argument);
void TxStream(void *argument);
void RxStream(void *argument);

/* USER CODE BEGIN PFP */

static void MAIN_DATASTREAM_Init(void);
static void MAIN_DIFFDRIVE_Init(void);
static void MAIN_MAF_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Global variables shared between tasks
volatile float linear=0, angular= 0;
volatile bool diffDrive_enabled= false;
volatile bool diffDrive_stopped= true;
volatile bool RxStream_enabled= true;
volatile bool RxStream_stopped= false;
volatile bool allow_new_msg= false;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM5_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  // Initializing my libraries
  MAIN_DIFFDRIVE_Init();
  MAIN_DATASTREAM_Init();
  MAIN_MAF_Init();

  // Using the flag "wait_button" to start MCU with WIFI disabled
  // if blue button is pressed during reset. It's useful to avoid
  // battery leakage
  bool wait_for_button= false;
  if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){
   	wait_for_button= true;
   	// Show pressed button
   	while( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){
   		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
   		HAL_Delay(50);
   	}
   	// Turn on LED
   	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
   	// Debouncing released button
   	HAL_Delay(1000);
  }

  // Reset ESP-8266
  HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_RESET);

  // Wait for blue button if this option is on
  if( wait_for_button )
	  while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) )
		  	  ;

  HAL_Delay(1000);

  // Start ESP
  HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_SET);

  // Wait a bit for ESP initialization
  HAL_Delay(3000);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */

  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of DiffDriveTask */
  DiffDriveTaskHandle = osThreadNew(MoveDifferentialDrive, NULL, &DiffDriveTask_attributes);

  /* creation of TxTask */
  TxTaskHandle = osThreadNew(TxStream, NULL, &TxTask_attributes);

  /* creation of RxTask */
  RxTaskHandle = osThreadNew(RxStream, NULL, &RxTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

#ifdef OBSTACLE_AVOIDANCE
  /* creation of InfraredTask */
  InfraredTaskHandle = osThreadNew(InfraredSensor, NULL, &InfraredTask_attributes);
#endif

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 8;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 15;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 15;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 15;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 15;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 460800;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SUPPLY_ENABLE_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : WIFI_RESET_Pin */
  GPIO_InitStruct.Pin = WIFI_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WIFI_RESET_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SUPPLY_ENABLE_Pin LD2_Pin */
  GPIO_InitStruct.Pin = SUPPLY_ENABLE_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : INFRARED_SENSOR_Pin */
  GPIO_InitStruct.Pin = INFRARED_SENSOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INFRARED_SENSOR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static void MAIN_DATASTREAM_Init(){

	DATASTREAM_InitTypeDef DATASTREAM_InitStruct;

	DATASTREAM_InitStruct.DMA_Rx_Enable= true;
	DATASTREAM_InitStruct.DMA_Rx_Stream_IRQn= DMA1_Stream1_IRQn;
	DATASTREAM_InitStruct.huart= &huart3;

	hstream.Init= DATASTREAM_InitStruct;

	DATASTREAM_Init(&hstream);

}


static void MAIN_DIFFDRIVE_Init(void){

	uint16_t max_sample= 1200u;

	hdiffdrive.Init.timing_htim= &htim3;
	hdiffdrive.Init.timing_time_step= TIME_STEP;

	hdiffdrive.Init.encoder_htim[left ]= &htim2;
	hdiffdrive.Init.encoder_htim[right]= &htim5;
	hdiffdrive.Init.encoder_tim_channel[left ]= TIM_CHANNEL_1;
	hdiffdrive.Init.encoder_tim_channel[right]= TIM_CHANNEL_1;
	hdiffdrive.Init.encoder_tim_arr_resolution[left ]= 32;
	hdiffdrive.Init.encoder_tim_arr_resolution[right]= 32;

	hdiffdrive.Init.encoder_resolution[left ]= 1900; //1920; // Number of step each wheel revolution
	hdiffdrive.Init.encoder_resolution[right]= 1900; //1920;

	hdiffdrive.Init.motor_htim[left ][forward ]= &htim1;
	hdiffdrive.Init.motor_htim[left ][backward]= &htim1;
	hdiffdrive.Init.motor_htim[right][forward ]= &htim1;
	hdiffdrive.Init.motor_htim[right][backward]= &htim1;

	hdiffdrive.Init.motor_pwm_channel[left ][forward ]= TIM_CHANNEL_1;
	hdiffdrive.Init.motor_pwm_channel[left ][backward]= TIM_CHANNEL_2;
	hdiffdrive.Init.motor_pwm_channel[right][forward ]= TIM_CHANNEL_3;
	hdiffdrive.Init.motor_pwm_channel[right][backward]= TIM_CHANNEL_4;

	hdiffdrive.Init.motor_max_voltage[left ]= 	 MOTOR_MAX_VOLTAGE;
	hdiffdrive.Init.motor_max_voltage[right]= 	 MOTOR_MAX_VOLTAGE;
	hdiffdrive.Init.motor_min_voltage[left ]= 	 MOTOR_MIN_VOLTAGE;
	hdiffdrive.Init.motor_min_voltage[right]= 	 MOTOR_MIN_VOLTAGE;
	hdiffdrive.Init.motor_supply_voltage[left]=  SUPPLY_VOLTAGE;
	hdiffdrive.Init.motor_supply_voltage[right]= SUPPLY_VOLTAGE;

	hdiffdrive.Init.control_fdfrwrd_arma_AR[left ]= AR_L;
	hdiffdrive.Init.control_fdfrwrd_arma_AR[right]= AR_R;
	hdiffdrive.Init.control_fdfrwrd_arma_MA[left ]= MA_L;
	hdiffdrive.Init.control_fdfrwrd_arma_MA[right]= MA_R;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ar[left ]= N_AR_L;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ar[right]= N_AR_R;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ma[left ]= N_MA_L;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ma[right]= N_MA_R;
	hdiffdrive.Init.control_fdfrwrd_arma_tau[left ]= TAU_L;
	hdiffdrive.Init.control_fdfrwrd_arma_tau[right]= TAU_R;

	hdiffdrive.Init.control_pid_P[left ]= PID_P_L;
	hdiffdrive.Init.control_pid_P[right]= PID_P_R;
	hdiffdrive.Init.control_pid_I[left ]= PID_I_L;
	hdiffdrive.Init.control_pid_I[right]= PID_I_R;
	hdiffdrive.Init.control_pid_D[left ]= PID_D_L;
	hdiffdrive.Init.control_pid_D[right]= PID_D_R;

	hdiffdrive.Init.history_max_sample= max_sample;

	// --- Mechanical parameters (from real model) --- //
	hdiffdrive.Init.mech_prmtr_l= WHEEL_DISTANCE;
	hdiffdrive.Init.mech_prmtr_r= WHEEL_RADIUS;
	hdiffdrive.Init.mech_prmtr_max_wheel_speed= WHEEL_MAX_SPEED;

	// --- Initialize handle --- //
	DIFFDRIVE_Init(&hdiffdrive);

}

// Moving Average Filter initialization --------------------------------------------- //
static void MAIN_MAF_Init(void){

	int filtr_v_size=  2;
	int filtr_om_size= 2;

	int filtr_infrared_size= 100;

	// Null pointer to use a MAF without weights
	void* weights_buff= NULL;

	// Creating filters
	hfilter_v  =       MAF_Create(filtr_v_size,  weights_buff);
	hfilter_om =       MAF_Create(filtr_om_size, weights_buff);
	hfilter_infrared = MAF_Create(filtr_infrared_size, weights_buff);

}


#ifdef OBSTACLE_AVOIDANCE

/* USER CODE BEGIN Header_InfraredSensor */
/**
* @brief Function implementing the InfraredTask thread.
* @param argument: Not used
* @retval None
*/

/* USER CODE END Header_InfraredSensor */
void InfraredSensor(void *argument)
{
  /* USER CODE BEGIN InfraredSensor */

	/* Infinite loop */

	// Infrared variable
	float sens = 0;

	for (;;) {

		// Read current value on IR pin (0 is obstacle detected, 1 is free space)
		sens = HAL_GPIO_ReadPin(INFRARED_SENSOR_GPIO_Port, INFRARED_SENSOR_Pin);

		// Use MAF filter to transform only 0 or 1 state in values that span from 0 to 1
		// (like a normalized distance measure)
		MAF_Update(&hfilter_infrared, sens);

		osDelay(1);
	}
  /* USER CODE END InfraredSensor */
}
#endif

/* USER CODE END 4 */

/* USER CODE BEGIN Header_MoveDifferentialDrive */
/**
  * @brief  Function implementing the DiffDriveTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MoveDifferentialDrive */
void MoveDifferentialDrive(void *argument)
{
  /* USER CODE BEGIN 5 */

	// Trajectory speeds (linear and angular speed)
	float v= 0, om= 0;

	// Trajectory speed limits (theoretically computed below)
	float v_max, om_max;

	// Extracting Max wheels speed (radiant each second)
	float wheel_speed[2];
	float wheel_speed_max= hdiffdrive.mech_prmtr.max_wheel_speed;
	wheel_speed[left ]= wheel_speed_max;
	wheel_speed[right]= wheel_speed_max;

	// Delta_a_max computation (linear variation, is max when wheels rotate at max speed on same direction)
	DIFFDRIVE_WheelSpeed2TrajectorySpeed(&v_max, NULL, wheel_speed);
	v_max *= 1; // Here you could modulate max value

	// Delta_th_max computation (angular variation, is max when wheel rotate at max speed on opposite direction)
	wheel_speed[left]= -wheel_speed_max;
	DIFFDRIVE_WheelSpeed2TrajectorySpeed(NULL, &om_max, wheel_speed);
	om_max *= 1; // Here you could modulate max value

#ifdef OBSTACLE_AVOIDANCE
	// IR sensor variables
	float IR_val;
	float IR_thres= 0.3;
#endif

	// Setting Global flag to indicate that Diff Drive is going to move
	diffDrive_stopped= false;

	// Start control
	DIFFDRIVE_TrackingStart();

	/* Infinite loop */
	for(;;){

		// If movement is enabled
		if( diffDrive_enabled ){

			v=  linear;
			om= angular;

			// Enroll angular speed to a fixed range
			if(fabsf(om) > om_max)
				// x/|x| == sign(x)
				om = om_max*om/fabsf(om);

			//
			// Compute v_max, to find a feasible trajectory, i.e. that respect wheel max speed rate.
			// Here I fix omega (angular speed), computed above, and I find the current max (feasible) linear value
			// Look at chapter 2, expression 1.11, on the links below (on STM32CubeIDE press CTRL and then click). You
			// have just to find the max or min v value (min is -max) with the constraints of omega fixed and
			// wheel_speed between the fixed range:
			//
			// https://github.com/FloydPeppe/Programmazione_Sistemi_Embedded_Automazione/blob/master/Relazione/Relazione_Progetto.pdf
			//
			float l = hdiffdrive.mech_prmtr.l;
			float r = hdiffdrive.mech_prmtr.r;
			if( om >= 0 )
				v_max = r * wheel_speed_max - om * l;
			else
				v_max = r * wheel_speed_max + om * l;

			// Enroll linear speed to fixed range
			if(fabsf(v) > v_max)
				// x/|x| == sign(x)
				v = v_max*v/fabsf(v);

			// Update filters to smooth a bit input trajectory
			MAF_Update(&hfilter_v,  v);
			MAF_Update(&hfilter_om, om);

			// Get current values
			v=  __MAF_GET_VAL(&hfilter_v);
			om= __MAF_GET_VAL(&hfilter_om);

			// If diff drive has been previously stopped
			if( diffDrive_stopped ){
				// Restart control
				DIFFDRIVE_TrackingResume();
				// Set flag to indicate that is going to move
				diffDrive_stopped= false;
			}

#ifdef OBSTACLE_AVOIDANCE
			// Only in forward direction (infrared sensor is in front of diff drive)
			if( v > 0 ){
				// v enveloped by infrared sensor value
				v *= (IR_val = __MAF_GET_VAL(&hfilter_infrared));

				//IR_val = __MAF_GET_VAL(&hfilter_infrared);

				// If IR_val < IR_thres, v = 0
				if( IR_val < IR_thres )
					v = 0;
			}
#endif

			// Timing the external transmitter periodically setting this flag
			allow_new_msg= true;

			// Make step control
			DIFFDRIVE_TrackingStep(v, om);

		}
		// if movement is disabled and diffDrive is moving
		else if( !diffDrive_stopped ){
			// Suspend control (stop diff drive)
			DIFFDRIVE_TrackingSuspend();

			// Reset filters
			MAF_Reset(&hfilter_v,  0);
			MAF_Reset(&hfilter_om, 0);

			// Set global flag to indicate that diff drive is now stopped
			diffDrive_stopped= true;
		}


	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_TxStream */
/**
* @brief Function implementing the TxTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TxStream */
void TxStream(void *argument)
{
  /* USER CODE BEGIN TxStream */

	// Variables I need later
	DIFFDRIVE_STATE_TypeDef state_measure, state_target;;

	/* Infinite loop */
	for(;;)
	{

		// Blocking the external transmitter until a new message is allowed. DiffDrive task periodically set true
		// the below flag (the external transmitter must be set to wait until received data before to send new more)
		if( allow_new_msg ){

			// Get current history sample
			DIFFDRIVE_HISTORY_Get_Sample(&state_measure,NULL);

			// Send diffDrive current pose to serial connected to ESP WiFi module (this trigger the external transmitter
			// to send new data)
			DATASTREAM_Print(
							&hstream, float_csv, 3,
							state_measure.pose.x_a,
							state_measure.pose.y_a,
							state_measure.pose.theta
			);

			// Resetting the flag
			allow_new_msg= false;
		}

		// If blue button is pressed
		if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

			// When I'm here, I use HAL_Delay(ms) and not osDelay(ms), because during Tx stream
			// I suspend other theads, so I don't need os timing. Moreover, osDelay sometimes hangs forever
			// without no reason, I guess 'cause other threads are suspended and scheduler try to wait for them.

			// Stop Rx stream
			RxStream_enabled= false;

			// Wait until Rx is stream is stopped
			while( !RxStream_stopped )
				;

			// Stop Differential Drive motion
			diffDrive_enabled= false;

			// Wait until diff drive is still
			while( !diffDrive_stopped )
				;

			// Suspend other threads while sending values
			osThreadSuspend(DiffDriveTaskHandle);
#ifdef OBSTACLE_AVOIDANCE
			osThreadSuspend(InfraredTaskHandle);
#endif
			osThreadSuspend(RxTaskHandle);

			// Turn on LED
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

			// Reset ESP-8266
			HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_RESET);

			osDelay(400);

			// Send values forever
			for(;;){

				// If blue button is pressed again
				if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

					// Enable ESP-8266
					HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_SET);

					// Rapidly flash led to show MCU ready to send values
					for(int i=0; i<20; i++){
						HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
						osDelay(50);
					}

					// Turn off LED
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

					// Wait for 10 received chars, receiver must send something before to start receiving
					char ch[10];
					HAL_UART_Receive(&huart3, (uint8_t*) ch, sizeof(ch), HAL_MAX_DELAY);

					// Wait a bit
					osDelay(2000);

					// Histroy variable
					DIFFDRIVE_HISTORY_TypeDef tmphist= hdiffdrive.history;

					// Number of saved samples
					int N_sample;
					if( tmphist.sample_cnt < tmphist.max_sample )
						N_sample=  tmphist.sample_cnt;
					else
						N_sample=  tmphist.max_sample;

					// First values send is the actual number of samples
					DATASTREAM_Print(&hstream, int_csv, 1, N_sample);
					// Bit of transfer delay
					osDelay(5);

					// Init History Get function
					DIFFDRIVE_HISTORY_Get_Init();

					for(int k= 0; k < N_sample; k++){

						// Get one state sample from history
						DIFFDRIVE_HISTORY_Get_Sample(&state_measure, &state_target);

						// Send values to serial connected to ESP WiFi module
						DATASTREAM_Print(
								&hstream, float_csv, 18,
								state_measure.input[left], state_measure.input[right],
								state_target .input[left], state_target .input[right],
								state_measure.wheel[left], state_measure.wheel[right],
								state_target .wheel[left], state_target .wheel[right],
								state_measure.pose.x_a,    state_measure.pose.y_a,
								state_measure.pose.theta,  state_measure.pose.v,
								state_measure.pose.om,
                                state_target .pose.x_a,    state_target.pose.y_a,
								state_target .pose.theta,  state_target.pose.v,
								state_target .pose.om
						);
						// Bit of transfer delay
						osDelay(5);
						// Toggling led to show data transfer operation
						if( !(k%50) )
							HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
					}

					// Turn off LED
					HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

					// Reset ESP-8266
					HAL_GPIO_WritePin(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin, GPIO_PIN_RESET);
				}
			}
		}
	}
  /* USER CODE END TxStream */
}

/* USER CODE BEGIN Header_RxStream */
/**
* @brief Function implementing the RxTask thread.
* @param argument: Not used
* @retval None
*/

//#define STOP_WHEN_ZERO_ENTRY_

/* USER CODE END Header_RxStream */
void RxStream(void *argument)
{
  /* USER CODE BEGIN RxStream */
	/* Infinite loop */

	// Debug array
	uint32_t running_time[100]={0};
	int k_= 0;

	// Communication timeout of milliseconds (set osWaitForever for no timeout)
	uint32_t timeout_time= 2000; //osWaitForever;

#ifdef CSV_PACKET

	// Strings
	char str_1[20], str_2[20];

	// Data buffer
	uint8_t data[74];

#else
#ifdef JSON_PACKET

	// Strings
	char str_1[20], str_2[20];

	// Data buffer
	uint8_t data[200];

	// Target strings to find on packet
	char target_str[]= "value"; //"gravity";
	char curr_str[]=   "value"; //"gravity";
#else
#ifdef BINARY_PACKET

	// Float values expected to extract
	uint8_t n_values= 2;

	// Repetition of the packet
	uint8_t packet_repeat= 2;

	// Actual number of float received (n_values+1 because one float is an "inf" to show where the packet start)
	uint8_t n_float= (n_values+1)*packet_repeat;

	// Number of byte
	uint8_t n_byte= n_float*sizeof(float);

	// Data buffer in byte
	uint8_t data[n_byte];

#endif /* CSV_PACKET    */
#endif /* JSON_PACKET   */
#endif /* BINARY_PACKET */

	// Keep LED on to show that data transfer is down
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

	// Start DMA Rx (Rx_flag will be connected with the flag inside datastream handle struct)
	DATASTREAM_ReceiveDMA(&hstream, data, sizeof(data));

	// Wait for a received input
	while( !__DATASTREAM_RX_DMA_CPLT(&hstream) )
		__NOP();

	// LED off to show that data transfer is up
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	// Setting Global flag to enable Diff Drive
	diffDrive_enabled= true;

	// Measure time
	uint32_t tic= osKernelGetTickCount();
	uint32_t toc;

	for(;;){

		// If data is received
		if( __DATASTREAM_RX_DMA_CPLT(&hstream) ){

			// Debug time
			if( k_ < sizeof(running_time)/sizeof(*running_time) )
				running_time[k_++]= osKernelGetTickCount()-tic;

#ifdef BINARY_PACKET

			uint8_t tmp_data[n_byte];
			float curr_val;
			// FLip array of byte data (I guess Matlab send with different endiannes)
			for( int i=0; i<n_byte; i++)
				tmp_data[n_byte-1-i]= data[i];

			// Find the index where the "inf" values is, the packet start from there
			int idx_start=0;
			while( idx_start < n_byte && !isinf(curr_val= *((float*)(tmp_data+idx_start++))) )
				;

			// If the data, read from the found index, contain n float values
			// If the data, read from the found index, contain 2 float
			if( --idx_start < n_byte-sizeof(float)*n_values ){
				// First value I send is the last I receive (from Matlab Transmitter I send first linear and then angular
				// here I receive first angular and then linear)
				angular= *((float*)(tmp_data+idx_start+(n_values-1)*sizeof(float)));
				linear=  *((float*)(tmp_data+idx_start+(n_values-0)*sizeof(float)));
			}

#else

			int start_idx=0;

#ifdef CSV_PACKET

			// Find message start index (where the '\n' char is)
			while( data[start_idx++] != '\n' && start_idx < sizeof(data) )
				;

#else
#ifdef JSON_PACKET

			// Find message start index from JSON packet (where the "value" string ends, use debugger to see how
			// packet is formatted)
			do{
				for(int j=0; j<sizeof(curr_str)-1; j++)
					curr_str[j]= data[start_idx+j];
				curr_str[sizeof(curr_str)-1]= '\0';

				start_idx++;

			}while( strcmp(curr_str,target_str) && start_idx < sizeof(data)+sizeof(curr_str) );

			start_idx= start_idx+sizeof(target_str)+1;

#endif /* JSON_PACKET */
#endif /* CSV_PACKET */

			// Read CSV message (data itself is CSV formatted both for JSON and pure CSV packet)
			int i= start_idx;
			while( data[i] != ',' &&  i-start_idx < sizeof(str_1) && i < sizeof(data) ){
				str_1[i-start_idx] = data[i];
				i++;
			}
			str_1[i-start_idx]= '\0';

			start_idx= ++i;
			while( data[i] != ',' && data[i] != '\n' && i-start_idx < sizeof(str_2) && i < sizeof(data) ){
				str_2[i-start_idx] = data[i];
				i++;
			}
			str_2[--i-start_idx]= '\0';

			// Convert in float values
			linear= (float)atof((char*) str_1);
			angular= (float)atof((char*) str_2);

	//		linear /= 1e8;
	//		angular /= 1e8;

#ifdef STOP_WHEN_ZERO_ENTRY_
			// If values are both zero stop diff drive
			if( linear == 0 && angular == 0 )
				diffDrive_enabled= false;
			else
				diffDrive_enabled= true;
#endif /* STOP_WHEN_ZERO_ENTRY_ */


#endif /* BINARY_PACKET */

			// Try resetting
			//DATASTREAM_ReceiveDMA_Reset(&hstream);

			// Ready for receive new messages
			DATASTREAM_ReceiveDMA(&hstream, data, sizeof(data));

			// Get current time
			tic= osKernelGetTickCount();
		}


		// Stopping Rx if flag is at reset value
		while( !RxStream_enabled ){
			if( !RxStream_stopped ){
				DATASTREAM_ReceiveDMA_Reset(&hstream);
				RxStream_stopped= true;
			}
		}

		// If data aren't received for a fixed period of time
		if( (toc= osKernelGetTickCount()-tic) > timeout_time ){

			// Stop Differential Drive motion
			diffDrive_enabled= false;

			// Wait until diff drive is still
			while( !diffDrive_stopped )
				;

			// Turn on LED to show that connection is down
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

			// Try resetting uart Rx and DMA interrupt
			DATASTREAM_ReceiveDMA_Reset(&hstream);

			// Try to start a new DMA receiving request
			DATASTREAM_ReceiveDMA(&hstream, data, sizeof(data));

			// Wait for input
			while( !__DATASTREAM_RX_DMA_CPLT(&hstream) ){

				// While receiving is disabled keep loop here before to reset
				while( !RxStream_enabled )
					RxStream_stopped= true;

				// If more time elapses, without received data, reset system
				if( (osKernelGetTickCount()-tic) > 3*timeout_time ){
					// Reset MCU
					HAL_NVIC_SystemReset();
				}
			}

			// Turn on LED to show that connection is up
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			// Resume other threads
			osThreadResume(DiffDriveTaskHandle);
#ifdef OBSTACLE_AVOIDANCE
			osThreadResume(InfraredTaskHandle);
#endif
			// Enable diffDrive
			diffDrive_enabled= true;
		}

	}

  /* USER CODE END RxStream */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
