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
  *                        https://opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

//#include "my_lib/retarget.h"
#include "my_lib/differential_drive.h"
#include "my_lib/datastream.h"
#include "my_lib/digital_filters.h"
#include "my_lib/ultrasonic_sensor.h"
#include "my_lib/servo.h"

#include <my_data/control_parameters.h>
#include "my_data/diffdrive_parameters.h"

#include "stdbool.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//#define TEST_ULTRASONIC

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim12;

UART_HandleTypeDef huart3;

/* Definitions for DiffDriveTask */
osThreadId_t DiffDriveTaskHandle;
const osThreadAttr_t DiffDriveTask_attributes = {
  .name = "DiffDriveTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for TrajGenTask */
osThreadId_t TrajGenTaskHandle;
const osThreadAttr_t TrajGenTask_attributes = {
  .name = "TrajGenTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for InfraredTask */
osThreadId_t InfraredTaskHandle;
const osThreadAttr_t InfraredTask_attributes = {
  .name = "InfraredTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for TxTask */
osThreadId_t TxTaskHandle;
const osThreadAttr_t TxTask_attributes = {
  .name = "TxTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* USER CODE BEGIN PV */

// Handles of my libraries

DIFFDRIVE_HandleTypeDef  hdiffdrive;
MAF_HandleTypeDef        hmaf_trajGen_v;
MAF_HandleTypeDef        hmaf_trajGen_om;
MAF_HandleTypeDef        hmaf_diffDrive_v;
MAF_HandleTypeDef        hmaf_diffDrive_om;
MAF_HandleTypeDef		 hfilter_infrared;

ULTRASONIC_HandleTypeDef hultra;
SERVO_HandleTypeDef      hservo;
DATASTREAM_HandleTypeDef hstream;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM12_Init(void);
static void MX_TIM8_Init(void);
static void MX_USART3_UART_Init(void);
void MoveDiffDrive(void *argument);
void TrajectoryGenerator(void *argument);
void InfraredSensor(void *argument);
void TxStream(void *argument);

/* USER CODE BEGIN PFP */

static void MAIN_DIFFDRIVE_Init(void);
static void MAIN_ULTRASONIC_Init(void);
static void MAIN_MAF_Init(void);
static void MAIN_SERVO_Init(void);
static void MAIN_DATASTREAM_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Globals */

// ---- Global variables, shared between threads ------ //

volatile float linear=0, angular=0;
volatile bool near_obstacle= true;
volatile bool diffDrive_enabled= true;
volatile bool diffDrive_stopped= true;

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
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM12_Init();
  MX_TIM8_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  // Initialize my libraries
  MAIN_DIFFDRIVE_Init();
  MAIN_DATASTREAM_Init();
  MAIN_ULTRASONIC_Init();
  MAIN_MAF_Init();
  MAIN_SERVO_Init();


  // Wait until blue button is pressed
  while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) )
  		;

  // Wait a bit
  HAL_Delay(1000);

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
  DiffDriveTaskHandle = osThreadNew(MoveDiffDrive, NULL, &DiffDriveTask_attributes);

  /* creation of TrajGenTask */
  TrajGenTaskHandle = osThreadNew(TrajectoryGenerator, NULL, &TrajGenTask_attributes);

  /* creation of InfraredTask */
  InfraredTaskHandle = osThreadNew(InfraredSensor, NULL, &InfraredTask_attributes);

  /* creation of TxTask */
  TxTaskHandle = osThreadNew(TxStream, NULL, &TxTask_attributes);

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
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
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
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
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
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
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

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
  TIM_IC_InitTypeDef sConfigIC = {0};

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
  if (HAL_TIM_IC_Init(&htim12) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim12, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM12_Init 2 */

  /* USER CODE END TIM12_Init 2 */

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
  HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|ULTRASONIC_TRIGGER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : WIFI_ENABLE_Pin */
  GPIO_InitStruct.Pin = WIFI_ENABLE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(WIFI_ENABLE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin ULTRASONIC_TRIGGER_Pin */
  GPIO_InitStruct.Pin = LD2_Pin|ULTRASONIC_TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : INFRARED_SENSOR_Pin */
  GPIO_InitStruct.Pin = INFRARED_SENSOR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(INFRARED_SENSOR_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

static void MAIN_DATASTREAM_Init(void){

	DATASTREAM_InitTypeDef DATASTREAM_InitStruct;

	DATASTREAM_InitStruct.DMA_Rx_Enable= false;
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

static void MAIN_ULTRASONIC_Init(void){

	hultra.Init.htim=           		&htim12;
	hultra.Init.history_lenght= 		100;
	hultra.Init.io_trig_GPIOx=   	    ULTRASONIC_TRIGGER_GPIO_Port;
	hultra.Init.io_trig_Pin=   			ULTRASONIC_TRIGGER_Pin;
	hultra.Init.io_echo_GPIOx=  		ULTRASONIC_ECHO_GPIO_Port;
	hultra.Init.io_echo_Pin=			ULTRASONIC_ECHO_Pin;
	hultra.Init.io_echo_IC_tim_channel= TIM_CHANNEL_1;

	ULTRASONIC_Init(&hultra);

}


// Moving Average Filter initialization --------------------------------------------- //
static void MAIN_MAF_Init(void){

	int filtr_v_size_1=  15; // 15;
	int filtr_om_size_1= 7; // 7;

	int filtr_v_size_2=  12;//50;
	int filtr_om_size_2= 12;//50;

	int filtr_infrared_size= 100;

	// Null pointer to use a MAF without weights
	void* weights_buff= NULL;

	// Creating filters
	hmaf_trajGen_v		 = MAF_Create(filtr_v_size_1, 	   weights_buff);
	hmaf_trajGen_om		 = MAF_Create(filtr_om_size_1, 	   weights_buff);
	hmaf_diffDrive_v     = MAF_Create(filtr_v_size_2,	   weights_buff);
	hmaf_diffDrive_om    = MAF_Create(filtr_om_size_2,     weights_buff);
	hfilter_infrared     = MAF_Create(filtr_infrared_size, weights_buff);

}


static void MAIN_SERVO_Init(void){

	hservo.Init.htim=        &htim8;
	hservo.Init.pwm_channel= TIM_CHANNEL_1;

	SERVO_Init(&hservo);

}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_MoveDiffDrive */
/**
  * @brief  Function implementing the DiffDriveTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MoveDiffDrive */
void MoveDiffDrive(void *argument)
{
  /* USER CODE BEGIN 5 */

	// Local flag
	bool first_exception= true;

	// PID values
	float tmp_P= hdiffdrive.control[left].pid.P;
	float tmp_I= hdiffdrive.control[left].pid.I;

	// Speed variables
	float v, om;

	// Setting Global flags to indicate that Diff Drive is going to move
	diffDrive_stopped= false;

	// Start control
	DIFFDRIVE_TrackingStart();

	/* Infinite loop */
	for (;;) {

		// If movement is enabled
		if (diffDrive_enabled) {

			// Near obstacle not detected
			if (!near_obstacle) {

				// DO SOMETHING AT EXCEPTION END
				if (!first_exception) {

					// Restart motion
					DIFFDRIVE_TrackingSuspend();
					DIFFDRIVE_TrackingResume();

					MAF_Reset(&hmaf_diffDrive_v,  0);
					MAF_Reset(&hmaf_diffDrive_om, 0);

					hdiffdrive.control[left ].pid.P = tmp_P;
					hdiffdrive.control[right].pid.P = tmp_P;
					hdiffdrive.control[left ].pid.I = tmp_I;
					hdiffdrive.control[right].pid.I = tmp_I;
				}

				// Then set first = true
				first_exception = true;

				MAF_Update(&hmaf_diffDrive_v, linear);
				v = __MAF_GET_VAL(&hmaf_diffDrive_v);

				MAF_Update(&hmaf_diffDrive_om, angular);
				om = __MAF_GET_VAL(&hmaf_diffDrive_om);

			}
			// Near obstacle detected
			else {
				// DO SOMETHING AT EXCEPTION START
				if (first_exception) {

					hdiffdrive.control[left ].pid.P = tmp_P * 0.1;
					hdiffdrive.control[right].pid.P = tmp_P * 0.1;
					hdiffdrive.control[left ].pid.I = tmp_I * 0.1;
					hdiffdrive.control[right].pid.I = tmp_I * 0.1;
				}

				// Then set first = false
				first_exception = false;

				// Filtering values generated from TrajectoryGenerator task
				MAF_Update(&hmaf_diffDrive_v, linear);
				MAF_Update(&hmaf_diffDrive_om, angular);

				// Extract filtered values
				v =  __MAF_GET_VAL(&hmaf_diffDrive_v);
				om = __MAF_GET_VAL(&hmaf_diffDrive_om);
			}

			// If diff drive has been previously stopped
			if( diffDrive_stopped ){
				// Restart control
				DIFFDRIVE_TrackingResume();
				// Set flag to indicate that is going to move
				diffDrive_stopped= false;
			}

			// Make normal step of trajectory tracking
			DIFFDRIVE_TrackingStep(v, om);

		}
		// if movement is disabled and diffDrive is moving
		else if( !diffDrive_stopped ){
			// Suspend control (stop diff drive)
			DIFFDRIVE_TrackingSuspend();

			// Reset filters
			MAF_Reset(&hmaf_diffDrive_v,  0);
			MAF_Reset(&hmaf_diffDrive_om, 0);

			// Set global flag to indicate that diff drive is now stopped
			diffDrive_stopped= true;
		}
	}
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_TrajectoryGenerator */
/**
* @brief Function implementing the TrajGenTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TrajectoryGenerator */
void TrajectoryGenerator(void *argument)
{
  /* USER CODE BEGIN TrajectoryGenerator */

	// ------------ Servo variables ---------- //

	// Servo degree "equilibrium"
	float deg_eq= 82;

	// Array of angular position
	float deg_array[30];
	int N_deg= sizeof(deg_array)/sizeof(float);

	// Generate values for servo positions
	float deg_start= -8;
	for(int i=0; i<N_deg; i++)
		if( i <= N_deg/2 )
			deg_array[i]= deg_start+i*12;
		else
			deg_array[i]= deg_start+(N_deg-i)*12;

	// Counter variables
	int cnt_1=0, cnt_2=0;


	// ----------- Distance sensors variables ---------- //

	// Status variable
	ULTRASONIC_StatusTypeDef status;

	float distance;
	float distance_threshold= 15;

	float max_distance= ULTRASONIC_MAX_DISTANCE*0.5;
	float min_distance= ULTRASONIC_MIN_DISTANCE;

	// Infrared sensor variable
	float IR_val;


	// ------- Trajectory variables --------------------- //

	float v;
	float v_max; //= 0.003;

	float om;
	float om_max; //= 0.025;

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


	/* Infinite loop */
	for(;;){
		for(int deg_idx=0; deg_idx < N_deg; deg_idx++){

			// Setting up sensor direction using a servo
			SERVO_SetDegree(&hservo, deg_array[deg_idx]);

			// Wait a bit for servo settlement and between ultrasonic acquisition
			osDelay(120);

			/* Capture measured distance */

			status= ULTRASONIC_CaptureMeasure(&hultra);

			// If there is an error set max distance (an error can occur when a sensor timeout
			// take place, because the ultrasonic signal is lost)
			if( status == ULTRASONIC_OK )
				distance= __ULTRASONIC_GET_DISTANCE(&hultra);
			else
				distance= max_distance;

			// Sensor fusion with IR Sensor
			IR_val = __MAF_GET_VAL(&hfilter_infrared);
			distance *= IR_val;

			// Set minimum distance
			if (distance < min_distance)
				distance = min_distance;

			/* Using measured distance and servo angular position to generate a trajectory */

			// Robot linear speed (direct proportional to distance) and negative below a distance threshold
			v = (distance - distance_threshold) * 4e-3;
			if (distance < distance_threshold)
				v *= 1.5;

			// Robot angular speed (inversely proportional to distance) with sign that
			// depends on servo current position
			if (deg_array[deg_idx] > deg_eq)
				om = -43.2/distance;
			else
				om =  43.2/distance;

			// Enroll angular variations
			if(fabsf(om) > om_max)
				// x/|x| == sign(x)
				om = om_max*om/fabsf(om);
			// 0 below between a fixed range
			if (fabsf(om) < 0.01)
				om = 0;
/*
			//
			// Compute v_max, to find a feasible trajectory, i.e. that respect wheel max speed rate.
			// Here I fix omega (angular speed), computed above, and I find the current max (feasible) linear value
			// Look at chapter 2, expression 1.11, on the links below (on STM32CubeIDE press CTRL and then click). You
			// have just to find the max or min v value (min is -max) with the constraints of omega fixed and
			// wheel_speed between the fixed range:
			//
			// https://github.com/FloydPeppe/Programmazione_Sistemi_Embedded_Automazione/blob/master/Relazione/Relazione_Progetto.pdf
			//
			float l= hdiffdrive.mech_prmtr.l;
			float r= hdiffdrive.mech_prmtr.r;
			if( om >= 0 )
				v_max= r*wheel_speed_max - om*l;
			else
				v_max= r*wheel_speed_max + om*l;
*/
			// Enroll linear variations
			if(fabsf(v) > v_max)
				// x/|x| == sign(x)
				v = v_max*v/fabsf(v);

			// If distance is above a fixed threshold use the generated trajectory variation
			if (distance > distance_threshold) {

				cnt_1 = 0;

				// No near obstacle detected
				near_obstacle = false;

				// Smoothing measure using a Moving Average Filter

				MAF_Update(&hmaf_trajGen_v, v);
				linear = __MAF_GET_VAL(&hmaf_trajGen_v);

				MAF_Update(&hmaf_trajGen_om, om);
				angular = __MAF_GET_VAL(&hmaf_trajGen_om);

			}
			// Otherwise, if distance is below a fixed threshold freeze servo in that position and move 'way from obstacle
			else {
				// This do freeze servo
				deg_idx--;
				if (++cnt_1 > 1 || IR_val <= 0.5) {

					// Trajectory following relaxed (it doesn't need to follow the trajectory perfectly, it just
					// need to move away from a really near obstacle)
					near_obstacle = true;

					/* Avoiding the use of filtered values to have a fast response near an obstacle. Anyway,
					      keep update filters just to not have discontinuities after this exception ends.     */

					// Force a minimum Delta_a value, otherwise, near threshold, sometimes robot stand still
					linear = (v - v_max * 0.1) * 1;

					if (linear > 0)
						linear = -linear;

					// Set current Delta_theta angle
					angular = om * 0.4;

					// Filters updates
					MAF_Update(&hmaf_trajGen_v, v);
					//linear= __MAF_GET_VAL(&hmaf_trajGen_v);
					MAF_Update(&hmaf_trajGen_om, angular);
					// angular= __MAF_GET_VAL(&hmaf_trajGen_om);

					// Freeze servo just for a fixed time
					if (cnt_1++ > 30) {
						cnt_1 = 0;
						deg_idx++;
						//v= -v;
					}
				}
			}

			if ( distance > distance_threshold && distance < distance_threshold * 4 ) {
				deg_idx--;
				if (cnt_2++ > 10) {
					cnt_2 = 0;
					deg_idx++;
				}
			}

			// Toggle led to see if the task is still running
			// HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

#ifdef TEST_ULTRASONIC

		  // Transmit measured distance //
		  DATASTREAM_Print(&hstream, float_csv, 1, distance);

#endif /* TEST_ULTRASONIC */

		}
	}
  /* USER CODE END TrajectoryGenerator */
}

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
	DIFFDRIVE_STATE_TypeDef state_measure, state_target;

	/* Infinite loop */
	for(;;){

		// If blue button is pressed
		if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

			// When I'm here, I use HAL_Delay(ms) and not osDelay(ms), because during Tx stream
			// I suspend other theads, so I don't need os timing. Moreover, osDelay sometimes hangs forever
			// without no reason, I guess 'cause other threads are suspended and scheduler try to wait for them.

			// Stop Differential Drive motion
			diffDrive_enabled= false;

			// Wait until diff drive is still
			while( !diffDrive_stopped )
				;

			// Suspend other threads while sending values
			osThreadSuspend(DiffDriveTaskHandle);
			osThreadSuspend(TrajGenTaskHandle);
			osThreadSuspend(InfraredTaskHandle);

			// Turn on LED
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

			// Reset ESP-8266
			HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);

			// Debounce button
			osDelay(200);

			// Send values forever
			for(;;){

				// If blue button is pressed again
				if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

					// Enable ESP-8266
					HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);

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
					HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);
				}
			}
		}
	}
  /* USER CODE END TxStream */
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
