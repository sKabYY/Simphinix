#include <const.h>
#include <proto.h>

PUBLIC void assertion_failure(char* exp, char* file, 
		char* base_file, int line) {
	disp_str(file);
	disp_str(":");
	disp_int(line);
	disp_str(" ");
}

