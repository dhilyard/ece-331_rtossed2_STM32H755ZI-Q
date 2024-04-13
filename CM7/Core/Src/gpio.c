/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PC14-OSC32_IN (OSC32_IN)   ------> RCC_OSC32_IN
     PC15-OSC32_OUT (OSC32_OUT)   ------> RCC_OSC32_OUT
     PH0-OSC_IN (PH0)   ------> RCC_OSC_IN
     PH1-OSC_OUT (PH1)   ------> RCC_OSC_OUT
*/
void MX_GPIO_Init(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

/* USER CODE BEGIN 2 */

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

//Pin Write
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);	//LD1
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_RESET);	//LD2
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	//LD3
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);	//TFT_RST
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);	//TFT_DC
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);	//B1
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET); //LDX - PD2
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET); //LDX2 - PD3
//Initialize
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);	//LD1
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	//TFT_RST

	GPIO_InitStruct.Pin = GPIO_PIN_1;

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	//LD2

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	//TFT_DC

	GPIO_InitStruct.Pin = GPIO_PIN_2; //LDX

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_3; //LDX2

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	GPIO_InitStruct.Pin = GPIO_PIN_14;	//LD3

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13;	//B1
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

/* USER CODE END 2 */
}
