//User System Calls Header

#ifndef USER_SYSCALLS_H
#define USER_SYSCALLS_H

/* Includes ------------------------------------------------*/
#include "main.h"
#include <sys/types.h>
#include <stdio.h>
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
uint32_t microsleep(uint32_t time_sleep);
int _kill(int pid, int sig);
int _millisleep(int req);
/* Function Prototypes */

/* External Structure Declarations */

#endif				/* __USER_SYSCALLS_H__ */
