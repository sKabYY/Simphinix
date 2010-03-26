#ifndef _PROC_H_
#define _PROC_H_

#include <type.h>
#include <protect.h>
#include <timer.h>

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
	int pid;

	t_16 ldt_sel;
	DESCRIPTOR ldts[LDT_SIZE];

	clock_t ticks;
	timer_t timer;
	int priority;

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

typedef struct s_image {
	int proc_nr;
	t_pf_task initial_eip;
	int stacksize;
	char name[16];
} IMAGE;

#define proc2pid(x) (x - proc_table)

#define NR_TASKS 4
#define NR_PROCS 2

#define TSK_S PT_STACK
#define IDL_S NO_STACK
#define HRD_S NO_STACK

#define PT_STACK 0x8000
#define NO_STACK 0

#define STACK_SIZE_TOTAL (PT_STACK* (NR_TASKS+NR_PROCS))

#endif

