#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main() {
	disp_str("-----\"kernel_main\" begins-----\n");

	TASK* p_task = task_table;
	PROCESS* p_proc	= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	t_16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;

	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);
		p_proc->ticks = p_proc->priority = p_task->priority;
		p_proc->pid = i;

		p_proc->ldt_sel	= selector_ldt;
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS>>3], 
				sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | 
				PRIVILEGE_TASK << 5;	// change the DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], 
				sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | 
				PRIVILEGE_TASK << 5; // change the DPL

		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | 
				SA_TIL | RPL_TASK;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | 
				SA_TIL | RPL_TASK;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | 
				SA_TIL | RPL_TASK;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | 
				SA_TIL | RPL_TASK;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | 
				SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | 
				RPL_TASK;
		p_proc->regs.eip= (t_32)p_task->initial_eip;
		p_proc->regs.esp= (t_32)p_task_stack;
		p_proc->regs.eflags = 0x1202; // IF=1, IOPL=1, bit 2 is always 1.

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += (1 << 3);
	}

	p_proc_ready = proc_table;
	k_reenter = 0;
	ticks = 0;

	init_clock();
	init_keyboard();

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA() {
//	int i = 0;
//	__asm__ volatile("int $0x32");
	while(1) {
		disp_str("A");
//		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}


/*======================================================================*
                               TestB
 *======================================================================*/
void TestB() {
//	int i = 0x1000;
	while (1) {
		disp_str("B");
//		disp_int(i++);
		disp_str(".");
		delay(1);
	}
}

