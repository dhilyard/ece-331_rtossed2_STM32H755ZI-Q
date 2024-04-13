//User System Calls
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "main.h"
#include "user_syscalls.h"
#include "process.h"
/* USER CODE BEGIN Defines */
#define __NR_millisleep 2
#define __NR_kill 5

uint32_t microsleep(uint32_t time_sleep)
{
	/* Conversion from milliseconds to microseconds */
	uint32_t time_sleep_micro = 1000 * time_sleep;

	/* Convert uwTick to microseconds */
	uint32_t uwTick_micro = uwTick * 1000;
	/* Set current process wake time to current time + time to sleep */
	current->w_time = time_sleep_micro + uwTick_micro;
	/* Set current process state to Sleep */
	current->state = ST_TIME_SLEEP;
	
	/* Calculate remaining sleep time */
	uint32_t sleep_rem = current->w_time - uwTick;

	/* Yield Processor */
	yield();

	return sleep_rem;
}

// Kill user space system call - does nothing in kernel space
int _kill(int pid, int sig)
{
	register uint32_t r0 asm("r0"); // IN: Syscall # - OUT: return value
	register uint32_t r1 asm("r1"); // IN: pid - OUT: errno
	register uint32_t r2 asm("r2"); // IN: sig
	int ret; // Stage return value
	//int errno;
	// Load registers and transition to kernel space
	__asm__ __volatile__ (
	"movs %0,%3\n\t"
	"ldr %1,%4\n\t"
	"ldr %2,%5\n\t"
	"svc 42\n\t"
	: "=r" (r0), "=r" (r1), "=r" (r2)
	: "i" (__NR_kill), "m" (pid), "m" (sig)
	:
	);
	__DSB();
	__ISB();
	
	ret=r0; // Collect return value
	errno=r1; // & errno from kernel
	return ret;
}


// millisleep user space kernel call
int _millisleep(int req) // req value assumed to be in ms
{
	register uint32_t r0 asm("r0"); // IN: Syscall # - OUT: Return Value
	register uint32_t r1 asm("r1"); // IN: req - OUT: errno
        int ret; // Stage return value
        //int errno;
	// Load registers and transition to kernel space
        __asm__ __volatile__ (
        "movs %0,%2\n\t"
        "ldr %1,%3\n\t"
        "svc #0\n\t"
        : "=r" (r0), "=r" (r1)
        : "i" (__NR_millisleep), "m" (req)
	:
	);

	/* Memory Barrier Calls */
        __DSB();
	__ISB();
	
	/* Wait for schedule to resume process */
	while (((SCB->ICSR)&SCB_ICSR_PENDSVSET_Msk)==SCB_ICSR_PENDSVSET_Msk);
	
	/* Collect return values */
	ret=r0;
	errno=r1;

	/* Return from function */
	return ret;	
}

