#pragma once

#include <stdint.h>
#include "regs.h"
#include "vm.h"
#include "elfldr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    task_state_created = 0,
    task_state_ready,
    task_state_running,
    task_state_blocking,
    task_state_terminated,
} task_state_t;

typedef struct {
    uintptr_t       entry;
    const Elf32_Ehdr*     ehdr;
    union sv32_pte* pte;
    uintptr_t       regs[REG_CTX_NUM];
    uintptr_t       mepc;
    task_state_t    state;
} task_t;

#ifdef __cplusplus
}
#endif
