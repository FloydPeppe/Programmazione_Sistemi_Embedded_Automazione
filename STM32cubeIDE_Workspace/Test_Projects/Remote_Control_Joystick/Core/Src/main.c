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
#include "my_lib/retarget.h"
#include "my_lib/datastream.h"
#include "my_lib/datastream_utils.h"
#include "my_lib/digital_filters.h"

#include "my_data/identification.h"

#include <stdio.h>
#include <stdlib.h>



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
DMA_HandleTypeDef hdma_usart3_tx;

/* Definitions for DiffDrive */
osThreadId_t DiffDriveHandle;
const osThreadAttr_t DiffDrive_attributes = {
  .name = "DiffDrive",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for Communication */
osThreadId_t CommunicationHandle;
const osThreadAttr_t Communication_attributes = {
  .name = "Communication",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for InfraredSens */
osThreadId_t InfraredSensHandle;
const osThreadAttr_t InfraredSens_attributes = {
  .name = "InfraredSens",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* USER CODE BEGIN PV */

DIFFDRIVE_HandleTypeDef hdiffdrive;
MAF_HandleTypeDef hfilter_a, hfilter_th, hfilter_infrared;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_USART3_UART_Init(void);
void MoveDifferentialDrive(void *argument);
void SerialCommunication(void *argument);
void InfraredSensor(void *argument);

/* USER CODE BEGIN PFP */

static void MAIN_DIFFDRIVE_Init(void);
static void MAIN_MAF_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int x_val=0, y_val=0, x_val_prev=0, y_val_prev=0;

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
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  MAIN_DIFFDRIVE_Init();
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
   	// Turn off LED
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

  // Keep LED on to show that data transfer is down
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

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
  /* creation of DiffDrive */
  DiffDriveHandle = osThreadNew(MoveDifferentialDrive, NULL, &DiffDrive_attributes);

  /* creation of Communication */
  CommunicationHandle = osThreadNew(SerialCommunication, NULL, &Communication_attributes);

  /* creation of InfraredSens */
  InfraredSensHandle = osThreadNew(InfraredSensor, NULL, &InfraredSens_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
  htim3.Init.Prescaler = 840-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1000-1;
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
  huart3.Init.BaudRate = 115200;
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
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

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

static void MAIN_DIFFDRIVE_Init(void){

	float Ts= 0.01;
	int max_sample= 1400;

	hdiffdrive.Init.timing_htim= &htim3;
	hdiffdrive.Init.timing_time_step= Ts;

	hdiffdrive.Init.encoder_htim[left ]= &htim2;
	hdiffdrive.Init.encoder_htim[right]= &htim5;
	hdiffdrive.Init.encoder_channel[left ]= TIM_CHANNEL_1;
	hdiffdrive.Init.encoder_channel[right]= TIM_CHANNEL_1;

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

	hdiffdrive.Init.motor_max_voltage[left ]= 	  MOTOR_MAX_VOLTAGE;
	hdiffdrive.Init.motor_max_voltage[right]= 	  MOTOR_MAX_VOLTAGE;
	hdiffdrive.Init.motor_min_voltage[left ]= 	  MOTOR_MIN_VOLTAGE;
	hdiffdrive.Init.motor_min_voltage[right]= 	  MOTOR_MIN_VOLTAGE;
	hdiffdrive.Init.motor_supply_voltage[left]=   SUPPLY_VOLTAGE;
	hdiffdrive.Init.motor_supply_voltage[right]=  SUPPLY_VOLTAGE;

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

	hdiffdrive.Init.control_pid_P[left ]= 40;  //33.3;
	hdiffdrive.Init.control_pid_P[right]= 40;  //33.3;
	hdiffdrive.Init.control_pid_I[left ]= 8;   //7.6;
	hdiffdrive.Init.control_pid_I[right]= 8;   //7.6;
	hdiffdrive.Init.control_pid_D[left ]= 0;
	hdiffdrive.Init.control_pid_D[right]= 0;

	hdiffdrive.Init.history_max_sample= max_sample;

	hdiffdrive.Init.mech_prmtr_l=               0.155/2;
	hdiffdrive.Init.mech_prmtr_r=               0.069/2;
	hdiffdrive.Init.mech_prmtr_max_wheel_speed= 16;      // 16 rad/s, from DC motor specifics


	DIFFDRIVE_Init(&hdiffdrive);

}

// Moving Average Filter initialization --------------------------------------------- //
static void MAIN_MAF_Init(void){

	int filtr_a_size= 		 60;
	int filtr_th_size=		 60;
	int filtr_infrared_size= 100;
	// Null pointer to use a MAF without weights
	void* weights_buff= NULL;

	// Creating filters
	hfilter_a  = MAF_Create(filtr_a_size,  weights_buff);
	hfilter_th = MAF_Create(filtr_th_size, weights_buff);
	hfilter_infrared = MAF_Create(filtr_infrared_size, weights_buff);

}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_MoveDifferentialDrive */
/**
  * @brief  Function implementing the DiffDrive thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MoveDifferentialDrive */
void MoveDifferentialDrive(void *argument)
{
  /* USER CODE BEGIN 5 */
	float delta_a, delta_th;
	float delta_a_max= 3.8e-3, delta_th_max= 4e-2, delta_a_max_curr;

	float IR_val;

	bool stop_flag= true;

	/* Infinite loop */
	for(;;){

		if( delta_th > 0 )
			delta_a_max_curr= delta_a_max * (1-delta_th/delta_th_max*0.5);
		else
			delta_a_max_curr= delta_a_max * (1+delta_th/delta_th_max*0.5);

		if( x_val != 0 || y_val != 0  ){

			delta_a  = (float)x_val/512 * delta_a_max_curr;
			delta_th = (float)y_val/512 * delta_th_max;

			if( stop_flag ){
				DIFFDRIVE_TrackingStart();
				stop_flag= false;
			}
		}
		else{
			DIFFDRIVE_TrackingStop();
			MAF_Reset(&hfilter_a, 0);
			MAF_Reset(&hfilter_th, 0);

			stop_flag= true;

			delta_a=  0;
			delta_th= 0;
		}

		// Update filters
		MAF_Update(&hfilter_a,  delta_a);
		MAF_Update(&hfilter_th, delta_th);

		// Get current values
		delta_a=  __MAF_GET_VAL(&hfilter_a);
		delta_th= __MAF_GET_VAL(&hfilter_th);

		// Infrared sensor value
		IR_val = __MAF_GET_VAL(&hfilter_infrared);

		// If IR_val == 0 delta_a = 0
		if( delta_a > 0 && IR_val < 0.5 )
			delta_a = 0;

		// Make step control
		if( !stop_flag )
			DIFFDRIVE_TrackingStep(delta_a, delta_th);

	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_SerialCommunication */
/**
* @brief Function implementing the Communication thread.
* @param argument: Not used
* @retval None
*/

volatile bool data_received= false;
void UART_RxCpltCallback(UART_HandleTypeDef *huart){
	data_received = true;
}
/*
volatile bool data_transmitted= false;
void UART_TxCpltCallback(UART_HandleTypeDef *huart){
	data_transmitted = true;
}
*/
/* USER CODE END Header_SerialCommunication */
void SerialCommunication(void *argument)
{
  /* USER CODE BEGIN SerialCommunication */
	/* Infinite loop */

	// Values threshold
	int val_thres= 10;

	// Assign uart Rx complete callback (function defined above in this file)
	huart3.RxCpltCallback= UART_RxCpltCallback;
//	huart3.TxCpltCallback= UART_TxCpltCallback;

	uint8_t data[20];
	char tmp_str_1[5], tmp_str_2[5];
/*
	tmp_str_1[0]= 'c';
	data_transmitted= false;
	HAL_UART_Transmit_DMA(&huart3, (uint8_t*)tmp_str_1, sizeof(tmp_str_1));
	while( !data_transmitted )
		__NOP();
*/
	// Wait for a good input
	data[0]= '\0';
	bool bad_input= true;
	while( bad_input ){
		data_received= false;
		HAL_UART_Receive_DMA(&huart3, data, sizeof(data));
		while( !data_received )
			__NOP();
		for(char ch=48; ch<58; ch++ )
			if( data[0] == ch ){
				bad_input= false;
				break;
			}
		if( bad_input && (data[0] == 's' || data[0] == 'e' || data[0] == ' ') )
			bad_input= false;
	}

	// LED off to show that data transfer is up
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	// Measure time
	uint32_t tic= osKernelGetTickCount();
	uint32_t toc;

	for(;;){

		if( data_received ){

			// Find message packet start index (where the 's' char is)
			int start_idx;
			for(start_idx= 0; data[start_idx] != 's'; start_idx++)
				;

			// Read message
			for(int i=start_idx+1; i<start_idx+5; i++)
				tmp_str_1[i-(1+start_idx)] = data[i];
			tmp_str_1[4]= '\0';
			for(int i=start_idx+5; i<start_idx+9; i++)
				tmp_str_2[i-(5+start_idx)] = data[i];
			tmp_str_2[4]= '\0';

			// Convert in integer values
			x_val= (float)atoi((char*) tmp_str_1);
			y_val= (float)atoi((char*) tmp_str_2);

			// Convert values from 0 to 1023 to about -512 to 512 (values are generated from a joystick,
			// connected to an Arduino with a 10 bit resolution ADC)
			x_val= -(1023-x_val-512+1); // Inverted values
			y_val=  (1023-y_val-512+9);

			// Set zero values below a threshold to reject noise
			if( abs(x_val) < val_thres )
				x_val= 0;
			if( abs(y_val) < val_thres )
				y_val= 0;

			// Ready for new messages
			data_received= false;
			HAL_UART_Receive_DMA(&huart3, data, sizeof(data));

			tic= osKernelGetTickCount();

		}

		// If data aren't received for a period of time
		if( (toc= osKernelGetTickCount()-tic) > 1000 ){
			// Set values to zero
			x_val= 0;
			y_val= 0;

			osDelay(100);

			// Suspend other thread
			osThreadSuspend(DiffDriveHandle);

			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			// Reset uart Rx and DMA interrupt
			HAL_NVIC_DisableIRQ(DMA1_Stream1_IRQn);
			huart3.RxState= HAL_UART_STATE_READY;
			HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

			// Start a new DMA receiving request
			HAL_UART_Receive_DMA(&huart3, data, sizeof(data));

			// Wait for input
			while( !data_received ){
				// If more time elapses, without data, reset system
				if( (osKernelGetTickCount()-tic) > 6*toc ){

					// Reset MCU
					HAL_NVIC_SystemReset();

				}
			}

			osDelay(1000);
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

			// Resume other thread
			osThreadResume(DiffDriveHandle);

		}

	}

  /* USER CODE END SerialCommunication */
}

/* USER CODE BEGIN Header_InfraredSensor */
/**
* @brief Function implementing the InfraredSens thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_InfraredSensor */
void InfraredSensor(void *argument)
{
  /* USER CODE BEGIN InfraredSensor */

  int sens=0, k=0;
  (void) k;

  /* Infinite loop */
  for(;;)
  {
	  sens= HAL_GPIO_ReadPin(INFRARED_SENSOR_GPIO_Port, INFRARED_SENSOR_Pin);

	  MAF_Update(&hfilter_infrared, sens);

	  osDelay(1);
  }
  /* USER CODE END InfraredSensor */
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
