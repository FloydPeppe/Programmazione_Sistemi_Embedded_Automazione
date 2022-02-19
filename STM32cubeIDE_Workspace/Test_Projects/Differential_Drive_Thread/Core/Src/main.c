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
#include "my_lib/idinput_data.h"
#include "my_lib/reference_data.h"
#include "my_lib/identification_data.h"
#include "my_lib/digital_filters.h"
#include "my_lib/utils.h"

#include "stdbool.h"

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
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim12;

UART_HandleTypeDef huart2;

/* Definitions for ControlDiffDriv */
osThreadId_t ControlDiffDrivHandle;
const osThreadAttr_t ControlDiffDriv_attributes = {
  .name = "ControlDiffDriv",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for DistanceSensor */
osThreadId_t DistanceSensorHandle;
const osThreadAttr_t DistanceSensor_attributes = {
  .name = "DistanceSensor",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime1,
};
/* USER CODE BEGIN PV */

DIFFDRIVE_HandleTypeDef hdiffdrive;
MAF_HandleTypeDef hfilter;


// Debug variable
int check= 0;

/*
short int counter_L[N_sample];
short int counter_R[N_sample];
float input_L[N_sample+1];
float input_R[N_sample+1];
float t[N_sample];
*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM12_Init(void);
void StartControlDiffDrive(void *argument);
void StartDistanceSensor(void *argument);

/* USER CODE BEGIN PFP */

static void MAIN_DIFFDRIVE_Init(void);
static void MAIN_MAF_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

bool flag_update;
bool flag_send= false;

float* x= DELTA_A_ref;
float* y= DELTA_THETA_ref;

float delta_a=0, delta_th=0;


void delay_us(uint32_t us){
	__HAL_TIM_SET_COUNTER(&htim12, 0);
	while ((__HAL_TIM_GET_COUNTER(&htim12)) < us)
		;
}

float HCSR04_read (void){
	uint32_t echo_time = 0;

	// Start Timer
	HAL_TIM_Base_Start(&htim12);

	// Pull TRIG pin low for at least 2 us
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	delay_us(2);

	// Keep TRIG pin High for 10 us for start the sensor
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);

	// Wait for the ECHO pin to go high
	while ( !(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6)) );

	// Measure time for which the pin is high
	__HAL_TIM_SET_COUNTER(&htim12, 0);
	while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6))
		;

	// Measured Time
	echo_time= __HAL_TIM_GET_COUNTER(&htim12);

	// Stop Timer
	HAL_TIM_Base_Stop(&htim12);

	// Measure distance in cm (sound speed on air is about 0.0343 cm/us, us := microseconds)
	float distance  = (float)echo_time * (float)(0.0343)/2;

	return distance;
}


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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM8_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM12_Init();
  /* USER CODE BEGIN 2 */

  // Initialize
  DATASTREAM_Init(&huart2);
  MAIN_DIFFDRIVE_Init();
  MAIN_MAF_Init();

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
  /* creation of ControlDiffDriv */
  ControlDiffDrivHandle = osThreadNew(StartControlDiffDrive, NULL, &ControlDiffDriv_attributes);

  /* creation of DistanceSensor */
  DistanceSensorHandle = osThreadNew(StartDistanceSensor, NULL, &DistanceSensor_attributes);

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 8-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

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
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 84-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 8400-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 1000000-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
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
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM12 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM12_Init(void)
{

  /* USER CODE BEGIN TIM12_Init 0 */

  /* USER CODE END TIM12_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM12_Init 1 */

  /* USER CODE END TIM12_Init 1 */
  htim12.Instance = TIM12;
  htim12.Init.Prescaler = 84-1;
  htim12.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim12.Init.Period = 0xffff;
  htim12.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim12.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim12, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  HAL_GPIO_WritePin(GPIOA, ULTRASONIC_TRIGGER_Pin|SUPPLY_ENABLE_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ULTRASONIC_TRIGGER_Pin SUPPLY_ENABLE_Pin LD2_Pin */
  GPIO_InitStruct.Pin = ULTRASONIC_TRIGGER_Pin|SUPPLY_ENABLE_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ULTRASONIC_ECHO_Pin */
  GPIO_InitStruct.Pin = ULTRASONIC_ECHO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ULTRASONIC_ECHO_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static void MAIN_DIFFDRIVE_Init(void){

	float Ts= 0.01;
	int max_sample= 2000;

	hdiffdrive.Init.timing_htim= &htim3;
	hdiffdrive.Init.timing_time_step= Ts;

	hdiffdrive.Init.sensor_htim[left ]= &htim1;
	hdiffdrive.Init.sensor_htim[right]= &htim8;
	hdiffdrive.Init.sensor_channel[left ]= TIM_CHANNEL_1;
	hdiffdrive.Init.sensor_channel[right]= TIM_CHANNEL_1;

	hdiffdrive.Init.motor_htim[left ]= &htim2;
	hdiffdrive.Init.motor_htim[right]= &htim2;

	hdiffdrive.Init.motor_pwm_channel[left ][forward ]= TIM_CHANNEL_1;
	hdiffdrive.Init.motor_pwm_channel[left ][backward]= TIM_CHANNEL_2;
	hdiffdrive.Init.motor_pwm_channel[right][forward ]= TIM_CHANNEL_3;
	hdiffdrive.Init.motor_pwm_channel[right][backward]= TIM_CHANNEL_4;

	hdiffdrive.Init.control_fdfrwrd_arma_AR[left ]= AR_L;
	hdiffdrive.Init.control_fdfrwrd_arma_AR[right]= AR_R;
	hdiffdrive.Init.control_fdfrwrd_arma_MA[left ]= MA_L;
	hdiffdrive.Init.control_fdfrwrd_arma_MA[right]= MA_R;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ar[left ]= N_AR_L;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ar[right]= N_AR_R;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ma[left ]= N_MA_L;
	hdiffdrive.Init.control_fdfrwrd_arma_n_ma[right]= N_MA_R;
	hdiffdrive.Init.control_fdfrwrd_arma_tau[left ]=  TAU_L;
	hdiffdrive.Init.control_fdfrwrd_arma_tau[right]=  TAU_R;

	hdiffdrive.Init.control_pid_P[left ]= 0.40835;
	hdiffdrive.Init.control_pid_P[right]= 0.40835;
	hdiffdrive.Init.control_pid_I[left ]= 0.27005;
	hdiffdrive.Init.control_pid_I[right]= 0.27005;
	hdiffdrive.Init.control_pid_D[left ]= 0;
	hdiffdrive.Init.control_pid_D[right]= 0;

	hdiffdrive.Init.history_max_sample[left]=  max_sample;
	hdiffdrive.Init.history_max_sample[right]= max_sample;

	hdiffdrive.Init.mech_prmtr.l= 0.127/2;
	hdiffdrive.Init.mech_prmtr.r= 0.0659/2;

	DIFFDRIVE_Init(&hdiffdrive);

}

// Moving Average Filter initialization --------------------------------------------- //

static void MAIN_MAF_Init(void){

	int filtr_size= 4;

	// Values buffer
	float* fltr_buff = (float*)calloc(filtr_size,sizeof(float));
	// Null pointer to use a MAF without weights
	void* weights_buff= NULL;

	// Creating filter
	hfilter= MAF_Create(fltr_buff, filtr_size, weights_buff);


}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartControlDiffDrive */
/**
  * @brief  Function implementing the ControlDiffDriv thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartControlDiffDrive */
void StartControlDiffDrive(void *argument)
{
  /* USER CODE BEGIN 5 */

	while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) );
		;

	// Enabling Supply Voltage
	HAL_GPIO_WritePin(SUPPLY_ENABLE_GPIO_Port, SUPPLY_ENABLE_Pin, GPIO_PIN_SET);
	osDelay(1000);

	DIFFDRIVE_CONTROL_TypeDef *hctrl= hdiffdrive.control;

	float newest_target[2]= {0, 0};
	float delta_theta;

	// Init feed-forward buffer (with NULL value in second argument the buffers are initialized with zeros)
	FEEDFORWARD_Buffer_Init(&hctrl[left ].fdfrwrd, NULL);
	FEEDFORWARD_Buffer_Init(&hctrl[right].fdfrwrd, NULL);

	__HAL_TIM_SET_COUNTER(&htim5,0);
	HAL_TIM_Base_Start(&htim5);

	// Reset encoder sensor' counter
	DIFFDRIVE_SENSOR_Reset();

	// Starting and enabling history
	DIFFDRIVE_HISTORY_Start();

	// Start control timing
	DIFFDRIVE_TIMING_Start();

	int i=0;
	long int temp[10];

  /* Infinite loop */
  for(;;)
  {
	  __HAL_TIM_SET_COUNTER(&htim5,0);
//	  if(flag_update)
//		  delta_theta= delta_th;

	  // Computing newest Target
	  DIFFDRIVE_Compute_Wheel_Target(newest_target, delta_a, delta_th);

	  // Control wheels
	  DIFFDRIVE_Wheel_Motion_Control(newest_target);

	  if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

		  // Stop other thread while sending values
		  osThreadSuspend(DistanceSensorHandle);

		  DIFFDRIVE_HISTORY_TypeDef* tmphist= hdiffdrive.history;

		  DIFFDRIVE_TIMING_Stop();
		  DIFFDRIVE_MOTOR_Stop();

		  osDelay(1000);

		  for(int k= 0; k < tmphist[left].time_index; k++){
			  DATASTREAM_RTOS_Send_Value(
					  stream_on_matlab, 6,
					  tmphist[left].input[k],        tmphist[right].input[k],
					  tmphist[left].wheel_actual[k], tmphist[right].wheel_actual[k],
					  tmphist[left].wheel_target[k], tmphist[right].wheel_target[k]
			  );
			  // Toggling led to show data transfer operation
			  if( !(k%50) )
				  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		  }

		  // Resume other thread
		  osThreadResume(DistanceSensorHandle);

		  // Init feed-forward buffer (with NULL value in second argument the buffers are initialized with zeros)
		  FEEDFORWARD_Buffer_Init(&hctrl[left ].fdfrwrd, NULL);
		  FEEDFORWARD_Buffer_Init(&hctrl[right].fdfrwrd, NULL);

		  newest_target[left ]= 0;
		  newest_target[right]= 0;

		  DIFFDRIVE_SENSOR_Reset();
		  DIFFDRIVE_HISTORY_Start();
		  DIFFDRIVE_TIMING_Start();
	  }


	  if( i<10 )
		  temp[i]= (long int)__HAL_TIM_GET_COUNTER(&htim5);
	  i++;
/*
	  // Computing newest Target
	  DIFFDRIVE_Compute_Wheel_Target(newest_target, x[i], y[i]);

	  // Control wheels
	  DIFFDRIVE_Wheel_Motion_Control(newest_target);

	  if( i<N_SAMPLE_REF )
		  i++;
	  else{
		  DIFFDRIVE_MOTOR_Stop();

		  i=0;
		  uint32_t cnt= __HAL_TIM_GET_COUNTER(&htim5);

		  osDelay(2000);
	  }
*/
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartDistanceSensor */
/**
* @brief Function implementing the DistanceSensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDistanceSensor */
void StartDistanceSensor(void *argument)
{
  /* USER CODE BEGIN StartDistanceSensor */

	float distance;
	char uart_data_buff[8];

	int i= 0;
	bool flag_tmp= false;

  /* Infinite loop */
  for(;;)
  {

	  distance= HCSR04_read();

	  // Moving Average Filter
	  MAF_Update(&hfilter, distance);
	  // Extracting Moving Average Value
	  distance= __MAF_GET_VAL(hfilter);

	  if( distance < 1)
		  distance = 1;
	  else
	  if( distance > 180 )
		  distance= 180;

	  delta_a= 4*distance/100000; //0.001;
	  delta_th= 20*1/distance*0.03;

	  if( distance < 10 )
		  delta_a= 0;

	  if( delta_th > 0.025 )
		  delta_th= 0.025;

	  if( delta_th < 0.01 )
		  delta_th= 0;

	  if ( delta_a > 0.002)
		  delta_a= 0.002;

	  if( flag_tmp )
		  delta_th= -delta_th;

	  if( i%100 == 0)
		  flag_tmp= !flag_tmp;

	  i++;

/*
	  ftoa(distance, uart_data_buff, 4); // Float to Array!

	  // Polling TX - a transmission every second
	  HAL_UART_Transmit(&huart2, (uint8_t *)uart_data_buff, sizeof(uart_data_buff), HAL_MAX_DELAY);
	  HAL_UART_Transmit(&huart2, (uint8_t *)((char[1]){'\n'}), sizeof(char),HAL_MAX_DELAY);
*/

	  // Wait at least 60ms for ultrasonic sensor
	  osDelay(60);

	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

  }
  /* USER CODE END StartDistanceSensor */
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
