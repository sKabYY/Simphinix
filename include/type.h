#ifndef _SIMPHINIX_TYPE_H_
#define _SIMPHINIX_TYPE_H_

typedef unsigned int	t_32;
typedef unsigned short	t_16;
typedef unsigned char	t_8;
typedef unsigned int	t_port;

typedef void (*int_handler)();
typedef void (*t_pf_task)();
typedef void (*irq_handler)(int irq);

#endif

