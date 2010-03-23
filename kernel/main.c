#include <type.h>
#include <const.h>
#include <protect.h>
#include <proto.h>
#include <string.h>
#include <proc.h>
#include <global.h>

/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main() {
	disp_str("-----\"kernel_main\" begins-----\n");
//disp_pos=0;

	IMAGE* p_image = image;
	PROCESS* p_proc	= proc_table - NR_TASKS;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	t_16 selector_ldt = SELECTOR_LDT_FIRST;
	t_8 privilege;
	t_8 rpl;
	int eflags;
	int prio;
	int i;

	for (i = -NR_TASKS; i < NR_PROCS; i++) {
//dbgprtstr("init proc: ");dbgprtstr(p_image->name);dbgprtstr("\n");
		if (i < 0) {
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202;
			prio = 15;
		}
		else {
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202;
			prio = 5;
		}
		strcpy(p_proc->p_name, p_image->name);
		p_proc->ticks = p_proc->priority = prio;
		p_proc->pid = p_image->proc_nr;
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
				privilege << 5;	// change the DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], 
				sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | 
				privilege << 5; // change the DPL

		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p_proc->regs.eip= (t_32)p_image->initial_eip;
		p_proc->regs.esp= (t_32)p_task_stack;
		p_proc->regs.eflags = eflags; // IF=1, IOPL=1, bit 2 is always 1.

		p_task_stack -= p_image->stacksize;
		p_proc++;
		p_image++;
		selector_ldt += (1 << 3);
	}
	p_proc = proc_table + IDLE;
	p_proc->ticks = p_proc->priority = 4;
	p_proc = proc_table + KERNEL;
	p_proc->p_flags |= UNRUNNABLE;

	proc_ptr = p_proc_ready = proc_table + CLOCK;
	current_pid = proc2pid(proc_ptr);
	k_reenter = 0;
	ticks = 0;

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
	message m;
	while(1) {
		disp_str("#");
		disp_int(get_ticks());
		disp_str(".");
//		sendrec(RECEIVE, ANY, &m);
		delay(3);
	}
}


/*======================================================================*
                               TestB
 *======================================================================*/
PUBLIC void TestB() {
	message m;
	while (1) {
		disp_str("%");
		disp_int(get_ticks());
		disp_str(".");
//		sendrec(RECEIVE, ANY, &m);
		delay(3);
	}
}

PUBLIC int get_ticks() {
	message msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	sendrec(BOTH, SYSTEM, &msg);
	return msg.RETVAL;
}

