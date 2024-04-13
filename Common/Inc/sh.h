
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sh.h
  * @brief   This file contains all the function prototypes for
  *          the sh.c file
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SH_H
#define SH_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define BACKSPACE 8
#define DELETE 127

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
char *getLine(char buffer[48]);
int parser(char *line, char *tokens[48]);
int shell(void);
int idle(void);
/* USER CODE END Prototypes */


#endif /*__ SH_H__ */

