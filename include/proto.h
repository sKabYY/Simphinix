#ifndef _PROTO_H_
#define _PROTO_H_

#include <type.h>
#include <const.h>
#include <proc.h>

/* kliba.S */
PUBLIC void	out_byte(t_port port, t_8 value);
PUBLIC t_8	in_byte(t_port port);
PUBLIC void	disp_str(char * info);
PUBLIC void disp_int(int i);
PUBLIC void	disp_color_str(char * info, int color);
PUBLIC int	disable_irq(int irq);
PUBLIC void enable_irq(int irq);

#define dbgprtstr disp_str
#define dbgprtint disp_int

/* protect.c */
PUBLIC void	init_prot();
PUBLIC t_32 seg2phys(t_16 seg);

/* klib.c */
PUBLIC void delay(int count);
PUBLIC int get_ticks();
PUBLIC void sleep(int ms);

/* kernel.S */
PUBLIC void restart();

/* main.c */
PUBLIC void TestA();
PUBLIC void TestB();

/* system.c */
PUBLIC void system_task();

/* idle.c */
PUBLIC void idle_task();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* proc.c */
PUBLIC int ldt_seg_linear(PROCESS* p, int idx);
PUBLIC void* va2la(int pid, void* va);
PUBLIC void reset_msg(message* p);
PUBLIC void schedule();
PUBLIC int sendrec(int function, int src_dest, message* msg);
PUBLIC int notify(int dest);

/* syscall.S */
PUBLIC int _sendrec(int function, 
		int caller_nr, int src_dest, message* msg);
/* lvl0call.S */
PUBLIC void level0(int_handler handler);

/* clock.c */
PUBLIC void clock_task();
PUBLIC void clock_handler(int irq);
PUBLIC void set_timer(timer_t* tp, clock_t exp_time, tmr_func_t watchdog);
PUBLIC void reset_timer(timer_t* tp);
PUBLIC clock_t get_uptime();

/* keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_handler(int irq);

/* misc.c */
PUBLIC void assertion_failure(char* exp, char* file,
		char* base_file, int line);

/* timer.c */
PUBLIC clock_t tmrs_clrtimer(timer_t** tmrs,
		timer_t* tp, clock_t* next_time);
PUBLIC clock_t tmrs_settimer(timer_t** tmrs,
		timer_t* tp, clock_t exp_time, tmr_func_t watchdog,
		clock_t* new_head);
PUBLIC void tmrs_exptimers(timer_t** tmrs,
		clock_t now, clock_t* new_head);

#endif

