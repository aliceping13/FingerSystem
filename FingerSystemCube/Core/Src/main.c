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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define IMU_ADDR       (0x6A << 1)
#define ACCEL_SENS     0.000244f    // ±8g: 0.244 mg/LSB
#define GYRO_SENS      0.070f       // ±2000 dps: 70 mdps/LSB

/* Register addresses */
#define REG_WHO_AM_I   0x0F
#define REG_CTRL1      0x10   // Accel control
#define REG_CTRL2      0x11   // Gyro control
#define REG_CTRL3      0x12   // BDU, IF_INC
#define REG_CTRL6      0x15   // Gyro full-scale
#define REG_CTRL8      0x17   // Accel full-scale
#define REG_STATUS     0x1E
#define REG_OUTX_L_G   0x22   // Gyro X low byte (burst: 22h-27h = gx,gy,gz)
#define REG_OUTZ_L_A   0x28   // Accel Z low byte (burst: 28h-2Dh = az,ay,ax)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
HAL_StatusTypeDef ret;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C2_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int __io_putchar(int ch) {
    HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  HAL_StatusTypeDef ret;
  uint8_t buf[12];
  char msg[128];

  // /* USER CODE BEGIN 2 */
  
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
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */

  // I2C scan
// WHO_AM_I check — like the temp sensor checking HAL_OK before proceeding
  buf[0] = REG_WHO_AM_I;
  ret = HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: WHO_AM_I Tx\r\n", 20, HAL_MAX_DELAY);
      Error_Handler();
  }
  ret = HAL_I2C_Master_Receive(&hi2c2, IMU_ADDR, buf, 1, HAL_MAX_DELAY);
  if (ret != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: WHO_AM_I Rx\r\n", 20, HAL_MAX_DELAY);
      Error_Handler();
  }
  if (buf[0] != 0x71) {
      sprintf(msg, "IMU not found! WHO_AM_I=0x%02X\r\n", buf[0]);
      HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
      Error_Handler();
  }
  HAL_UART_Transmit(&huart2, (uint8_t*)"IMU found!\r\n", 12, HAL_MAX_DELAY);

  // Configure IMU — each write checked like the temp sensor example
  // CTRL3: enable BDU (bit 6) + IF_INC (bit 4, default 1) = 0x40
  //   BDU prevents reading a high byte from one sample and low byte from the next
  buf[0] = REG_CTRL3; buf[1] = 0x40;
  if (HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: CTRL3\r\n", 14, HAL_MAX_DELAY);
      Error_Handler();
  }

  // CTRL6: gyro full-scale ±2000 dps = FS_G[3:0] = 0100 = 0x04
  buf[0] = REG_CTRL6; buf[1] = 0x04;
  if (HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: CTRL6\r\n", 14, HAL_MAX_DELAY);
      Error_Handler();
  }

  // CTRL8: accel full-scale ±8g = FS_XL[1:0] = 10 = 0x02
  buf[0] = REG_CTRL8; buf[1] = 0x02;
  if (HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: CTRL8\r\n", 14, HAL_MAX_DELAY);
      Error_Handler();
  }

  // CTRL2: gyro 120 Hz high-performance = ODR_G[3:0] = 0110 = 0x06
  buf[0] = REG_CTRL2; buf[1] = 0x06;
  if (HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: CTRL2\r\n", 14, HAL_MAX_DELAY);
      Error_Handler();
  }

  // CTRL1: accel 120 Hz high-performance = ODR_XL[3:0] = 0110 = 0x06
  buf[0] = REG_CTRL1; buf[1] = 0x06;
  if (HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 2, HAL_MAX_DELAY) != HAL_OK) {
      HAL_UART_Transmit(&huart2, (uint8_t*)"Error: CTRL1\r\n", 14, HAL_MAX_DELAY);
      Error_Handler();
  }

/* USER CODE END 2 */


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint8_t test[] = "alive\r\n";
    HAL_UART_Transmit(&huart2, test, sizeof(test)-1, HAL_MAX_DELAY);
    HAL_Delay(500);    

    // Poll STATUS_REG until both accel (bit 0) and gyro (bit 1) are ready
    // Same pattern as temp sensor: Tx register address, then Rx data
    uint8_t status = 0;
    while (!(status & 0x03)) {
        buf[0] = REG_STATUS;
        ret = HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 1, HAL_MAX_DELAY);
        if (ret != HAL_OK) { continue; }
        ret = HAL_I2C_Master_Receive(&hi2c2, IMU_ADDR, &status, 1, HAL_MAX_DELAY);
        if (ret != HAL_OK) { status = 0; }
    }

    // Burst read 6 bytes of gyro: 0x22-0x27 (gx_L, gx_H, gy_L, gy_H, gz_L, gz_H)
    buf[0] = REG_OUTX_L_G;
    ret = HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error Tx gyro\r\n", 15, HAL_MAX_DELAY);
        continue;
    }
    ret = HAL_I2C_Master_Receive(&hi2c2, IMU_ADDR, buf, 6, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error Rx gyro\r\n", 15, HAL_MAX_DELAY);
        continue;
    }

    int16_t gx_raw = (int16_t)(buf[1] << 8 | buf[0]);
    int16_t gy_raw = (int16_t)(buf[3] << 8 | buf[2]);
    int16_t gz_raw = (int16_t)(buf[5] << 8 | buf[4]);

    // Burst read 6 bytes of accel: 0x28-0x2D (az_L, az_H, ay_L, ay_H, ax_L, ax_H)
    // NOTE: register order from datasheet is Z, Y, X — not X, Y, Z!
    buf[0] = REG_OUTZ_L_A;
    ret = HAL_I2C_Master_Transmit(&hi2c2, IMU_ADDR, buf, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error Tx accel\r\n", 16, HAL_MAX_DELAY);
        continue;
    }
    ret = HAL_I2C_Master_Receive(&hi2c2, IMU_ADDR, buf, 6, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Error Rx accel\r\n", 16, HAL_MAX_DELAY);
        continue;
    }

    // Datasheet order: [0,1]=az  [2,3]=ay  [4,5]=ax
    int16_t az_raw = (int16_t)(buf[1] << 8 | buf[0]);
    int16_t ay_raw = (int16_t)(buf[3] << 8 | buf[2]);
    int16_t ax_raw = (int16_t)(buf[5] << 8 | buf[4]);

    float gx = gx_raw * GYRO_SENS;
    float gy = gy_raw * GYRO_SENS;
    float gz = gz_raw * GYRO_SENS;
    float ax = ax_raw * ACCEL_SENS;
    float ay = ay_raw * ACCEL_SENS;
    float az = az_raw * ACCEL_SENS;

    sprintf(msg, "Accel (g):  %.3f  %.3f  %.3f\r\n", ax, ay, az);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    sprintf(msg, "Gyro (dps): %.2f  %.2f  %.2f\r\n", gx, gy, gz);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);

    HAL_Delay(100);
  }
  /* USER CODE END WHILE */
    
  /* USER CODE BEGIN 3 */
    
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00503D58;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
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
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
