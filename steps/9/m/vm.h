#pragma once

#include <stdint.h>
#include "arch/riscv/pte.h"
#include "arch/riscv/csr.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"

#define PAGE_SIZE       (RISCV_PGSIZE) // 4KiB
#define PAGE_NUM(addr)  (((uintptr_t)(addr)) >> RISCV_PGSHIFT)
#define PTE_ENTRY_NUM   (PAGE_SIZE / sizeof(union sv32_pte))

#ifdef __cplusplus
extern "C" {
#endif

void set_satp(const union sv32_pte* ptes1st);
void init_pte(union sv32_pte* ptes1st, union sv32_pte* ptes2nd);
int setup_pte(union sv32_pte* ptes1st,
        uintptr_t va, uint64_t pa, size_t size,
        int read, int write, int exec);
void dump_pte(const union sv32_pte* ptes);

#ifdef __cplusplus
}
#endif
