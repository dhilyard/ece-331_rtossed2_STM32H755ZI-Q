/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma2d.h"
#include "eth.h"
#include "mdma.h"
#include "quadspi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"
#include "Adafruit_ST7735.h"
#include "frame_buffer.h"
#include "graphics.h"
#include "usart.h"
#include "sh.h"
#include "progs.h"
#include "process.h"

//Make Memory locations visible
const SCnSCB_Type *scnscb = SCnSCB;
const SCB_Type *scb = SCB;
const SysTick_Type *systick = SysTick;
const NVIC_Type *nvic = NVIC;
const ITM_Type *itm = ITM;
const DWT_Type *dwt = DWT;
const TPI_Type *tpi = TPI;
const CoreDebug_Type *coredebug = CoreDebug;
const MPU_Type *mpu = MPU;
const FPU_Type *fpu = FPU;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U)		/* HW semaphore 0 */
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
volatile int kready=0;
extern const uint32_t _eustack[];
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */
	//char ch;
	/* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
	int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
	/* Wait until CPU2 boots and enters in stop mode or timeout */
	timeout = 0xFFFF;
	while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET)
	       && (timeout-- > 0)) ;
	if (timeout < 0) {
		Error_Handler();
	}
/* USER CODE END Boot_Mode_Sequence_1 */
	/* MCU Configuration-------------------------------------------------------- */

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
	__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
	HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
	HAL_HSEM_Release(HSEM_ID_0, 0);
/* wait until CPU2 wakes up from stop mode */
	timeout = 0xFFFF;
	while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET)
	       && (timeout-- > 0)) ;
	if (timeout < 0) {
		Error_Handler();
	}
/* USER CODE END Boot_Mode_Sequence_2 */

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_MDMA_Init();
	MX_USART3_UART_Init();
	MX_DMA2D_Init();
	MX_ETH_Init();
	MX_QUADSPI_Init();
	MX_TIM15_Init();
	MX_USB_DEVICE_Init();
	char buf[48] = "Hello World";
	/* USER CODE BEGIN 2 */

	/* Enable I & D Caches */
	SCB_EnableICache();
	SCB_EnableDCache();

	/* Initialize TFT Screen*/
	graphics_init(160, 128);
	graphics_setTextWrap(false);
	graphics_setTextColor(graphics_mk_color(0x0F800));
	graphics_setTextSize(2);
	memset(frame, 0x00, 160 * 128 * 2);
	Adafruit_ST7735_init();
	qspi_write_frame(frame);
	HAL_TIM_Base_Start_IT(&htim15);
	HAL_Delay(1000);
	frame_buffer_fillRect(10, 10, 25, 25, 0xFFFF);
	graphics_drawText(buf, 10, 75);

	/* Disable buffering*/
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IOLBF, 0);
	HAL_Delay(2500);

	/* Set PSP to _eustack */
	__set_PSP((int) _eustack);

	/* USER CODE END 2 */
	/* Infinite loop */	
	/* ALLOW USER SPACE PROCESSES */	
	proc_table_init();
	kready = 1;
	//procTable_init();
	
	
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		//HAL_GPIO_TogglePin(LDX_GPIO_Port, LDX_Pin);
		HAL_Delay(100);
		//shell();
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
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Supply configuration update enable
  */
	HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 100;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

				/** Initializes the CPU, AHB and APB buses clocks
  */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
	    | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
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
	while (1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);

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
void assert_failed(uint8_t * file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif				/* USE_FULL_ASSERT */
