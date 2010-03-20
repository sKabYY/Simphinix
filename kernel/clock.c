#include "const.h"
#include "proc.h"
#include "proto.h"
#include "global.h"

PUBLIC void init_clock() {
	put_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
}

PUBLIC void clock_handler(int irq) {
	//disp_str("#");

	if (k_reenter != 0) {
		return;
	}

	++p_proc_ready;
	if (p_proc_ready >= proc_table + NR_TASKS) {
		p_proc_ready = proc_table;
	}
}

