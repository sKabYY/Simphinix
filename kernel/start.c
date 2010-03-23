#include <type.h>
#include <const.h>
#include <protect.h>
#include <proto.h>
#include <string.h>
#include <proc.h>
#include <global.h>

PUBLIC void cstart() {
	disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
			"-----\"cstart\" begins-----\n");
	memcpy(&gdt, (void*)(*((t_32*)(&gdt_ptr[2]))),
			*((t_16*)(&gdt_ptr[0])) + 1);
	t_16* p_gdt_limit = (t_16*)(&gdt_ptr[0]);
	t_32* p_gdt_base = (t_32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base = (t_32)&gdt;

	t_16* p_idt_limit = (t_16*)(&idt_ptr[0]);
	t_32* p_idt_base = (t_32*)(&idt_ptr[2]);
	*p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
	*p_idt_base = (t_32)&idt;

	init_prot();

	disp_str("-----\"cstart\" ends-----\n");
}

