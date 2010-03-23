#define _SIMPHINIX_GLOBAL_VARIABLES_HERE_

#include <type.h>
#include <const.h>
#include <protect.h>
#include <proto.h>
#include <proc.h>
#include <global.h>

PUBLIC IMAGE image[NR_TASKS+NR_PROCS] = {
	{IDLE, idle_task, IDL_S, "IDLE"},
	{CLOCK, clock_task, TSK_S, "CLOCK"},
	{SYSTEM, system_task, TSK_S, "SYSTEM"},
	{KERNEL, NULL, HRD_S, "KERNEL"},
	{0, TestA, TSK_S, "TestA"},
	{1, TestB, TSK_S, "TestB"},
};

PRIVATE PROCESS procs[NR_TASKS+NR_PROCS];
PUBLIC PROCESS* proc_table = procs+NR_TASKS;
PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC irq_handler irq_table[NR_IRQ];

