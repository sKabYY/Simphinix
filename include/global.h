#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#ifdef _SIMPHINIX_GLOBAL_VARIABLES_HERE_
#undef EXTERN
#define EXTERN
#endif

#include <type.h>
#include <const.h>
#include <protect.h>
#include <proc.h>

EXTERN clock_t		ticks;

EXTERN int			disp_pos;

EXTERN t_8			gdt_ptr[6];
EXTERN DESCRIPTOR	gdt[GDT_SIZE];
EXTERN t_8			idt_ptr[6];
EXTERN GATE			idt[IDT_SIZE];

EXTERN TSS			tss;
EXTERN PROCESS*		p_proc_ready;

EXTERN int			k_reenter;

EXTERN PROCESS*		proc_ptr;
EXTERN int			current_pid;

extern PROCESS*		proc_table;
extern IMAGE		image[];
extern char			task_stack[];
extern irq_handler	irq_table[];

#endif

