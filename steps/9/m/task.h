#pragma once

#include <stdint.h>
#include "regs.h"
#include "vm.h"
#include "elfldr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uintptr_t       entry;
    const Elf32_Ehdr*     ehdr;
    union sv32_pte* pte;
    uintptr_t       regs[REG_CTX_NUM];
    uintptr_t       mepc;
} task_t;

#ifdef __cplusplus
}
#endif
