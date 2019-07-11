#pragma once

#include <stdint.h>
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void handle_syscall(uintptr_t* regs, uintptr_t mepc, task_t* curr);

#ifdef __cplusplus
}
#endif
