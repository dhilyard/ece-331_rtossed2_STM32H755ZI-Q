/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32h7xx_it.h"
#include "process.h"
#include <stdio.h>
#include "kernel_syscalls.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define __NR_millisleep 2
#define __NR_kill 5
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint32_t uwTickCNT = 0;
//volatile int kready = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern MDMA_HandleTypeDef hmdma_quadspi_fifo_th;
extern QSPI_HandleTypeDef hqspi;
extern TIM_HandleTypeDef htim15;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1) {
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
	/* USER CODE BEGIN HardFault_IRQn 0 */
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
	uint32_t *psp=(uint32_t *)__get_PSP(); // Points to R0 on exception frame
	uint32_t register r0 asm("r0"); // syscall number
	pid_t pid; // PID
	int sig; // Signal to be sent
	uint32_t req; // Time to sleep in milliseconds
	// Add variable here
	// Act on a system call
	switch (r0) {
		
	case __NR_millisleep:
		__asm__ __volatile__ (
			"str r1,%0\n\t"
			: "=m" (req)
		);
		*psp=(uint32_t)sys_millisleep(req);
		*(psp+1)=0;
		break;

	case __NR_kill:
		__asm__ __volatile__ (
			"str r1,%0\n\t"
			"str r2,%1\n\t"
			: "=m" (pid), "=m" (sig)
		);
		*psp=(uint32_t)sys_kill(pid, sig);
		*(psp+1)=0;
		break;

	default:
		*psp=-1;
		*(psp+1)=-22;
		break;
	}
	return;
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
	/* USER CODE BEGIN PendSV_IRQn 0 */
	/* Push R4-R11 to the stack */
	reg_save();

	struct task_struct *next;
	
	/* Create Secondary Stack Pointer */

	register uint32_t *sp asm("sp");
	/* Set next equal to output of the scheduler */
	next = schedule();
	


	/* Copy & Save registers R4-R11 from the stack to current process struct */	
	current->r.R[4] = *(sp);
	current->r.R[5] = *(++sp);
	current->r.R[6] = *(++sp);
	current->r.R[7] = *(++sp);
	current->r.R[8] = *(++sp);
	current->r.R[9] = *(++sp);
	current->r.R[10] = *(++sp);
	current->r.R[11] = *(++sp);
	for(int i=0; i<5; i++){
		*(sp++);
	}
	/* Checks for PSP Behavior based on current/next process */	
	if(next == &proc_table[0]) {
		current->r.SP = __get_PSP();
	} else if(current == &proc_table[0]){
		__set_PSP((uint32_t)next->r.SP);
	} else {
		current->r.SP = __get_PSP();
		__set_PSP((uint32_t)next->r.SP);
	}
	
	/* Prepare current for the next process */
	current = next;

	/* Clean up stack by pointing secondary sp to location prior to reg_save() */
	//sp = sp_clean;

	/* Restore R4-R11 to the next process struct */	
	uint32_t nextR4Addr = &next->r.R[4];
	restore_regs(nextR4Addr);

	/* Return from Exception and load PC with exc_return value from next struct */
	uint32_t exc_return_addr = &next->exc_return;
	switch_context_return(exc_return_addr);

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */
	/* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
	/* USER CODE BEGIN SysTick_IRQn 0 */
	if(uwTickCNT >= 32 && kready==1){
		HAL_GPIO_TogglePin(LDX_GPIO_Port, LDX_Pin);
		yield();
		uwTickCNT = 0;
	}
	/* USER CODE END SysTick_IRQn 0 */
	uwTickCNT++;
	uwTick += (uint32_t)uwTickFreq;
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles QUADSPI global interrupt.
  */
void QUADSPI_IRQHandler(void)
{
	/* USER CODE BEGIN QUADSPI_IRQn 0 */

	/* USER CODE END QUADSPI_IRQn 0 */
	HAL_QSPI_IRQHandler(&hqspi);
	/* USER CODE BEGIN QUADSPI_IRQn 1 */

	/* USER CODE END QUADSPI_IRQn 1 */
}

/* Interrupt Vector */
void USART3_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart3);
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
	/* USER CODE BEGIN OTG_FS_IRQn 0 */

	/* USER CODE END OTG_FS_IRQn 0 */
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
	/* USER CODE BEGIN OTG_FS_IRQn 1 */

	/* USER CODE END OTG_FS_IRQn 1 */
}

/**
  * @brief This function handles TIM15 global interrupt.
  */
void TIM15_IRQHandler(void)
{
	/* USER CODE BEGIN TIM15_IRQn 0 */

	/* USER CODE END TIM15_IRQn 0 */
	HAL_TIM_IRQHandler(&htim15);
	/* USER CODE BEGIN TIM15_IRQn 1 */

	/* USER CODE END TIM15_IRQn 1 */
}

/**
  * @brief This function handles MDMA global interrupt.
  */
void MDMA_IRQHandler(void)
{
	/* USER CODE BEGIN MDMA_IRQn 0 */

	/* USER CODE END MDMA_IRQn 0 */
	HAL_MDMA_IRQHandler(&hmdma_quadspi_fifo_th);
	/* USER CODE BEGIN MDMA_IRQn 1 */

	/* USER CODE END MDMA_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

