#include <const.h>
#include <type.h>
#include <proc.h>
#include <proto.h>
#include <global.h>

#include <timer.h>
PRIVATE void awake(timer_t* tp);

PUBLIC void system_task() {
	message msg;
	while (1) {
//dbgprtstr("\nbigen recieve\n");
		sendrec(RECEIVE, ANY, &msg);
//dbgprtstr("\nreceive OK\n");
		int src = msg.source;
		PROCESS* p = proc_table + src;
		switch (msg.type) {
			case GET_TICKS:
				msg.RETVAL = get_uptime();
				sendrec(SEND, src, &msg);
				break;
			case SLEEP:
//dbgprtstr(p->p_name);
				p->timer.tmr_arg.ta_int = src;
				set_timer(&p->timer, msg.EXP_TIME, awake);
				break;
			default:
				break;
		}
	}
}

PRIVATE void awake(timer_t* tp) {
//dbgprtstr("awake: ");
//dbgprtint((tp->tmr_arg).ta_int);
	notify(tp->tmr_arg.ta_int);
}

