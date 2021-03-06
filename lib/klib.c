
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include <type.h>
#include <const.h>
#include <protect.h>
#include <proto.h>
#include <string.h>
#include <proc.h>
#include <global.h>


/*======================================================================*
                               itoa
 *======================================================================*/
/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
PUBLIC char * itoa(char * str, int num)
{
	char *	p = str;
	char	ch;
	int	i;
	int	flag = 0;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = 1;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}

/*======================================================================*
                               disp_int
 *======================================================================*/
PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output, input);
	disp_str(output);
}

/*======================================================================*
                                 delay
 *======================================================================*/
PUBLIC void delay(int count) {
	int i, j, k;
	for (k = 0; k < count; k++) {
		for (i = 0; i< 10; i++) {
			for (j = 0; j < 100000; j++);
		}
	}
}

/*======================================================================*
                               get_ticks
 *======================================================================*/
PUBLIC int get_ticks() {
	message msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	sendrec(BOTH, SYSTEM, &msg);
	return msg.RETVAL;
}

/*======================================================================*
                                 sleep
 *======================================================================*/
PUBLIC void sleep(int ms) {
	message msg;
	int ret;
	reset_msg(&msg);
	msg.type = SLEEP;
	msg.EXP_TIME = get_ticks() + (clock_t)(HZ*ms/1000);
//dbgprtstr("EXP: ");dbgprtint(msg.EXP_TIME);dbgprtstr("@ ");
	ret = sendrec(SEND, SYSTEM, &msg);
	assert(ret == 0);
	sendrec(RECEIVE, CLOCK, &msg);
}

