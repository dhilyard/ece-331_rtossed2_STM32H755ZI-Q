//Kernel System Calls Header

#ifndef KERNEL_SYSCALLS_H
#define KERNEL_SYSCALLS_H

/* Includes ------------------------------------------------*/
#include "main.h"
#include <sys/types.h>
#include <stdio.h>
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
int sys_millisleep(uint32_t req);
int sys_kill(pid_t pid, int sig);

/* Function Prototypes */

/* External Structure Declarations */

#endif				/* __KERNEL_SYSCALLS_H__ */
