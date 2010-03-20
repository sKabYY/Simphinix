#include "sconst.h"

.data
disp_pos:	.4byte	0
.text
.global	disp_str
.global disp_color_str
.global out_byte
.global in_byte
.global disable_irq
.global enable_irq

.global disp_pos

/*==========================*/
/* void disp_str(char* str) */
/*==========================*/
disp_str:
	mov		4(%esp), %esi
	mov		(disp_pos), %edi
	mov		$0xf, %ah
.disp_str_1:
	lodsb
	test	%al, %al
	jz		.disp_str_2
	cmp		$0xa, %al
	jnz		.disp_str_3
	push	%eax
	mov		%edi, %eax
	mov		$160, %bl
	div		%bl
	and		$0xff, %eax
	inc		%eax
	mov		$160, %bl
	mul		%bl
	mov		%eax, %edi
	pop		%eax
	jmp		.disp_str_1
.disp_str_3:
	mov		%ax, %gs:(%edi)
	add		$2, %edi
	jmp		.disp_str_1
.disp_str_2:
	mov		%edi, (disp_pos)
	ret

/*============================================*/
/* void disp_color_str(char* info, int color) */
/*============================================*/
disp_color_str:
	push	%ebp
	mov		%esp, %ebp

	mov		8(%ebp), %esi
	mov		12(%ebp), %ah
	mov		(disp_pos), %edi
.disp_color_str_1:
	lodsb
	test	%al, %al
	jz		.disp_color_str_2
	cmp		$0xa, %al
	jnz		.disp_color_str_3
	push	%eax
	mov		%edi, %eax
	mov		$160, %bl
	div		%bl
	and		$0xff, %eax
	inc		%eax
	mov		$160, %bl
	mul		%bl
	mov		%eax, %edi
	pop		%eax
	jmp		.disp_color_str_1
.disp_color_str_3:
	mov		%ax, %gs:(%edi)
	add		$2, %edi
	jmp		.disp_color_str_1
.disp_color_str_2:
	mov		%edi, (disp_pos)
	pop		%ebp
	ret

/*=======================================*/
/* void out_byte(t_port port, t_8 value) */
/*=======================================*/
out_byte:
	mov		4(%esp), %edx
	mov		8(%esp), %al
	out		%al, %dx
	nop
	nop
	ret

/*==========================*/
/* t_8 in_byte(t_port port) */
/*==========================*/
in_byte:
	mov		4(%esp), %edx
	xor		%eax, %eax
	in		%dx, %al
	nop
	nop
	ret

/*==========================*/
/* int disable_irq(int irq) */
/*==========================*/
disable_irq:
	mov		4(%esp), %ecx
	pushf
	cli
	mov		$1, %ah
	rol		%cl, %ah
	cmp		$8, %cl
	jae		disable_8
disable_0:
	in		$INT_M_CTLMASK, %al
	test	%ah, %al
	jnz		dis_already
	or		%ah, %al
	out		%al, $INT_M_CTLMASK
	popf
	mov		$1, %eax
	ret
disable_8:
	in		$INT_S_CTLMASK, %al
	test	%ah, %al
	jnz		dis_already
	or		%ah, %al
	out		%al, $INT_S_CTLMASK
	popf
	mov		$1, %eax
	ret
dis_already:
	popf
	xor		%eax, %eax
	ret

/*==========================*/
/* void enable_irq(int irq) */
/*==========================*/
enable_irq:
	mov		4(%esp), %ecx
	pushf
	cli
	mov		$~1, %ah
	rol		%cl, %ah
	cmp		$8, %cl
	jae		enable_8
enable_0:
	in		$INT_M_CTLMASK, %al
	and		%ah, %al
	out		%al, $INT_M_CTLMASK
	popf
	ret
enable_8:
	in		$INT_S_CTLMASK, %al
	and		%ah, %al
	out		%al, $INT_S_CTLMASK
	popf
	ret

