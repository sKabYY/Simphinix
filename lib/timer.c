#include <const.h>
#include <timer.h>

/*====================================*
 * tmrs_clrtimer(tmrs, tp, next_time) *
 *====================================*/
PUBLIC clock_t tmrs_clrtimer(timer_t** tmrs, 
		timer_t *tp, clock_t* next_time) {
	timer_t** atp;
	clock_t prev_time;

	if (*tmrs)
		prev_time = (*tmrs)->tmr_exp_time;
	else
		prev_time = 0;

	tp->tmr_exp_time = TMR_NEVER;

	for (atp = tmrs; *atp != NULL; atp = &(*atp)->tmr_next) {
		if (*atp == tp) {
			*atp = tp->tmr_next;
			break;
		}
	}

	if (next_time) {
		if (*tmrs)
			*next_time = (*tmrs)->tmr_exp_time;
		else
			*next_time = 0;
	}

	return prev_time;
}

/*=======================================================*
 * tmrs_settimer(tmrs, tp, exp_time, watchdog, new_head) *
 *=======================================================*/
PUBLIC clock_t tmrs_settimer(timer_t** tmrs,
		timer_t* tp, clock_t exp_time, tmr_func_t watchdog,
		clock_t* new_head) {
	timer_t** atp;
	clock_t old_head = 0;
	if (*tmrs)
		old_head = (*tmrs)->tmr_exp_time;
	tmrs_clrtimer(tmrs, tp, NULL);
	tp->tmr_exp_time = exp_time;
	tp->tmr_func = watchdog;

	for (atp = tmrs; *atp != NULL; atp = &(*atp)->tmr_next) {
		if (exp_time < (*atp)->tmr_exp_time) break;
	}
	tp->tmr_next = *atp;
	*atp = tp;
	if (new_head)
		*new_head = (*tmrs)->tmr_exp_time;
	return old_head;
}

/*=====================================*
 * tmrs_exptimers(tmrs, now, new_head) *
 *=====================================*/
PUBLIC void tmrs_exptimers(timer_t** tmrs, 
		clock_t now, clock_t* new_head) {
	timer_t* tp;
	while ((tp = *tmrs) != NULL && tp->tmr_exp_time <= now) {
		*tmrs = tp->tmr_next;
		tp->tmr_exp_time = TMR_NEVER;
		(*tp->tmr_func)(tp);
	}
	if (new_head) {
		if (*tmrs)
			*new_head = (*tmrs)->tmr_exp_time;
		else
			*new_head = 0;
	}
}

