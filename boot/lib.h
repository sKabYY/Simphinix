print:
	push	%si
	mov		2(%esp), %si
	push	%ax
	push	%bx
prnext:
	lodsb
	test	%al, %al
	jz		prdone
	mov		$0xe, %ah
	mov		$0x1, %bx
	int		$0x10
	jmp		prnext
prdone:
	pop		%bx
	pop		%ax
	mov		%si, 2(%esp)
	pop		%si
	ret

