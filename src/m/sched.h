#pragma once

#include <stdint.h>
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void sched_init(task_t* task);
task_t* sched_curr_task();
void sched_schedule(uintptr_t* regs, uintptr_t epc);

#ifdef __cplusplus
}
#endif
