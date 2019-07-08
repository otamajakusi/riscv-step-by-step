#pragma once

#include <stdint.h>
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void handle_futex(uintptr_t* regs, task_t* curr);

#ifdef __cplusplus
}
#endif
