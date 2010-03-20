#ifndef _SIMPHINIX_PROTO_H_
#define _SIMPHINIX_PROTO_H_

#include "type.h"
#include "const.h"

/* kliba.S */
PUBLIC void	out_byte(t_port port, t_8 value);
PUBLIC t_8	in_byte(t_port port);
PUBLIC void	disp_str(char * info);
PUBLIC void disp_int(int i);
PUBLIC void	disp_color_str(char * info, int color);
PUBLIC int	disable_irq(int irq);
PUBLIC void enable_irq(int irq);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC t_32 seg2phys(t_16 seg);

/* klib.c */
PUBLIC void delay(int count);

/* kernel.asm */
PUBLIC void restart();

/* main.c */
PUBLIC void TestA();
PUBLIC void TestB();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* proc.c */
PUBLIC void schedule();

/* clock.c */
PUBLIC void init_clock();
PUBLIC void clock_handler(int irq);

/* keyboard.c */
PUBLIC void init_keyboard();
PUBLIC void keyboard_handler(int irq);

#endif

