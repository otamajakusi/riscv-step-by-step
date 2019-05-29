#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_pte();
int setup_pte(uintptr_t va, uint64_t pa, size_t size,
        int read, int write, int exec);

#ifdef __cplusplus
}
#endif
