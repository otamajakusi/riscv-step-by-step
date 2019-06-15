#pragma once

#include <stdint.h>
#include "task.h"
#include "elfldr.h"

#ifdef __cplusplus
extern "C" {
#endif

int create_task(const Elf32_Ehdr* ehdr, union sv32_pte* pte);
task_t* get_current_task();
void switch_task(uintptr_t* regs, uintptr_t mepc);
void schedule();


#ifdef __cplusplus
}
#endif
