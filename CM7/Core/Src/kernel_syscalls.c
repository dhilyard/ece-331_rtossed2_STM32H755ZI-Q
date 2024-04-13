#include <stdio.h>
#include <string.h>
#include "main.h"
#include "kernel_syscalls.h"
#include "process.h"

int sys_millisleep(uint32_t req) // req is assumed to be in ms
{
	/* Convert to microseconds as w_time is stored in microseconds */
	uint32_t req_micro = req * 1000;
	uint32_t uwTick_micro = uwTick * 1000;

	/* Store wake time in current process */
	current->w_time = req_micro + uwTick_micro;
	/* Set current process to sleep */
	current->state = ST_TIME_SLEEP;
	
	/* yield and then return req */
	yield();
	return req;
}
int sys_kill(pid_t pid, int sig)
{
	return 0;
}
