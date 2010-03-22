#include "const.h"
#include "type.h"
#include "proto.h"
#include "global.h"

PUBLIC void task_sys() {
	message msg;
int i = 5;
	while (1) {
//disp_int(i++);
		sendrec(RECEIVE, ANY, &msg);
//disp_str("R");
		int src = msg.source;
		switch (msg.type) {
			case GET_TICKS:
				msg.RETVAL = ticks;
				sendrec(SEND, src, &msg);
//disp_str("X");
				break;
			default:
				break;
		}
	}
}

