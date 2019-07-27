#pragma once

#include "debug.h"

#ifdef __cplusplus
extern "C" {
#endif

void mem_printf(const char *format, ...);
void mem_flush();

#ifdef __cplusplus
}
#endif
