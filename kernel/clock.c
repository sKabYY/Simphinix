#include <const.h>
#include <type.h>
#include <proc.h>
#include <proto.h>
#include <global.h>

PRIVATE timer_t* clock_timers;
PRIVATE clock_t next_timeout;
PRIVATE int clock_lock;
PRIVATE clock_t ticks;

PRIVATE void init_clock();
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
	if (next_timeout <= ticks) {
		tmrs_exptimers(&clock_timers, ticks, NULL);
		next_timeout = clock_timers == NULL ?
				TMR_NEVER : clock_timers->tmr_exp_time;
	}
	do_schedule();
}

PRIVATE void do_schedule() {
	PROCESS* p;
	int greatest_ticks = 0;
	int is_all_block = 1;
	while (greatest_ticks <= 0) {
		for (p = proc_table-NR_TASKS; p < proc_table+NR_PROCS; p++) {
//dbgprtstr(p->p_name);
//dbgprtint(p->p_flags);
//dbgprtstr(" ");
			if (p->p_flags == 0 && proc2pid(p) != CLOCK && 
					proc2pid(p) != IDLE) {
				is_all_block = 0;
				if (p->ticks > greatest_ticks) {
					greatest_ticks = p->ticks;
					p_proc_ready = p;
				}
			}
		}
		if (greatest_ticks <= 0) {
			for (p = proc_table-NR_TASKS; p < proc_table+NR_PROCS; p++) {
				if (p->p_flags == 0)
					p->ticks = p->priority;
			}
		}
		if (is_all_block) {
			p_proc_ready = proc_table + IDLE;
			break;
		}
	}
//dbgprtstr(p_proc_ready->p_name);
}

PRIVATE void init_clock() {
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (t_8)(TIMER_FREQ/HZ));
	out_byte(TIMER0, (t_8)((TIMER_FREQ/HZ) >> 8));

	clock_timers = NULL;
	next_timeout = TMR_NEVER;
	clock_lock = 0;
	ticks = 0;

	put_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
}

PUBLIC void clock_handler(int irq) {
//dbgprtint(current_pid);
//dbgprtint(proc_ptr->ticks);
	ticks++;
	proc_ptr->ticks--;

//dbgprtstr(proc_ptr->p_name);
	if (clock_lock != 0) {
		return;
	}
	clock_lock = 1;
//dbgprtint(next_timeout);
	if (proc_ptr->ticks <= 0 || next_timeout <= ticks) {
		schedule();
	}
	clock_lock = 0;
}

PUBLIC void set_timer(timer_t* tp, 
		clock_t exp_time, tmr_func_t watchdog) {
//dbgprtstr("&clock_timers: ");dbgprtint(&clock_timers);dbgprtstr("\n");
//dbgprtstr("clock_timers: ");dbgprtint(clock_timers);dbgprtstr("\n");
	tmrs_settimer(&clock_timers, tp, exp_time, watchdog, NULL);
	next_timeout = clock_timers->tmr_exp_time;
}

PUBLIC void reset_timer(timer_t* tp) {
	tmrs_clrtimer(&clock_timers, tp, NULL);
	next_timeout = (clock_timers == NULL) ?
			TMR_NEVER : clock_timers->tmr_exp_time;
}

PUBLIC clock_t get_uptime() {
//dbgprtstr("get_uptime: ");
//dbgprtint(ticks);
//dbgprtstr("\n");
	return ticks;
}

