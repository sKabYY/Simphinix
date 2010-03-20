.text
.global memcpy
.global memset
.global strcpy

/*==================================================*/
/* void* memcpy(void* p_dst, void* p_src, int size) */
/*==================================================*/
memcpy:
	push	%ebp
	mov		%esp, %ebp

	push	%esi
	push	%edi
	push	%ecx

	mov		8(%ebp), %edi
	mov		12(%ebp), %esi
	mov		16(%ebp), %ecx
.memcpy_1:
	cmp		$0, %ecx
	jz		.memcpy_2

	mov		%ds:(%esi), %al
	inc		%esi

	mov		%al, %es:(%edi)
	inc		%edi

	dec		%ecx
	jmp		.memcpy_1
.memcpy_2:
	mov		8(%ebp), %eax

	pop		%ecx
	pop		%edi
	pop		%esi
	mov		%ebp, %esp
	pop		%ebp
	ret

/*=============================================*/
/* void memset(void* p_dst, char ch, int size) */
/*=============================================*/
memset:
	push	%ebp
	mov		%esp, %ebp

	push	%esi
	push	%edi
	push	%ecx

	mov		8(%ebp), %edi
	mov		12(%ebp), %edx
	mov		16(%ebp), %ecx
.memset_1:
	cmp		$0, %ecx
	jz		.memset_2
	mov		%dl, (%edi)
	inc		%edi
	dec		%ecx
	jmp		.memset_1
.memset_2:
	pop		%ecx
	pop		%edi
	pop		%esi
	mov		%ebp, %esp
	pop		%ebp
	ret

/*========================================*/
/* char* strcpy(char* p_dst, char* p_src) */
/*========================================*/
strcpy:
	push	%ebp
	mov		%esp, %ebp
	mov		12(%ebp), %esi
	mov		8(%esp), %edi
.strcpy_1:
	mov		(%esi), %al
	inc		%esi
	mov		%al, (%edi)
	inc		%edi
	cmp		$0, %al
	jnz		.strcpy_1
	mov		8(%ebp), %eax
	pop		%ebp
	ret

