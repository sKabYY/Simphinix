#define _SIMPHINIX_GLOBAL_VARIABLES_HERE_

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"

PUBLIC TASK task_table[NR_TASKS] = {
	{TestA, 10, STACK_SIZE_TESTA, "TestA"},
	{TestB, 5, STACK_SIZE_TESTB, "TestB"}
};

PUBLIC PROCESS proc_table[NR_TASKS];
PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC irq_handler irq_table[NR_IRQ];

