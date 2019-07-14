#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int _atomic_compare_exchange(
        uint32_t *addr, uint32_t expected, uint32_t desired);
uint32_t _atomic_exchange(uint32_t *addr, uint32_t desired);

/* 
 * The following operations are done atomically.
 *
 * uintptr_t tmp = *addr;
 * if (tmp == expected) {
 *     *addr = desired;
 *     return 1;
 * } else {
 *     return 0;
 * }
 */
static inline int atomic_compare_exchange(
        uint32_t *addr, uint32_t expected, uint32_t desired)
{
    int ret = _atomic_compare_exchange(addr, expected, desired);
    return ret;
}

/*
 * The following operations are done atomically.
 * 
 *  uintptr_t tmp = *addr;
 *  *addr = desired;
 *  return tmp;
 */
static inline uint32_t atomic_exchange(uint32_t *addr, uint32_t desired)
{
    uint32_t ret = _atomic_exchange(addr, desired);
    return ret;
}

#ifdef __cplusplus
}
#endif
