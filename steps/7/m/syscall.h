#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void handle_syscall(uintptr_t* regs, uintptr_t mepc);

#ifdef __cplusplus
}
#endif
