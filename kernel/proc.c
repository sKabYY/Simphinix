#include "const.h"
#include "proc.h"
#include "type.h"
#include "string.h"
#include "global.h"
#include "proto.h"

PRIVATE void block(PROCESS* p);
PRIVATE void unblock(PROCESS* p);
PRIVATE int msg_send(PROCESS* current, int dest, message* m);
PRIVATE int msg_receive(PROCESS* current, int src, message* m);
PRIVATE int deadlock(int src, int dest);

PUBLIC int _sys_call(int function, int src_dest, message* msg) {
	assert(k_reenter == 0);

	int ret = 0;
	int caller = proc2pid(proc_ptr);
	message* mla = (message*)va2la(caller, msg);
	mla->source = caller;

	assert(mla->source != src_dest);

	if (function == SEND) {
//dbgprtstr("Q");
		ret = msg_send(proc_ptr, src_dest, msg);
//dbgprtstr("W");
		if (ret != 0) return ret;
	}
	else if (function == RECEIVE) {
		ret = msg_receive(proc_ptr, src_dest, msg);
		if (ret != 0) return ret;
	}
	else {
		//TODO
	}
//dbgprtstr("E");
//dbgprtstr(proc_ptr->p_name);
	return 0;
}

PUBLIC int sendrec(int function, int src_dest, message* msg) {
	int ret = 0;
	if (function == RECEIVE)
		reset_msg(msg);
	switch (function) {
		case BOTH:
			ret = _sendrec(SEND, src_dest, msg);
//dbgprtstr("\nsr: ");
//dbgprtstr(proc_ptr->p_name);dbgprtint(ret);dbgprtstr("\n");
			if (ret == 0)
				ret = _sendrec(RECEIVE, src_dest, msg);
			break;
		case SEND:
		case RECEIVE:
			ret = _sendrec(function, src_dest, msg);
			break;
		default:
			assert( (function == BOTH) || 
					(function == SEND) || 
					(function == RECEIVE) );
			break;
	}
	return ret;
}

PUBLIC int ldt_seg_linear(PROCESS* p, int idx) {
	DESCRIPTOR* d = &p->ldts[idx];
	return d->base_high << 24 | d->base_mid << 16 | d->base_low;
}

PUBLIC void* va2la(int pid, void* va) {
	PROCESS* p = &proc_table[pid];
	t_32 seg_base = ldt_seg_linear(p, INDEX_LDT_RW);
	t_32 la = seg_base + (t_32)va;
	if (pid < NR_TASKS + NR_PROCS) {
		assert(la == (t_32)va);
	}
	return (void*)la;
}

PUBLIC void reset_msg(message* p)
{
	memset(p, 0, sizeof(message));
}

PUBLIC void schedule() {
	PROCESS* p;
	int greatest_ticks = 0;
//	disp_str(" ");
	while (!greatest_ticks) {
		for (p = proc_table; p < proc_table + NR_TASKS; p++) {
//			dbgprtstr(p->p_name);
//			dbgprtint(p->p_flags);
//			dbgprtstr(" ");
			if (p->p_flags == 0) {
				if (p->ticks > greatest_ticks) {
					greatest_ticks = p->ticks;
					p_proc_ready = p;
				}
			}
		}
		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table + NR_TASKS; p++) {
				if (p->p_flags == 0)
					p->ticks = p->priority;
			}
		}
	}
	proc_ptr = p_proc_ready;
//	disp_str(proc_ptr->p_name);
}

PRIVATE void block(PROCESS* p) {
	assert(p->p_flags);
	schedule();
}

PRIVATE void unblock(PROCESS* p) {
	assert(p->p_flags == 0);
}

PRIVATE int deadlock(int src, int dest) {
	PROCESS* p = proc_table + dest;
	while (1) {
		if (p->p_flags & SENDING) {
			if (p->p_sendto == src) {
				return 1;
			}
			p = proc_table + p->p_sendto;
		}
		else {
			break;
		}
	}
	return 0;
}

PRIVATE int msg_send(PROCESS* current, int dest, message* m) {
	PROCESS* sender = current;
	PROCESS* p_dest = proc_table + dest;
//dbgprtstr("\nsend: ");
//dbgprtstr(sender->p_name);dbgprtstr("->");dbgprtstr(p_dest->p_name);
//dbgprtstr("\n");

	assert(proc2pid(sender) != dest);

	if (deadlock(proc2pid(sender), dest)) {

	}

	if ((p_dest->p_flags & RECEIVING) &&
	    (p_dest->p_recvfrom == proc2pid(sender) ||
	     p_dest->p_recvfrom == ANY)) {
		assert(p_dest->p_msg);
		assert(m);

		phys_copy(va2la(dest, p_dest->p_msg),
			  va2la(proc2pid(sender), m),
			  sizeof(message));
		p_dest->p_msg = 0;
		p_dest->p_flags &= ~RECEIVING;
		p_dest->p_recvfrom = NO_TASK;
		unblock(p_dest);

		assert(p_dest->p_flags == 0);
		assert(p_dest->p_msg == 0);
		assert(p_dest->p_recvfrom == NO_TASK);
		assert(p_dest->p_sendto == NO_TASK);
		assert(sender->p_flags == 0);
		assert(sender->p_msg == 0);
		assert(sender->p_recvfrom == NO_TASK);
		assert(sender->p_sendto == NO_TASK);
	}
	else {
		sender->p_flags |= SENDING;
		assert(sender->p_flags == SENDING);
		sender->p_sendto = dest;
		sender->p_msg = m;

		PROCESS* p;
		if (p_dest->q_sending) {
			p = p_dest->q_sending;
			while (p->next_sending)
				p = p->next_sending;
			p->next_sending = sender;
		}
		else {
			p_dest->q_sending = sender;
		}
		sender->next_sending = 0;

		block(sender);

		assert(sender->p_flags == SENDING);
		assert(sender->p_msg != 0);
		assert(sender->p_recvfrom == NO_TASK);
		assert(sender->p_sendto == dest);
	}
	return 0;
}

PRIVATE int msg_receive(PROCESS* current, int src, message* m)
{
	PROCESS* p_who_wanna_recv = current;
	PROCESS* p_from = 0;
	PROCESS* prev = 0;
	int copyok = 0;
//dbgprtstr("\nrecv: ");
//dbgprtstr(p_who_wanna_recv->p_name);dbgprtstr("\n");

	assert(proc2pid(p_who_wanna_recv) != src);

	if ((p_who_wanna_recv->has_int_msg) &&
	    ((src == ANY) || (src == INTERRUPT))) {
		message msg;
		reset_msg(&msg);
		msg.source = INTERRUPT;
		msg.type = HARD_INT;
		assert(m);
		phys_copy(va2la(proc2pid(p_who_wanna_recv), m), &msg,
			  sizeof(message));

		p_who_wanna_recv->has_int_msg = 0;

		assert(p_who_wanna_recv->p_flags == 0);
		assert(p_who_wanna_recv->p_msg == 0);
		assert(p_who_wanna_recv->p_sendto == NO_TASK);
		assert(p_who_wanna_recv->has_int_msg == 0);

		return 0;
	}

	if (src == ANY) {
		if (p_who_wanna_recv->q_sending) {
			p_from = p_who_wanna_recv->q_sending;
			copyok = 1;

			assert(p_who_wanna_recv->p_flags == 0);
			assert(p_who_wanna_recv->p_msg == 0);
			assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
			assert(p_who_wanna_recv->p_sendto == NO_TASK);
			assert(p_who_wanna_recv->q_sending != 0);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == NO_TASK);
			assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		}
	}
	else {
		p_from = &proc_table[src];

		if ((p_from->p_flags & SENDING) &&
		    (p_from->p_sendto == proc2pid(p_who_wanna_recv))) {
			copyok = 1;

			PROCESS* p = p_who_wanna_recv->q_sending;
			assert(p);
			while (p) {
				assert(p_from->p_flags & SENDING);
				if (proc2pid(p) == src) {
					p_from = p;
					break;
				}
				prev = p;
				p = p->next_sending;
			}

			assert(p_who_wanna_recv->p_flags == 0);
			assert(p_who_wanna_recv->p_msg == 0);
			assert(p_who_wanna_recv->p_recvfrom == NO_TASK);
			assert(p_who_wanna_recv->p_sendto == NO_TASK);
			assert(p_who_wanna_recv->q_sending != 0);
			assert(p_from->p_flags == SENDING);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == NO_TASK);
			assert(p_from->p_sendto == proc2pid(p_who_wanna_recv));
		}
	}

	if (copyok) {
		if (p_from == p_who_wanna_recv->q_sending) {
			assert(prev == 0);
			p_who_wanna_recv->q_sending = p_from->next_sending;
			p_from->next_sending = 0;
		}
		else {
			assert(prev);
			prev->next_sending = p_from->next_sending;
			p_from->next_sending = 0;
		}

		assert(m);
		assert(p_from->p_msg);
		phys_copy(va2la(proc2pid(p_who_wanna_recv), m),
			  va2la(proc2pid(p_from), p_from->p_msg),
			  sizeof(message));

		p_from->p_msg = 0;
		p_from->p_sendto = NO_TASK;
		p_from->p_flags &= ~SENDING;
		unblock(p_from);
	}
	else {
		p_who_wanna_recv->p_flags |= RECEIVING;

		p_who_wanna_recv->p_msg = m;

		if (src == ANY)
			p_who_wanna_recv->p_recvfrom = ANY;
		else
			p_who_wanna_recv->p_recvfrom = proc2pid(p_from);

		block(p_who_wanna_recv);

		assert(p_who_wanna_recv->p_flags == RECEIVING);
		assert(p_who_wanna_recv->p_msg != 0);
		assert(p_who_wanna_recv->p_recvfrom != NO_TASK);
		assert(p_who_wanna_recv->p_sendto == NO_TASK);
		assert(p_who_wanna_recv->has_int_msg == 0);
	}
	return 0;
}

