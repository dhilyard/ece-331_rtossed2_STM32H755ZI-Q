
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
#include "sh.h"
#include "main.h"
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  The application entry point.
  * @retval int
**/

char *getLine(char buffer[48]) //Console line retrieval
{	
	int pos=0;
	char ch;
	while(1){
		ch=getchar();
		if(ch=='\r'|| ch=='\n'){ //End Conditions
			buffer[pos] = '\0';
			printf("%c", ch);
			return buffer;
		} else if(ch == BACKSPACE || ch == DELETE){// Character delete handler
			printf("%c", ch);
			if(pos > 0){
				pos--;
				printf(" \b \b"); //Format to make backspace look visually correct.
			}
		} else if(pos==47){ //Don't print any characters if at end of buffer 
			buffer[pos] = '\0';
		} else { //Handles all normal characters
			printf("%c", ch);
			buffer[pos] = ch;
			pos++;
		}	
	}
	return buffer;
}

int parser(char *line, char *tokens[48])
{
	printf("\r\n"); //Formatting to prevent overwrite of entered command.
	int pos = 0;
	int echof = 0;
	char* token = strtok(line, " ");
	int wordCount = 0;
	while(token != NULL && wordCount < 25){
		//strcpy(tokens[pos], token); //Strcpy causing HardFault
		int tklen = strlen(token);
		token[tklen] = '\0';
		tokens[pos] = token;	
		//tokens[pos][strlen(token)] = '\0';
		if(echof == 1){ //Print echo'd args
			printf("%s ", tokens[pos]);
		}
		//Echo check
		int result = strcmp(tokens[0], "echo");
		if(result==0){ 
			echof=1;
		}
		wordCount++;
		token = strtok(NULL, " ");
		pos++;
	}
	if(echof==1){
		printf("\r\n");
	}
	return wordCount;
}

int shell(void)
{
	char *line;
	int argc;
	char buffer[48];
	char *tokens[48];

	do{
		printf("$ "); /* Command Line Cursor */
		line = getLine(buffer); /* Get Line from stdio */
		argc = parser(line, tokens); /*Split Line into words */
		printf("Word Count: %d\r\n", argc);
	} while(1);
}
