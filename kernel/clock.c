#include "const.h"
#include "type.h"
#include "proc.h"
#include "proto.h"
#include "global.h"

PRIVATE void do_clocktick(message* msg);
PRIVATE void do_schedule();

PUBLIC void clock_task() {
	message msg;
	init_clock();
	while (1) {
		sendrec(RECEIVE, ANY, &msg);
		switch (msg.type) {
			case HARD_INT:
				do_clocktick(&msg);
				break;
			default:
				//TODO
				break;
		}
	}
}

PRIVATE void do_clocktick(message* msg) {
	do_schedule();
}

PRIVATE void do_schedule() {
	PROCESS* p;
	int greatest_ticks = 0;
	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++) {
//dbgprtstr(p->p_name);
//dbgprtint(p->p_flags);
//dbgprtstr(" ");
			if (p->p_flags == 0 && proc2pid(p) != CLOCK) {
				if (p->ticks > greatest_ticks) {
					greatest_ticks = p->ticks;
					p_proc_ready = p;
				}
			}
		}
		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table + NR_TASKS+NR_PROCS; p++) {
				if (p->p_flags <= 0)
					p->ticks = p->priority;
			}
		}
	}
//dbgprtstr(p_proc_ready->p_name);dbgprtint(p_proc_ready->p_flags);
}

PUBLIC void init_clock() {
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (t_8)(TIMER_FREQ/HZ));
	out_byte(TIMER0, (t_8)((TIMER_FREQ/HZ) >> 8));

	put_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
}

PUBLIC void clock_handler(int irq) {
	ticks++;
	proc_ptr->ticks--;

	if (k_reenter != 0) {
		return;
	}

	if (proc_ptr->ticks <= 0)
		schedule();
	if (proc_ptr->p_flags)
		proc_ptr = p_proc_ready;
}

