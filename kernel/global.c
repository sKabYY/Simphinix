#define _SIMPHINIX_GLOBAL_VARIABLES_HERE_

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"

PUBLIC TASK task_table[NR_TASKS+NR_PROCS] = {
	{clock_task, STACK_SIZE_PP, "clock"},
	{task_sys, STACK_SIZE_PP, "system"},
	{TestA, STACK_SIZE_PP, "TestA"},
	{TestB, STACK_SIZE_PP, "TestB"}
};

PRIVATE PROCESS proc_images[1+NR_TASKS+NR_PROCS];
PUBLIC PROCESS* proc_table = proc_images+1;
PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC irq_handler irq_table[NR_IRQ];

