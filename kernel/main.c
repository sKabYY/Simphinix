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
//disp_pos=0;

	TASK* p_task = task_table;
	PROCESS* p_proc	= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	t_16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;

	proc_table[KERNEL].pid = KERNEL;
	proc_table[KERNEL].p_flags |= UNRUNNABLE;

	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
//dbgprtstr("init proc: ");dbgprtstr(p_task->name);dbgprtstr("\n");
		strcpy(p_proc->p_name, p_task->name);
		p_proc->ticks = p_proc->priority = 10;
		p_proc->pid = i;
		p_proc->p_flags = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = NULL;
		p_proc->next_sending = NULL;
//dbgprtstr("init proc: ");dbgprtstr(p_proc->p_name);
//dbgprtstr(": ");dbgprtint(p_proc->pid);dbgprtstr("\n");

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

	proc_ptr = p_proc_ready = proc_table;
	current_pid = proc2pid(proc_ptr);
	k_reenter = 0;
	ticks = 9;

//	init_clock();
	init_keyboard();

	disp_str("-----\"kernel_main\" ends-----\n");

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
PUBLIC void TestA() {
	while(1) {
		disp_str("A");
		disp_int(get_ticks());
		disp_str(".");
		delay(1);
	}
}


/*======================================================================*
                               TestB
 *======================================================================*/
PUBLIC void TestB() {
	while (1) {
		disp_str("B");
		disp_int(get_ticks());
		disp_str(".");
		delay(1);
	}
}

PUBLIC int get_ticks() {
	message msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	sendrec(BOTH, SYSTEM, &msg);
	return msg.RETVAL;
}

