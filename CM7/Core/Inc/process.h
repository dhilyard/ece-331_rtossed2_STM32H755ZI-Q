//Kernel space header

#ifndef PROCESS_H
#define PROCESS_H

/* Includes ------------------------------------------------*/
#include "main.h"
#include <sys/types.h>
#include <stdio.h>
extern volatile int kready;
/* USER CODE BEGIN Private defines */
#define PROC_MAX 4
#define STACK_BITS 800		/* 25 32-bit registers */

/* Process States */
#define ST_UNUSED 0b000
#define ST_TIME_SLEEP 0b001
#define ST_IO_SLEEP 0b010
#define ST_RUN 0b011
#define ST_STOP 0b100
#define ST_ZOMBIE 0b101

/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */

/* Enable PendSV and Barriers */
static inline void yield() 
{
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	/* Data and Instruction Sync Barriers */
	__DSB();
	__ISB();
}

/* Core Registers Struct */
struct __attribute__((packed)) registers{
	uint32_t *xPSR; /* Program Status Registers */
	uint32_t *PC;   /* Program Counter */
	uint32_t *LR;   /* Link Register */
	uint32_t *SP;   /* Stack Pointer */
	uint32_t *R[13];        /* R0-R12 */
};

/* Process Data Structure */
struct __attribute__((packed)) task_struct{
	uint32_t state;	/* State Based on Bitmap of Defines above */
	pid_t pid;	/* Process ID */
	uint32_t exc_return;	/* Exception Handler Return */
	uint32_t *sp_start;	/* Starting stack pointer address */
	int (*cmd)();	/* Pointer to a command with no input args and int return. */
	uint32_t w_time; /* Wake Time - Stored in Microseconds */
	struct registers r;
};

/* Save registers from current process onto the stack*/
static inline void reg_save(void) 
{
	__asm__ __volatile__(
		"push {r4-r11}\n\t"
		:
		:
		: "memory"
	);
}

/* Restore registers from stack to the next process struct */
static inline void restore_regs(uint32_t nextR4Addr)
{
	__asm__  __volatile__(
		"ldmia %0, {r4-r11}\n\t" /* Load R4-R11 starting at R4 incrementing after */
		: "+r" (nextR4Addr)
		:
		: "memory"
	);
}

/* EXC_RETURN Function to swap stack pointers*/
static inline void switch_context_return(uint32_t exc_return_addr) 
{
	__asm__ __volatile__(
		"ldr pc, [%0]\n\t"	// Load the value of exc_return into pc      
		:
		:"r"(exc_return_addr)
		:
	);
}

/* Function Prototypes */
void stack_init(struct task_struct * task);
void proc_table_init(void);
void proc_start(void);
struct task_struct *schedule(void);

/* External Structure Declarations */
extern struct task_struct proc_table[PROC_MAX];
extern volatile struct task_struct *next;
extern struct task_struct *current;
#define TASK_IDLE &proc_table[0]

#endif				/* __PROCESS_H__ */


