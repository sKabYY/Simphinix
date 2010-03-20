#include "const.h"
#include "proto.h"
#include "type.h"

void init_keyboard() {
	put_irq_handler(KEYBOARD_IRQ, keyboard_handler);
	enable_irq(KEYBOARD_IRQ);
}

PUBLIC void keyboard_handler(int irq) {
	t_8 scan_code = in_byte(KB_DATA);
	disp_int(scan_code);
}

