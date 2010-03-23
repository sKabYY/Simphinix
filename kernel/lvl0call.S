#include <sconst.h>

.text

.global level0

/* void level0(void (*func)(void)) */
level0:
	mov		4(%esp), %eax
	int		$LEVEL0_VECTOR
	ret

