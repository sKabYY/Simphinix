#ifndef _SIMPHINIX_TYPE_H_
#define _SIMPHINIX_TYPE_H_

typedef unsigned long long	t_64;
typedef unsigned int		t_32;
typedef unsigned short		t_16;
typedef unsigned char		t_8;
typedef unsigned int		t_port;
typedef long				clock_t;

typedef void (*int_handler)();
typedef void (*t_pf_task)();
typedef void (*irq_handler)(int irq);

struct mess1 {
	int m1i1;
	int m1i2;
	int m1i3;
	int m1i4;
};
struct mess2 {
	void* m2p1;
	void* m2p2;
	void* m2p3;
	void* m2p4;
};
struct mess3 {
	int	m3i1;
	int	m3i2;
	int	m3i3;
	int	m3i4;
	t_64	m3l1;
	t_64	m3l2;
	void*	m3p1;
	void*	m3p2;
};
typedef struct {
	int source;
	int type;
	union {
		struct mess1 m1;
		struct mess2 m2;
		struct mess3 m3;
	} u;
} message;

#endif

