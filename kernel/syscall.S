#include <sconst.h>

.global _sendrec

.code32
.text

/* _sendrec(int function, int caller_nr, int src_dest, message* msg) */
_sendrec:
	mov		4(%esp), %ecx
	mov		8(%esp), %edx
	mov		12(%esp), %eax
	mov		16(%esp), %ebx
	int		$SYSVEC
	ret

