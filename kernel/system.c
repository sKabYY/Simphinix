#include <const.h>
#include <type.h>
#include <proto.h>
#include <global.h>

PUBLIC void system_task() {
	message msg;
	while (1) {
//dbgprtstr("\nbigen recieve\n");
		sendrec(RECEIVE, ANY, &msg);
//dbgprtstr("\nreceive OK\n");
		int src = msg.source;
		switch (msg.type) {
			case GET_TICKS:
				msg.RETVAL = ticks;
				sendrec(SEND, src, &msg);
				break;
			default:
				break;
		}
	}
}

