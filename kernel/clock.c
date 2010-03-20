#include "const.h"
#include "proc.h"
#include "proto.h"
#include "global.h"

PUBLIC void init_clock() {
	out_byte(TIMER_MODE, RATE_GENERATOR);
	out_byte(TIMER0, (t_8)(TIMER_FREQ/HZ));
	out_byte(TIMER0, (t_8)((TIMER_FREQ/HZ) >> 8));

	put_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
}

PUBLIC void clock_handler(int irq) {
	ticks++;
	p_proc_ready->ticks--;

	if (k_reenter != 0) {
		return;
	}

	schedule();
}

