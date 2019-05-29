#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_pte();
int setup_pte(uintptr_t va, uint64_t pa);

#ifdef __cplusplus
}
#endif
