#include "sconst.h"

.global _sendrec

.code32
.text

/* _sendrec(int function, int src_dest, message* msg) */
_sendrec:
	mov		4(%esp), %ecx
	mov		8(%esp), %eax
	mov		12(%esp), %ebx
	int		$SYSVEC
	ret

