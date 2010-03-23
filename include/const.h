#ifndef _SIMPHINIX_CONST_H_
#define _SIMPHINIX_CONST_H_

#define EXTERN extern

#define PUBLIC
#define PRIVATE static

#define NULL (void*)0

#define GDT_SIZE	128
#define	IDT_SIZE	256

#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

#define RPL_TASK	SA_RPL1

#define SENDING		0x02
#define RECEIVING	0x04
#define UNRUNNABLE	0x08

/* system call */
#define SYSVEC		0x32

/* tasks */
#define INTERRUPT	-10
#define ANY			(NR_TASKS + NR_PROCS + 10)
#define NO_TASK		(NR_TASKS + NR_PROCS + 20)

#define CLOCK		0
#define SYSTEM		1

enum msgtype {
	HARD_INT = 1,
	GET_TICKS,
};

#define RETVAL		u.m3.m3i1

/* 8259A interrupt controller ports. */
#define INT_M_CTL     0x20 /* I/O port for interrupt controller       <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints <Master> */
#define INT_S_CTL     0xA0 /* I/O port for second interrupt controller<Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints <Slave>  */

#define TIMER0		0x40
#define TIMER_MODE	0x43
#define RATE_GENERATOR	0x34

#define TIMER_FREQ	1193182L
#define HZ			100

#define KB_DATA		0x60

#define NR_IRQ		16

#define KERNEL		-1
#define CLOCK_IRQ	0
#define KEYBOARD_IRQ	1

/* ipc */
#define SEND		1
#define RECEIVE		2
#define BOTH		3 /* BOTH = (SEND|RECEIVE) */
#define NOTIFY		4

#ifdef ASSERT
void assert_failure(char* exp, char* file, char* base_file, int line);
#define assert(exp) if (exp); \
	else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define assert(exp)
#endif

#endif

