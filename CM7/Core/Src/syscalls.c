/*!
 * @file
 * 
 * @brief Support files for GNU libc.
 * 
 * @author  stolen from Sheaff.  
 * 
 * @date Jan  2016
 * 
 * Some standard syscall functions are redefined here in order to 
 * make printf() calls direct output to the USART (to be accessed 
 * by the host when the development board is connected through the
 * USB connector)
 * 
 * note the changes to _write()
 */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>
#include "usart.h"
#include "process.h"
#include "user_syscalls.h"

#define FreeRTOS
#define MAX_STACK_SIZE 0x2000

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));
static struct task_struct *io_wait = NULL;

#ifndef FreeRTOS
register char *stack_ptr asm("sp");
#endif

caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end, *min_stack_ptr;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;

#ifdef FreeRTOS
	/* Use the NVIC offset register to locate the main stack pointer. */
	min_stack_ptr = (char *)(*(unsigned int *)*(unsigned int *)0xE000ED08);
	/* Locate the STACK bottom address */
	min_stack_ptr -= MAX_STACK_SIZE;

	if (heap_end + incr > min_stack_ptr)
#else
	if (heap_end + incr > stack_ptr)
#endif
	{
//              write(1, "Heap and stack collision\n", 25);
//              abort();
		errno = ENOMEM;
		return (caddr_t) - 1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday(struct timeval *tp, struct timezone *tzp)
{
	/* Return fixed data for the timezone.  */
	if (tzp) {
		tzp->tz_minuteswest = 0;
		tzp->tz_dsttime = 0;
	}

	return 0;
}

void initialise_monitor_handles()
{
}

int _getpid(void)
{
	return 1;
}

/* Replaced by _kill in user_syscalls.c 
int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}
*/

void _exit(int status)
{
	_kill(status, -1);
	while (1) {
	}
}

int _write(int file, char *ptr, int len)
{
	// Transmit printf string to USART - blocking call
	HAL_UART_Transmit(&huart3, (uint8_t *) ptr, len, 10000);
	return len;
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _read(int fd, char *buf, int cnt){
	
	/* Interrupt based terminal character read */
	if(sizeof(*buf)==0){
		return 0;
	}
	//Interrupt UART Receive
	HAL_UART_Receive_IT(&huart3, (uint8_t *) buf, 1);
	//Enable IO Sleep for current process
	current->state = ST_IO_SLEEP;
	io_wait = current;
	yield();
	return 1;

	/* Old _read
	// Read characters from terminal
	HAL_UART_Receive(&huart3, (uint8_t *) buf, 1, HAL_MAX_DELAY);
	return 1;
	*/
}

/* USART3 Interrupt Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	io_wait->state = ST_RUN;
	yield();
}

/* Original Read Function
int _read(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++) {
		*ptr++ = __io_getchar();
	}

	return len;
}*/

int _open(char *path, int flags, ...)
{
	/* Pretend like we always fail */
	return -1;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	return -1;
}

int _stat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}
