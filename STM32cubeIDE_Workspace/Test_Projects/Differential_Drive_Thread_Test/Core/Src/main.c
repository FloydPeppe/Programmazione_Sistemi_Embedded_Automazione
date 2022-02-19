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
#include <my_lib/datastream_utils.h>
#include "my_lib/digital_filters.h"
#include "my_lib/ultrasonic_sensor.h"
#include "my_lib/servo.h"

#include <my_data/identification.h>
#include <my_data/idinput.h>
#include <my_data/reference.h>

#include "stdbool.h"

#include <stdio.h>
#include <stdlib.h>

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
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim12;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_tx;

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
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for InfraredSensor */
osThreadId_t InfraredSensorHandle;
const osThreadAttr_t InfraredSensor_attributes = {
  .name = "InfraredSensor",
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
/* USER CODE BEGIN PV */

DIFFDRIVE_HandleTypeDef  hdiffdrive;
MAF_HandleTypeDef        hfilter_a;
MAF_HandleTypeDef        hfilter_th;
MAF_HandleTypeDef        hfilter_a_2;
MAF_HandleTypeDef        hfilter_th_2;
MAF_HandleTypeDef		 hfilter_infrared;

ULTRASONIC_HandleTypeDef hultra;
SERVO_HandleTypeDef      hservo;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM12_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM8_Init(void);
static void MX_USART3_UART_Init(void);
void StartControlDiffDrive(void *argument);
void StartDistanceSensor(void *argument);
void StartInfraredSensor(void *argument);
void SerialCommnuication(void *argument);

/* USER CODE BEGIN PFP */

static void MAIN_DIFFDRIVE_Init(void);
static void MAIN_ULTRASONIC_Init(void);
static void MAIN_MAF_Init(void);
static void MAIN_SERVO_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Globals */

//float* delta_a_array=  DELTA_A_ref;
//float* delta_th_array= DELTA_THETA_ref;

/* Global variables, shared with threads */

volatile float delta_a=0, delta_th=0;
volatile bool tracking_error_flag= true;

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
  MX_TIM12_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  // Initialize
  DATASTREAM_Init(&huart3);
  MAIN_DIFFDRIVE_Init();
  MAIN_ULTRASONIC_Init();
  MAIN_MAF_Init();
  MAIN_SERVO_Init();



  // Wait until blue button is pressed
  while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) )
  		;

  // Enable WIFI to send/receive values
  HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);

  // Wait for ESP boot
  HAL_Delay(3000);

  // Rapidly flash led to show MCU ready to send values
  for(int i=0; i<20; i++){
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  HAL_Delay(50);
  }

  // Wait for 10 received chars
  char ch[10];
  HAL_UART_Receive(&huart3, (uint8_t*) ch, sizeof(ch), HAL_MAX_DELAY);

  // LED on
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

  // Wait a bit more
  HAL_Delay(3000);

  // LED on
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  // Wait until the blue button is pressed again
   while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) );
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
  /* creation of ControlDiffDriv */
  ControlDiffDrivHandle = osThreadNew(StartControlDiffDrive, NULL, &ControlDiffDriv_attributes);

  /* creation of DistanceSensor */
  DistanceSensorHandle = osThreadNew(StartDistanceSensor, NULL, &DistanceSensor_attributes);

  /* creation of InfraredSensor */
  InfraredSensorHandle = osThreadNew(StartInfraredSensor, NULL, &InfraredSensor_attributes);

  /* creation of Communication */
  CommunicationHandle = osThreadNew(SerialCommnuication, NULL, &Communication_attributes);

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
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 840;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

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
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
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

static void MAIN_DIFFDRIVE_Init(void){

	float Ts= 0.01;
	int max_sample= 1000;

	/* Timing parameters */
	hdiffdrive.Init.timing_htim= &htim3;
	hdiffdrive.Init.timing_time_step= Ts;

	/* Sensors parameters */
	hdiffdrive.Init.encoder_htim[left ]= &htim2;
	hdiffdrive.Init.encoder_htim[right]= &htim5;
	hdiffdrive.Init.encoder_channel[left ]= TIM_CHANNEL_1;
	hdiffdrive.Init.encoder_channel[right]= TIM_CHANNEL_1;

	hdiffdrive.Init.encoder_resolution[left ]= 1900; //1920; // Number of step each wheel revolution
	hdiffdrive.Init.encoder_resolution[right]= 1900; //1920;

	/* Motors parameters */
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

	/* Feed-forward law parameters */
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

	/* PID law parameters */
	hdiffdrive.Init.control_pid_P[left ]= 5*0.00133180775219883*1e1*5e2;   // 0.50835*0.08; // 0.40835;
	hdiffdrive.Init.control_pid_P[right]= 5*0.00133180775219883*1e1*5e2;   // 0.50835*0.08; // 0.40835
	hdiffdrive.Init.control_pid_I[left ]= 15.64986485011565*1e-3*5e2; // 0.57005*0.004; // 0.27005;
	hdiffdrive.Init.control_pid_I[right]= 15.64986485011565*1e-3*5e2; // 0.57005*0.004; // 0.27005;
	hdiffdrive.Init.control_pid_D[left ]= 0.5*0;
	hdiffdrive.Init.control_pid_D[right]= 0.5*0;

	/* History parameters */
	hdiffdrive.Init.history_max_sample= max_sample;

	/* Mechanical parameters (from real model) */
	hdiffdrive.Init.mech_prmtr_l=                 0.155/2;	// Distance between wheels
	hdiffdrive.Init.mech_prmtr_r=                 0.069/2; // Wheels radius
	hdiffdrive.Init.mech_prmtr_max_wheel_speed=   16;//20;  // Radiant each seconds (rad/s), DC motor and gears specifics

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

	int filtr_a_size=  15; // 15;
	int filtr_th_size= 7; // 7;

	int filtr_a_size_2=  12;//50;
	int filtr_th_size_2= 12;//50;

	int filtr_infrared_size= 100;

	// Null pointer to use a MAF without weights
	void* weights_buff= NULL;

	// Creating filters
	hfilter_a		 = MAF_Create(filtr_a_size, 	   weights_buff);
	hfilter_th		 = MAF_Create(filtr_th_size, 	   weights_buff);
	hfilter_a_2		 = MAF_Create(filtr_a_size_2,	   weights_buff);
	hfilter_th_2     = MAF_Create(filtr_th_size_2,     weights_buff);
	hfilter_infrared = MAF_Create(filtr_infrared_size, weights_buff);

}


static void MAIN_SERVO_Init(void){

	hservo.Init.htim=        &htim8;
	hservo.Init.pwm_channel= TIM_CHANNEL_1;

	SERVO_Init(&hservo);

}


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartControlDiffDrive */

float delta_a_hist[1000];
float delta_th_hist[1000];
int k_=0;
/**
  * @brief  Function implementing the ControlDiffDriv thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartControlDiffDrive */
void StartControlDiffDrive(void *argument)
{
  /* USER CODE BEGIN 5 */

#ifdef TEST_TIMING
	int i=0;
	volatile uint16_t tmp_var[20];

	HAL_TIM_Base_Start(&htim7);

	__HAL_TIM_SET_COUNTER(&htim7,0);

#endif

	bool first= true;
	float tmp_P= hdiffdrive.control[left].pid.P;
	float tmp_I= hdiffdrive.control[left].pid.I;

	float delta_a_loc, delta_th_loc;

	// Tracking error threshold
	float e_thr= 35*3e-2;

	// Start control
	DIFFDRIVE_TrackingStart();

  /* Infinite loop */
  for(;;)
  {

#ifdef TEST_TIMING
	  __HAL_TIM_SET_COUNTER(&htim7,0);
#endif

	  // Tracking error flag enabled
	  if( tracking_error_flag ){

		  // DO SOMETHING AT EXCEPTION END
		  if( !first ){
			  DIFFDRIVE_MOTOR_Stop();
			  osThreadSuspend(DistanceSensorHandle);
			  osDelay(400);
			  osThreadResume(DistanceSensorHandle);

			  DIFFDRIVE_CONTROL_Reset();

			  //MAF_Reset(&hfilter_a, 0);
			  //MAF_Reset(&hfilter_th, 0);
			  MAF_Reset(&hfilter_a_2, 0);
			  MAF_Reset(&hfilter_th_2, 0);
			  //MAF_Reset(&hfilter_infrared, 0);

			  hdiffdrive.control[left ].pid.P= tmp_P;
			  hdiffdrive.control[right].pid.P= tmp_P;
			  hdiffdrive.control[left ].pid.I= tmp_I;
			  hdiffdrive.control[right].pid.I= tmp_I;
		  }

		  // Then set first = true
		  first= true;

		  MAF_Update(&hfilter_a_2, delta_a);
		  delta_a_loc= __MAF_GET_VAL(&hfilter_a_2);

		  MAF_Update(&hfilter_th_2, delta_th);
		  delta_th_loc= __MAF_GET_VAL(&hfilter_th_2);

		  // Manage exception when the robot is stuck somehow and cannot keep following the target
		  if( hdiffdrive.control[left].pid.e_prev  >  e_thr || hdiffdrive.control[right].pid.e_prev >  e_thr ||
			  hdiffdrive.control[left].pid.e_prev  < -e_thr || hdiffdrive.control[right].pid.e_prev < -e_thr    ){


			  // Stop other thread
			//  osThreadSuspend(DistanceSensorHandle);

			  /* Move in back direction */
			  float delta_a_tmp=  -delta_a;
			  float delta_th_tmp= -delta_th;

			  hdiffdrive.control[left ].pid.P= tmp_P*0.1;
			  hdiffdrive.control[right].pid.P= tmp_P*0.1;
			  hdiffdrive.control[left ].pid.I= tmp_I*0.1;
			  hdiffdrive.control[right].pid.I= tmp_I*0.1;

			  for(int j=0; j<100; j++){

				  delta_th_tmp= delta_th;

				  // Keep use filters to smooth trajectory
				  MAF_Update(&hfilter_a_2,  delta_a_tmp);
				  MAF_Update(&hfilter_th_2, delta_th_tmp);

				  delta_a_loc=  __MAF_GET_VAL(&hfilter_a_2);
				  delta_th_loc= __MAF_GET_VAL(&hfilter_th_2);

				  if( k_ < 1000 ){
					  delta_a_hist[k_]=  delta_a_tmp;
				  	  delta_th_hist[k_]= delta_th_tmp;
				  	  k_++;
				  }

				  // Make Tracking step
				  DIFFDRIVE_TrackingStep(delta_a_loc, delta_th_loc);

				  if( !(j%10) )
					  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

			  }

			  osThreadSuspend(DistanceSensorHandle);
			  // Then stop motors and reset
			  DIFFDRIVE_MOTOR_Stop();

			  hdiffdrive.control[left ].pid.P= tmp_P;
			  hdiffdrive.control[right].pid.P= tmp_P;
			  hdiffdrive.control[left ].pid.I= tmp_I;
			  hdiffdrive.control[right].pid.I= tmp_I;

			  osDelay(460);

			  MAF_Reset(&hfilter_a, 0);
			  MAF_Reset(&hfilter_th, 0);
			  MAF_Reset(&hfilter_a_2, 0);
			  MAF_Reset(&hfilter_th_2, 0);

			  delta_a_loc= 0;
			  delta_th_loc= 0;

			  // Reset Control
			  DIFFDRIVE_CONTROL_Reset();

			  // Restart other thread
			  osThreadResume(DistanceSensorHandle);
		  }
	  }
	  // Tracking error flag disabled, when robot is really near an obstacle
	  else{
		  // DO SOMETHING AT EXCEPTION START
		  if(first){

			  hdiffdrive.control[left ].pid.P= tmp_P*0.1;
			  hdiffdrive.control[right].pid.P= tmp_P*0.1;
			  hdiffdrive.control[left ].pid.I= tmp_I*0.1;
			  hdiffdrive.control[right].pid.I= tmp_I*0.1;
		  }

		  // Then set first = false
		  first= false;

		  MAF_Update(&hfilter_a_2,  delta_a);
		  MAF_Update(&hfilter_th_2, delta_th);

		  delta_a_loc=  __MAF_GET_VAL(&hfilter_a_2);
		  delta_th_loc= __MAF_GET_VAL(&hfilter_th_2);
	  }


	  delta_a_hist [k_%1000]= delta_a;
	  delta_th_hist[k_%1000]= delta_th;
	  k_++;

	  // Make normal step of trajectory tracking
	  DIFFDRIVE_TrackingStep(delta_a_loc, delta_th_loc);

#ifdef TEST_TIMING
	  if( i<20 )
		  tmp_var[i++]= __HAL_TIM_GET_COUNTER(&htim7);
#endif

	  // If blue button is pressed
	  if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

		  // Stop other thread while sending values
		  osThreadSuspend(DistanceSensorHandle);

		  DIFFDRIVE_HISTORY_TypeDef tmphist= hdiffdrive.history;

		  // Stop control
		  DIFFDRIVE_TrackingStop();

		  // Repeat forever
		  for(;;){
			  // Enable WIFI to send/receive values
			  HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);

			  // Wait for ESP boot
			  HAL_Delay(3000);

			  // Rapidly flash led to show MCU ready to send values
			  for(int i=0; i<20; i++){
				  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
				  HAL_Delay(50);
			  }

			  // Wait for 10 received chars
			  char ch[10];
			  HAL_UART_Receive(&huart3, (uint8_t*) ch, sizeof(ch), HAL_MAX_DELAY);

			  // Wait until button is pressed again
			  //while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) )
			  //		;

			  // Wait a bit
			  osDelay(2000);

			  // Number of saved samples
			  uint32_t N_sample;
			  if( tmphist.sample_cnt < tmphist.max_sample )
				  N_sample=  tmphist.sample_cnt;
			  else
				  N_sample=  tmphist.max_sample;

			  // First values send is the actual number of samples
			  DATASTREAM_RTOS_SendValue(stream_on_matlab, 1, (double)N_sample);
			  // Bit of transfer delay
			  osDelay(8);

			  // Init History Get function
			  DIFFDRIVE_HISTORY_Get_Init();

			  for(int k= 0; k < N_sample; k++){
				  // Temporary variables
				  DIFFDRIVE_STATE_TypeDef state_measure, state_target;

				  // Get one state sample from history
				  DIFFDRIVE_HISTORY_Get_Sample(&state_measure, &state_target);

				  // Send values to serial connected to ESP WiFi module
				  DATASTREAM_RTOS_SendValue(
						  stream_on_matlab, 16,
						  state_measure.input[left], state_measure.input[right],
						  state_target .input[left], state_target .input[right],
						  state_measure.wheel[left], state_measure.wheel[right],
						  state_target .wheel[left], state_target .wheel[right],
						  state_measure.pose.x_a,    state_measure.pose.y_a,
						  state_measure.pose.theta,
						  state_target .pose.x_a,    state_target.pose.y_a,
						  state_target .pose.theta,
						  delta_a_hist[k],		     delta_th_hist[k]
				  );
				  // Bit of transfer delay
				  osDelay(8);
				  // Toggling led to show data transfer operation
				  if( !(k%50) )
					  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			  }

			  // Turn off LED
			  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			  // Disable WiFi
			  HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);

			  // Wait for blue button pressed again
			  while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) )
				  ;
		  }
/*
		  // Resume other thread
		  osThreadResume(DistanceSensorHandle);

		  // Start control again
		  DIFFDRIVE_TrackingStart();
*/
	  }
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

	ULTRASONIC_StatusTypeDef status;

	float deg_eq= 82;
	float deg_array[30]= {-8,12,22,32,42,52,62,72,82,92,102,112,122,142,152,172,152,142,122,112,102,92,82,72,62,52,42,32,22,12};
	int n= sizeof(deg_array)/sizeof(float);

	float distance;
	float distance_threshold= 15;

	float max_distance= ULTRASONIC_MAX_DISTANCE*0.5;
	float min_distance= ULTRASONIC_MIN_DISTANCE;

	float delta_a_tmp;
	float delta_a_max; //= 0.003;

	float delta_th_tmp;
	float delta_th_max; //= 0.025;

	float IR_val;

	int k=0, k2=0;
	(void)k; // Suppress unused variable warning


	float delta_wheel_max[2];
	delta_wheel_max[left ]= hdiffdrive.mech_prmtr.max_wheel_speed*hdiffdrive.timing.time_step;
	delta_wheel_max[right]= hdiffdrive.mech_prmtr.max_wheel_speed*hdiffdrive.timing.time_step;

	// Delta_a_max computation
	DIFFDRIVE_DeltaWheel2DeltaTrajectory(&delta_a_max, NULL, delta_wheel_max);
	delta_a_max *= 0.6; //0.6

	// Delta_th_max computation
	delta_wheel_max[left]= -delta_wheel_max[left];
	DIFFDRIVE_DeltaWheel2DeltaTrajectory(NULL, &delta_th_max, delta_wheel_max);
	delta_th_max *= 0.25;

	//Floyd è cattivello, un super cattivello
	//Obli invece è molto carina <3


  /* Infinite loop */
  for(;;)
  {

	  for(int j=0; j<n; j++){

		  // Setting up sensor direction using a servo
		  SERVO_SetDegree(&hservo, deg_array[j]);

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
		  if( distance < min_distance)
			  distance = min_distance;


		  /* Using measured distance and servo angular position to generate a trajectory */

		  // Robot linear position variation (direct proportional to distance) and negative below a distance threshold
		  delta_a_tmp= (distance-distance_threshold)*4*1e-5;
		  if( distance < distance_threshold)
			  delta_a_tmp *= 1.5;

		  // Robot angular position (yaw) variation (inversely proportional to distance with sign that
		  // depends on servo degree position)
		  if( deg_array[j] > deg_eq )
			  delta_th_tmp= -0.48/distance*0.9;//*1.5;
		  else
			  delta_th_tmp=  0.48/distance*0.9;//*1.5;


		  // Enroll angular variations
		  if( delta_th_tmp > delta_th_max )
			  delta_th_tmp= delta_th_max;
		  else
		  if( delta_th_tmp < -delta_th_max )
			  delta_th_tmp= -delta_th_max;

		  if( delta_th_tmp < 0.01 && delta_th_tmp > -0.01 )
			  delta_th_tmp= 0;

		  // Enroll position variationstype filter text
		  if ( delta_a_tmp > delta_a_max)
			  delta_a_tmp= delta_a_max;

		  if ( delta_a_tmp < -delta_a_max)
			  delta_a_tmp= -delta_a_max;

		  // If distance is above a fixed threshold use the generated trajectory variation
		  if( distance > distance_threshold ){

			  k= 0;

			  // Tracking error control enabling
			  tracking_error_flag= true;

		  	  // Smoothing measure using a Moving Average Filter

		  	  MAF_Update(&hfilter_a, delta_a_tmp);
		  	  delta_a= __MAF_GET_VAL(&hfilter_a);

		  	  MAF_Update(&hfilter_th, delta_th_tmp);
		  	  delta_th= __MAF_GET_VAL(&hfilter_th);

	  	  }
		  // Otherwise, if distance is below a fixed threshold freeze servo in that position and move 'way from obstacle
	  	  else{
	  		  // This do freeze servo
	  		  j--;
			  if( ++k > 1 || IR_val <= 0.5 ){

				  // Following error control disabling (it doesn't need to follow the trajectory perfectly, it just
				  // need to move away from a really near obstacle)
				  tracking_error_flag= false;

				  /* Avoiding the use of filtered values to have a fast response near an obstacle. Anyway,
					 keep  update filters just to not have discontinuities after this exception ends.       */

				  // Force a minimum Delta_a value, otherwise, near threshold, sometimes robot stand still
				  delta_a=  (delta_a_tmp - delta_a_max*0.1)*1;

				  if( delta_a > 0)
				  	  delta_a= -delta_a;

				  // Set current Delta_theta angle
				  delta_th= delta_th_tmp;

				  // Filters updates
				  MAF_Update(&hfilter_a,  delta_a_tmp);
				  //delta_a= __MAF_GET_VAL(&hfilter_a);
				  MAF_Update(&hfilter_th, delta_th_tmp);
				  //delta_th= __MAF_GET_VAL(&hfilter_th);

				  // Freeze servo just for a fixed time
				  if( k++ > 30 ){
					  k= 0;
					  j++;
					  //delta_a_tmp= -delta_a_tmp;
				  }
			  }
	  	  }

		  if( distance > distance_threshold && distance < distance_threshold*4){
			  j--;
			  if( k2++ > 10 ){
				  k2= 0;
				  j++;
			  }
		  }

		  // Toggle led to see if the task is still running
		 // HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		  // Wait at least 60ms for each ultrasonic sensor acquisition
//		  osDelay(60);


#ifdef TEST_ULTRASONIC
		  // Transmit measured distance //

		  char uart_data_buff[8];

		  ftoa(distance, uart_data_buff, 4); // Float to Array!

		  // Polling TX - a transmission every second
		  HAL_UART_Transmit(&huart2, (uint8_t *)uart_data_buff, sizeof(uart_data_buff), HAL_MAX_DELAY);
		  HAL_UART_Transmit(&huart2, (uint8_t *)((char[1]){'\n'}), sizeof(char),HAL_MAX_DELAY);

#endif /* TEST_ULTRASONIC */


	  }
  }
  /* USER CODE END StartDistanceSensor */
}

/* USER CODE BEGIN Header_StartInfraredSensor */
/**
* @brief Function implementing the InfraredSensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInfraredSensor */
void StartInfraredSensor(void *argument)
{
  /* USER CODE BEGIN StartInfraredSensor */
  /* Infinite loop */

	int sens=0, k=0;
	(void) k;

//	osThreadSuspend(DistanceSensorHandle);
//	osThreadSuspend(ControlDiffDrivHandle);

	for(;;){

		sens= HAL_GPIO_ReadPin(INFRARED_SENSOR_GPIO_Port, INFRARED_SENSOR_Pin);

		MAF_Update(&hfilter_infrared, sens);
/*
		if( !(k%10000) ){
			printf("%f\t\n",3*__MAF_GET_VAL(&hfilter_infrared));
			k= 0;
		}
*/
		osDelay(1);
	}
  /* USER CODE END StartInfraredSensor */
}

/* USER CODE BEGIN Header_SerialCommnuication */
/**
* @brief Function implementing the Communication thread.
* @param argument: Not used
* @retval None
*/

/* USER CODE END Header_SerialCommnuication */
void SerialCommnuication(void *argument)
{
  /* USER CODE BEGIN SerialCommnuication */

  uint32_t curr_sample_cnt= 0, prev_sample_cnt= 0;

  /* Infinite loop */
  for(;;)
  {

	  if( (curr_sample_cnt= hdiffdrive.history.sample_cnt) != prev_sample_cnt ){

		  // Update variable
		  prev_sample_cnt= curr_sample_cnt;

		  // Temporary variables
		  DIFFDRIVE_STATE_TypeDef state_measure, state_target;

		  // Get one state sample from history
		  DIFFDRIVE_HISTORY_Get_Sample(&state_measure, &state_target);

		  // Send values to serial connected to ESP WiFi module
		  DATASTREAM_RTOS_SendValue(
				  stream_on_matlab, 14,
				  state_measure.input[left], state_measure.input[right],
				  state_target .input[left], state_target .input[right],
				  state_measure.wheel[left], state_measure.wheel[right],
				  state_target .wheel[left], state_target .wheel[right],
				  state_measure.pose.x_a,    state_measure.pose.y_a,
				  state_measure.pose.theta,
				  state_target .pose.x_a,    state_target.pose.y_a,
				  state_target .pose.theta
		  );

	  }

	  osDelay(10);

  }
  /* USER CODE END SerialCommnuication */
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