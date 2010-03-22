#ifndef _SIMPHINIX_STRING_H_
#define _SIMPHINIX_STRING_H_

#include "const.h"

#define phys_copy memcpy

PUBLIC void* memcpy(void* p_dst, void* p_src, int size);
PUBLIC void memset(void* p_dst, char ch, int size);
PUBLIC char* strcpy(char* p_dst, char* p_src);

#endif

