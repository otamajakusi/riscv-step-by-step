#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* load_elf(const void *src, uintptr_t pa);

#ifdef __cplusplus
}
#endif
