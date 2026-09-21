/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/** @brief Extended CAN identifier used for wire status messages. */
#define WIRE_CAN_ID       0x1800EEF0UL

/** @brief Periodic transmission interval. */
#define TX_PERIOD_MS      1000U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan;

/* USER CODE BEGIN PV */

/** @brief Tick value recorded after the last periodic transmission. */
static uint32_t last_tx_tick = 0U;

/** @brief Number of falling-edge events recorded by the interrupt. */
static volatile uint32_t disconnect_events = 0U;

/** @brief Number of disconnection events successfully queued for CAN. */
static uint32_t disconnect_events_sent = 0U;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN_Init(void);
/* USER CODE BEGIN PFP */

static HAL_StatusTypeDef CAN_SendStatus(uint8_t status);

static void CAN_ConfigFilter(void);

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
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */

  CAN_ConfigFilter();

  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
      Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(
          &hcan,
          CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
      Error_Handler();
  }

  last_tx_tick = HAL_GetTick();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */

      /* Prioritize pending disconnection events. */
      if (disconnect_events_sent != disconnect_events)
      {
          if (CAN_SendStatus(1U) == HAL_OK)
          {
              disconnect_events_sent++;
          }
      }

      /* Send normal status every 1000 milliseconds. */
      uint32_t now = HAL_GetTick();

      if ((uint32_t)(now - last_tx_tick) >= TX_PERIOD_MS)
      {
          if (CAN_SendStatus(0U) == HAL_OK)
          {
              last_tx_tick = now;
          }
      }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 2;
  hcan.Init.Mode = CAN_MODE_LOOPBACK;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : STATUS_LED_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : WIRE_SENSE_Pin */
  GPIO_InitStruct.Pin = WIRE_SENSE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(WIRE_SENSE_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief Queues a one-byte wire status frame for CAN.
 * @param status Status byte: 0 for periodic status, 1 for disconnection.
 * @retval HAL_OK The frame was successfully queued.
 * @retval HAL_ERROR The fram could not be queued.
 * @note Successful queueing does not guarantee completed transmission.
 */

static HAL_StatusTypeDef CAN_SendStatus(uint8_t status)
{
	CAN_TxHeaderTypeDef header = {0};
	uint8_t data[8] = {0};
	uint32_t mailbox;

	header.ExtId = WIRE_CAN_ID;
	header.IDE = CAN_ID_EXT;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 1U;
	header.TransmitGlobalTime = DISABLE;

	data[0] = status;

	return HAL_CAN_AddTxMessage(&hcan, &header, data, &mailbox);
}

/**
 * @brief Accepts only extended data frames with WIRE_CAN_ID.
 * @note Payload length and contents are checked by the receive callback.
 */
static void CAN_ConfigFilter(void)
{
    CAN_FilterTypeDef filter = {0};

    /*
     * Extended ID occupies bits 31:3.
     * Bit 2 is IDE: 1 means extended frame.
     * Bit 1 is RTR: 0 means data frame.
     */
    uint32_t filter_id = (WIRE_CAN_ID << 3) | CAN_ID_EXT;
    uint32_t filter_mask = 0xFFFFFFFEUL;

    filter.FilterBank = 0U;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;

    filter.FilterIdHigh = (filter_id >> 16) & 0xFFFFU;
    filter.FilterIdLow = filter_id & 0xFFFFU;

    filter.FilterMaskIdHigh = (filter_mask >> 16) & 0xFFFFU;
    filter.FilterMaskIdLow = filter_mask & 0xFFFFU;

    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterActivation = ENABLE;

    if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief Processes pending frames in CAN receive FIFO 0.
 * @param hcan_ptr Handle of the CAN peripheral that raised the interrupt.
 * @note Data 0 turns the active-low LED on; data 1 toggles it.
 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan_ptr)
{
    CAN_RxHeaderTypeDef header;
    uint8_t data[8];

    if (hcan_ptr->Instance != CAN1)
    {
        return;
    }

    while (HAL_CAN_GetRxFifoFillLevel(hcan_ptr, CAN_RX_FIFO0) > 0U)
    {
        if (HAL_CAN_GetRxMessage(
                hcan_ptr,
                CAN_RX_FIFO0,
                &header,
                data) != HAL_OK)
        {
            return;
        }

        /* Check frame format and payload length. */
        if ((header.IDE != CAN_ID_EXT) ||
            (header.ExtId != WIRE_CAN_ID) ||
            (header.RTR != CAN_RTR_DATA) ||
            (header.DLC != 1U))
        {
            continue;
        }

        if (data[0] == 0U)
        {
            HAL_GPIO_WritePin(
                STATUS_LED_GPIO_Port,
                STATUS_LED_Pin,
                GPIO_PIN_RESET);
        }
        else if (data[0] == 1U)
        {
            HAL_GPIO_TogglePin(
                STATUS_LED_GPIO_Port,
                STATUS_LED_Pin);
        }
        /* Other payload values are ignored. */
    }
}

/**
 * @brief Records a falling edge on the critical-wire input.
 * @param GPIO_Pin Pin that triggered the external interrupt.
 * @note CAN transmission is performed by the main loop.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == WIRE_SENSE_Pin)
    {
        disconnect_events++;
    }
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
#ifdef USE_FULL_ASSERT
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
