#ifndef _SIMPHINIX_PROC_H_
#define _SIMPHINIX_PROC_H_

#include "type.h"
#include "protect.h"

typedef struct s_stackframe {
	t_32 gs;
	t_32 fs;
	t_32 es;
	t_32 ds;
	t_32 edi;
	t_32 esi;
	t_32 ebp;
	t_32 kernel_esp;
	t_32 ebx;
	t_32 edx;
	t_32 ecx;
	t_32 eax;
	t_32 retaddr;
	t_32 eip;
	t_32 cs;
	t_32 eflags;
	t_32 esp;
	t_32 ss;
} STACK_FRAME;

typedef struct s_proc {
	STACK_FRAME regs;

	t_16 ldt_sel;
	DESCRIPTOR ldts[LDT_SIZE];

	int ticks;
	int priority;

	t_32 pid;
	char p_name[16];

	int p_flags;
	message* p_msg;
	int p_recvfrom;
	int p_sendto;
	int has_int_msg;
	struct s_proc* q_sending;
	struct s_proc* next_sending;
	int nr_tty;
} PROCESS;

typedef struct s_task {
	t_pf_task initial_eip;
	int priority;
	int stacksize;
	char name[32];
} TASK;

#define NR_TASKS 2

#define STACK_SIZE_TESTA 0x8000
#define STACK_SIZE_TESTB 0x8000

#define STACK_SIZE_TOTAL (STACK_SIZE_TESTA + STACK_SIZE_TESTB)

#endif

