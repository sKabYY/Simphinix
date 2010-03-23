.extern level0

.text
.global idle_task

idle_task:
	pushl	$halt
	call	level0
	add		$4, %esp
	jmp		idle_task

halt:
	sti
	hlt
	cli
	ret

