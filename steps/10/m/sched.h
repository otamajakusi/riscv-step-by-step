#pragma once

#include <stdint.h>
#include "task.h"
#include "elfldr.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

int create_task(const Elf32_Ehdr* ehdr, union sv32_pte* pte);
task_t* get_current_task();
void schedule(uintptr_t* regs, uintptr_t mepc);
void terminate_current_task();

void start_schedule();

static inline task_t* get_current_task_safe() {
    task_t* p = get_current_task();
    ASSERT(p != NULL);
    return p;
}

#ifdef __cplusplus
}
#endif
