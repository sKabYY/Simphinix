#ifndef _SIMPHINIX_CONST_H_
#define _SIMPHINIX_CONST_H_

#define EXTERN extern

#define PUBLIC
#define PRIVATE static

#define GDT_SIZE	128
#define	IDT_SIZE	256

#define	PRIVILEGE_KRNL	0
#define	PRIVILEGE_TASK	1
#define	PRIVILEGE_USER	3

#define RPL_TASK	SA_RPL1

/* 8259A interrupt controller ports. */
#define INT_M_CTL     0x20 /* I/O port for interrupt controller       <Master> */
#define INT_M_CTLMASK 0x21 /* setting bits in this port disables ints <Master> */
#define INT_S_CTL     0xA0 /* I/O port for second interrupt controller<Slave>  */
#define INT_S_CTLMASK 0xA1 /* setting bits in this port disables ints <Slave>  */

#define KB_DATA		0x60

#define NR_IRQ		16

#define CLOCK_IRQ	0
#define KEYBOARD_IRQ	1

#endif

