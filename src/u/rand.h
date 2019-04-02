#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t xorshift32(uint32_t state[1]);
unsigned int rand();

#ifdef __cplusplus
}
#endif
