#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#include "elf.h"
#pragma GCC diagnostic pop

#include "vm.h"

#ifdef __cplusplus
extern "C" {
#endif

void* load_elf(const void *elf, size_t size, uintptr_t pa);

const Elf32_Ehdr* check_elf(const void *elf, size_t size);

const Elf32_Phdr* get_phdr_from_va(
        const Elf32_Ehdr* ehdr, uint32_t va, int read, int write, int exec);

int load_program_segment(
        const Elf32_Ehdr* ehdr, const Elf32_Phdr* phdr,
        uint32_t va, const union sv32_pte* ptes1st);

#ifdef __cplusplus
}
#endif
