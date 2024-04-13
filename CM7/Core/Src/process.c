//Kernel Space Processes
#include <stdio.h>
#include <string.h>
#include "process.h"
#include "sh.h"
#include "blink.h"
/* USER CODE BEGIN Defines */
#define STACK_SIZE 25

/* _eustack for SP and sp_start of proc_table[1] */
extern const uint32_t _eustack[];

/* Struct Declarations from process.h */
struct task_struct proc_table[PROC_MAX];
struct task_struct *current = TASK_IDLE;
struct task_struct *shell_stack_init = &proc_table[1];

void stack_init(struct task_struct *task)
{
	/* Create stack pointer from task's pointer start*/
	uint32_t *sp = (uint32_t *) task->sp_start;
	
	/* Set FPSCR-S0 to 0 */
	for(int i = 0; i < 17; i++) {
		*(--sp) = 0;
	}

	/* Init R0 through xPSR */
	*(--sp) = (uint32_t) task->r.xPSR;
	*(--sp) = (uint32_t) task->r.PC;
	*(--sp) = (uint32_t) task->r.LR;
	*(--sp) = (uint32_t) task->r.R[12];
	*(--sp) = (uint32_t) task->r.R[3];
	*(--sp) = (uint32_t) task->r.R[2];
	*(--sp) = (uint32_t) task->r.R[1];	
	*(--sp) = (uint32_t) task->r.R[0];

	/* Store stack pointer in task struct */	
	task->r.SP = sp;
}

void proc_table_init(void) 
{
	/* Initalize entire process table to 0. */
	memset(proc_table, 0, sizeof(proc_table));	
	
	/* Setup proc_table[0] for Idle Task */
	proc_table[0].state = ST_RUN;
	proc_table[0].r.xPSR = (uint32_t *) (0x01000000);
	proc_table[0].exc_return = EXC_RETURN_THREAD_MSP_FPU;
	proc_table[0].pid = 0;

	/* Setup proc_table[1] for Shell Process */
	proc_table[1].r.SP = (uint32_t *)_eustack;
	proc_table[1].sp_start = (uint32_t *)_eustack;
	proc_table[1].r.LR = 0;
	proc_table[1].r.PC = (uint32_t *)&proc_start;
	proc_table[1].r.xPSR = (uint32_t *)0x01000000; /* Thumb-State */
	proc_table[1].state = ST_RUN;
	proc_table[1].cmd = &shell;
	proc_table[1].exc_return = EXC_RETURN_THREAD_PSP;
	proc_table[1].pid = 1;
	
	/* Initialize the rest of the shell process stack */
	stack_init(shell_stack_init);

	/* PDBlink process init */
	proc_table[2].state = ST_RUN;
	proc_table[2].r.SP = (uint32_t *)(_eustack - 0x800);
	proc_table[2].sp_start = (uint32_t *)(_eustack - 0x800);
	proc_table[2].r.xPSR = (uint32_t *)0x01000000; /* Thumb State */
	proc_table[2].r.PC = (uint32_t *)&proc_start;
	proc_table[2].cmd = &pdblink;
	proc_table[2].exc_return = EXC_RETURN_THREAD_PSP;
	proc_table[2].pid = 2;

	shell_stack_init = &proc_table[2];
	stack_init(shell_stack_init);
}

void proc_start(void)
{
	/* Call command, usually shell, from the current task */
	current->cmd();
	
	/* If function pointed to by cmd returns, set state to STOP */
	current->state = ST_STOP;

	/* Loop to keep function from returning */
	while(1) {

	}
}


struct task_struct* schedule(void)
{
	/* Persistent Process Table Entry Tracker */
	static uint32_t sch_index = 0;

	/* Iterates through the process table to find next available process */ 
	for(int i=0; i<4; i++){
		/* Prevents overflow of sch_index */
		if(sch_index == 4){
			sch_index = 0;
		}
		/* Checks for sleeping processes and wakes them if their scheduled wakeup time has passed */
		if(proc_table[sch_index].state == ST_TIME_SLEEP){
			/* Convert processes' time back to milliseconds for comparison with uwTick */
			uint32_t w_time_conv = proc_table[sch_index].w_time / 1000;

			/* Check if wake time has passed and return process to run state if it has. */
			if(w_time_conv < uwTick){
				proc_table[sch_index].state = ST_RUN;
			}
		} 
		/* Checks if process at index is able to be run and returns pointer if so */
		if(proc_table[sch_index].state == ST_RUN){
			/* Ensures that index is incremented when an address is returned so the scheduler is not permanently fixed on one process */
			sch_index++;
			return &proc_table[sch_index-1];
		}
		sch_index++;
	}
	/* If the iterator cannot find an available process it is sent to idle task. */
	return &proc_table[0];
}
