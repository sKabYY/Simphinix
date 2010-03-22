#ifndef _SIMPHINIX_PROTO_H_
#define _SIMPHINIX_PROTO_H_

#include "type.h"
#include "const.h"
#include "proc.h"

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

/* kernel.S */
PUBLIC void restart();

/* main.c */
PUBLIC void TestA();
PUBLIC void TestB();
PUBLIC int get_ticks();

/* systask.c */
PUBLIC void task_sys();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* proc.c */
PUBLIC int ldt_seg_linear(PROCESS* p, int idx);
PUBLIC void* va2la(int pid, void* va);
PUBLIC void reset_msg(message* p);
PUBLIC void schedule();
PUBLIC int sendrec(int function, int src_dest, message* msg);

/* syscall.S */
PUBLIC int _sendrec(int function, int src_dest, message* msg);

/* clock.c */
PUBLIC void init_clock();
PUBLIC void clock_handler(int irq);

/* keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_handler(int irq);

/* misc.c */
PUBLIC void assertion_failure(char* exp, char* file,
		char* base_file, int line);

#endif

