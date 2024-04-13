
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : sh.c
  * @brief          : Shell program body
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blink.h"
#include "main.h"
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  The application entry point.
  * @retval int
**/

/* Blink External LED on PD3 */
int pdblink(void)
{
	while(1){
		HAL_GPIO_TogglePin(LDX2_GPIO_Port, LDX2_Pin);
		microsleep(100);
	}
}

