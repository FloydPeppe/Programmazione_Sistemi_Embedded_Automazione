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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "my_lib/differential_drive.h"
#include "my_lib/retarget.h"
#include "my_lib/datastream.h"

#include "my_data/idinput.h"
#include "my_data/reference.h"
#include "my_data/identification.h"

#include <stdio.h>
#include <stdlib.h>

#include "arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/*---------- DEFINES used to change code behavior ---------------------------*/

/* Only Sensor check (uncomment below, highest priority) */
//#define SENSOR_CHECK

/* Using a trajectory to be followed */
#define TRAJECTORY_TRACKING

/* Using an input voltage for motors identification */
#define IDENTIFICATION

// Priority to trajectory tracking behavior respect of identification
#ifdef TRAJECTORY_TRACKING
#undef IDENTIFICATION
#endif /* TRAJECTORY_TRACKING */

#ifdef IDENTIFICATION

// Just one of these defines uncommented
//#define TRIANGLE_INPUT
#define SMOOTH_INPUT

#endif /* IDENTIFICATION */

/* ---------------------------------------------------------------------------*/


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
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */

DIFFDRIVE_HandleTypeDef hdiffdrive;

// Debug variable
int check= 0;


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
/* USER CODE BEGIN PFP */

static void MAIN_DIFFDRIVE_Init(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  DATASTREAM_Init(&huart3);

  MAIN_DIFFDRIVE_Init();

#ifdef TRAJECTORY_TRACKING

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

  // LED off
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

#endif /* TRAJECTORY_TRACKING */

  // HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

#ifdef IDENTIFICATION

  // Allocating input vectors
  float **input= (float**)malloc(sizeof(float*)*2);
  for(int i=0; i<2; i++)
	  input[i]= (float*)malloc(sizeof(float)*N_SAMPLE_ID);

#ifdef TRIANGLE_INPUT

  for(int i=0; i<N_SAMPLE_ID; i++){
	  input[left ][i]= TRIANGLE_input_L[i]*MOTOR_MAX_VOLTAGE;
	  input[right][i]= TRIANGLE_input_R[i]*MOTOR_MAX_VOLTAGE;
  }

#endif /* TRIANGLE_INPUT */

#ifdef SMOOTH_INPUT

  for(int i=0; i<N_SAMPLE_ID; i++){
	  input[left ][i]= SMOOTH_input_L[i]*MOTOR_MAX_VOLTAGE;
	  input[right][i]= SMOOTH_input_R[i]*MOTOR_MAX_VOLTAGE;
  }

#endif /*SMOOTH_INPUT */

#endif /* IDENTIFICATION */

#ifdef SENSOR_CHECK
  int i=0;
#else
  // Enabling motion
  bool motion_flag= true;
#endif /* SENSOR_CHECK */

  // Wait until the blue button is pressed
  while( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) )
	  __NOP();

  HAL_Delay(1000);

  // Reset Sensors
  DIFFDRIVE_ENCODER_Reset();

  while (1)
  {

#ifdef SENSOR_CHECK

      DIFFDRIVE_ENCODER_CaptureMeasure();

	  if( !(i%100) )
		  DIFFDRIVE_MOTOR_SetPWMs((float[2]){ -MOTOR_MAX_VOLTAGE*0.5, -MOTOR_MAX_VOLTAGE*0.5});
	  if( !(i%150) )
		  DIFFDRIVE_MOTOR_SetPWMs((float[2]){ -MOTOR_MAX_VOLTAGE*0.7, -MOTOR_MAX_VOLTAGE*0.7});

	  printf("%f\t", __DIFFDRIVE_ENCODER_GET_CURRENT_VALUE(&hdiffdrive,left ));
	  printf("%f\n", __DIFFDRIVE_ENCODER_GET_CURRENT_VALUE(&hdiffdrive,right));

	  HAL_Delay(10);

	  i++;

	  DATASTREAM_SendValue(
			  stream_on_terminal, 4,
			  "L Counter %f\n", __DIFFDRIVE_ENCODER_GET_CURRENT_VALUE(&hdiffdrive,left ),
			  "R Counter %f\n", __DIFFDRIVE_ENCODER_GET_CURRENT_VALUE(&hdiffdrive,right)
	  );
	  HAL_Delay(500);

#else

	if( motion_flag ){

#ifdef TRAJECTORY_TRACKING


		// First values send is the actual number of samples
		DATASTREAM_SendValue(stream_on_matlab, 1, (double)N_SAMPLE_REF);
		// Bit of transfer delay
		HAL_Delay(8);

		DIFFDRIVE_TrackingStart();

		for(int i= 0; i<N_SAMPLE_REF; i++){
			// Make a control step
			DIFFDRIVE_TrackingStep(DELTA_A_ref[i], DELTA_THETA_ref[i]);

			// Temporary variables
			DIFFDRIVE_STATE_TypeDef state_measure, state_target;

			// Get one state sample from history
			DIFFDRIVE_HISTORY_Get_Sample(&state_measure, &state_target);

			// Send values to serial connected to ESP WiFi module
			DATASTREAM_SendValue(
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

		// Stop differential drive
		DIFFDRIVE_TrackingStop();

		// Disable WiFi
		HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);

#endif /* TRAJECTORY_TRACKING */

#ifdef IDENTIFICATION

		DIFFDRIVE_InputArray(N_SAMPLE_ID, input);

#endif /* IDENTIFICATION */

		// Stop motion
		motion_flag= false;

	}
	else{

		// Start sending values Task if blue button is pressed
		if( !HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) ){

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

			// LED off
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			// History struct
			DIFFDRIVE_HISTORY_TypeDef tmphist= hdiffdrive.history;

			// Number of saved samples
			uint32_t N_sample;
			if( tmphist.sample_cnt < tmphist.max_sample )
				N_sample=  tmphist.sample_cnt;
			else
				N_sample=  tmphist.max_sample;

			// First values send is the actual number of samples
			DATASTREAM_SendValue(stream_on_matlab, 1, (double)N_sample);
			// Bit of transfer delay
			HAL_Delay(8);

			// Init History Get function
			DIFFDRIVE_HISTORY_Get_Init();

			for(int k= 0; k < N_sample; k++){
				// Temporary variables
				DIFFDRIVE_STATE_TypeDef state_measure, state_target;

				// Get one state sample from history
				DIFFDRIVE_HISTORY_Get_Sample(&state_measure, &state_target);

				// Send values to serial connected to ESP WiFi module
				DATASTREAM_SendValue(
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
				// Bit of transfer delay
				HAL_Delay(8);
				// Toggling led to show data transfer operation
				if( !(k%50) )
					HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			}

			// Turn off LED
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

			// Disable WiFi
			HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);

		}
	}

#endif /* SENSOR_CHECK */

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
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 0, 0);
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
  HAL_GPIO_WritePin(GPIOA, SUPPLY_ENABLE_Pin|LD2_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : SUPPLY_ENABLE_Pin LD2_Pin */
  GPIO_InitStruct.Pin = SUPPLY_ENABLE_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

	hdiffdrive.Init.encoder_resolution[left ]= 1900; // Number of step each wheel revolution
	hdiffdrive.Init.encoder_resolution[right]= 1900;

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

	hdiffdrive.Init.mech_prmtr_l=               0.156/2;
	hdiffdrive.Init.mech_prmtr_r=               0.0695/2;
	hdiffdrive.Init.mech_prmtr_max_wheel_speed= 16;      // 16 rad/s, from DC motor specifics


	DIFFDRIVE_Init(&hdiffdrive);

}


/* USER CODE END 4 */

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
