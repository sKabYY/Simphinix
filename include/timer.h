#ifndef _TIMER_H_
#define _TIMER_H_

#include <limit.h>
#include <type.h>

struct timer;
typedef void (*tmr_func_t)(struct timer* tp);
typedef union {
	int ta_int;
	long ta_long;
	void* ta_ptr;
} tmr_arg_t;

typedef struct timer {
	struct timer* tmr_next;
	clock_t tmr_exp_time;
	tmr_func_t tmr_func;
	tmr_arg_t tmr_arg;
} timer_t;

#define TMR_NEVER ((clock_t) LONG_MAX)

#define tmr_arg(tp) (&(tp)->tmr_arg)
#define tmr_exp_time(tp) (&(tp)->tmr_exp_time)

#define tmr_inittimer(tp) (void)((tp)->tmr_exp_time = TMR_NEVER, \
	(tp)->tmr_next = NULL)

#endif

