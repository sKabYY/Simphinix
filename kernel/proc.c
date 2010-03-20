#include "const.h"
#include "proc.h"
#include "type.h"
#include "global.h"
#include "proto.h"

PUBLIC int _sys_call(int call_nr, int src_dst, message* m_ptr) {
	disp_str("_sys_call");
	return 0;
}

PUBLIC void schedule() {
	PROCESS* p;
	int greatest_ticks = 0;
	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table + NR_TASKS; p++) {
			if (p->ticks > greatest_ticks) {
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}
		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table + NR_TASKS; p++) {
				p->ticks = p->priority;
			}
		}
	}
}

