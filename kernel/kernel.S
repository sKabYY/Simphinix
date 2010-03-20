#include "sconst.h"

/* import global function */
.extern cstart
.extern exception_handler
.extern spurious_irq

/* import global variable */
.extern gdt_ptr
.extern idt_ptr
.extern tss
.extern disp_pos
.extern irq_table

.bss
.set StackLen, 2048
	.lcomm StackSpace, StackLen
.set StackTop, (StackSpace + StackLen)

.text

.global _start
.global restart

.global	divide_error
.global	single_step_exception
.global	nmi
.global	breakpoint_exception
.global	overflow
.global	bounds_check
.global	inval_opcode
.global	copr_not_available
.global	double_fault
.global	copr_seg_overrun
.global	inval_tss
.global	segment_not_present
.global	stack_exception
.global	general_protection
.global	page_fault
.global	copr_error
.global  hwint00
.global  hwint01
.global  hwint02
.global  hwint03
.global  hwint04
.global  hwint05
.global  hwint06
.global  hwint07
.global  hwint08
.global  hwint09
.global  hwint10
.global  hwint11
.global  hwint12
.global  hwint13
.global  hwint14
.global  hwint15
.global _s_call

_start:
	mov		$StackTop, %esp
	movw	$0, (disp_pos)
	sgdt	(gdt_ptr)
	call	cstart
	lgdt	(gdt_ptr)
	lidt	(idt_ptr)
	jmp		$SELECTOR_KERNEL_CS, $csinit
csinit:
	xor		%eax, %eax
	mov		$SELECTOR_TSS, %ax
	ltr		%ax
	sti
	jmp		kernel_main

.macro hwint_master i
	call	save
	in		$INT_M_CTLMASK, %al
	or		$(1 << \i), %al
	out		%al, $INT_M_CTLMASK
	mov		$EOI, %al
	out		%al, $INT_M_CTL
	sti
	push	$\i
	call	*(irq_table + 4*\i)
	add		$4, %esp
	cli
	in		$INT_M_CTLMASK, %al
	and		$~(1 << \i), %al
	out		%al, $INT_M_CTLMASK
	ret
.endm

.align 16
hwint00: hwint_master 0

.align 16
hwint01: hwint_master 1

.align 16
hwint02: hwint_master 2

.align 16
hwint03: hwint_master 3

.align 16
hwint04: hwint_master 4

.align 16
hwint05: hwint_master 5

.align 16
hwint06: hwint_master 6

.align 16
hwint07: hwint_master 7

.macro hwint_slave i
	pushl	$\i
	call	spurious_irq
	add		$4, %esp
	hlt
.endm

.align 16
hwint08: hwint_slave 8

.align 16
hwint09: hwint_slave 9

.align 16
hwint10: hwint_slave 10

.align 16
hwint11: hwint_slave 11

.align 16
hwint12: hwint_slave 12

.align 16
hwint13: hwint_slave 13

.align 16
hwint14: hwint_slave 14

.align 16
hwint15: hwint_slave 15

divide_error:
	push	$0xffffffff
	push	0
	jmp		exception
single_step_exception:
	push	$0xffffffff
	push	1
	jmp		exception
nmi:
	push	$0xffffffff
	push	$2
	jmp		exception
breakpoint_exception:
	push	$0xffffffff
	push	$3
	jmp		exception
overflow:
	push	$0xffffffff
	push	$4
	jmp		exception
bounds_check:
	push	$0xffffffff
	push	$5
	jmp		exception
inval_opcode:
	push	$0xffffffff
	push	$6
	jmp		exception
copr_not_available:
	push	$0xffffffff
	push	$7
	jmp		exception
double_fault:
	push	$8
	jmp		exception
copr_seg_overrun:
	push	$0xffffffff
	push	$9
	jmp		exception
inval_tss:
	push	$10
	jmp		exception
segment_not_present:
	push	$11
	jmp		exception
stack_exception:
	push	$12
	jmp		exception
general_protection:
	push	$13
	jmp		exception
page_fault:
	push	$14
	jmp		exception
copr_error:
	push	$0xffffffff
	push	$16
	jmp		exception

exception:
	call	exception_handler
	add		$8, %esp
	hlt

_s_call:
	call	save
	sti
	push	%esi

	push	%ebx /* msg                  */
	push	%eax /* src/dst              */
	push	%ecx /* SEND/RECEIVE/SENDREC */
	call	_sys_call
	add		$3*4, %esp

	pop		%esi
	cli
	ret

save:
	pushal
	push	%ds
	push	%es
	push	%fs
	push	%gs

	mov		%ss, %si
	mov		%si, %ds
	mov		%si, %es
	mov		%esp, %esi

	incl	(k_reenter)
	cmpl	$0, (k_reenter)
	jne		.save_1
	mov		$StackTop, %esp
	pushl	$restart
	jmp		*(RETADR-P_STACKBASE)(%esi)
.save_1:
	pushl	$restart_reenter
	jmp		*(RETADR-P_STACKBASE)(%esi)


restart:
	mov		(p_proc_ready), %esp
	lldt	P_LDT_SEL(%esp)
	lea		P_LDT_SEL(%esp), %eax
	mov		%eax, (tss+TSS3_S_SP0)
restart_reenter:
	decl	(k_reenter)
	pop		%gs
	pop		%fs
	pop		%es
	pop		%ds
	popal

	add		$4, %esp
	iretl

