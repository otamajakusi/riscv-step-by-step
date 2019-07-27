#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int _atomic_compare_exchange(
        uint32_t *addr, uint32_t expected, uint32_t desired);
uint32_t _atomic_exchange(uint32_t *addr, uint32_t desired);
uint32_t _atomic_fetch_add_relaxed(uint32_t *addr, uint32_t arg);
uint32_t _atomic_load_relaxed(uint32_t *addr);

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
#if 0
    int ret = _atomic_compare_exchange(addr, expected, desired);
    return ret;
#else
    return __sync_bool_compare_and_swap(addr, expected, desired);
#endif
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
#if 0
    uint32_t ret = _atomic_exchange(addr, desired);
    return ret;
#else
    return __sync_lock_test_and_set(addr, desired);
#endif
}

/*
 * The following operations are done atomically.
 *
 * uintptr_t tmp = *addr;
 * *addr = tmp + arg;
 * return tmp;
 */
static inline uint32_t atomic_fetch_add_relaxed(uint32_t *addr, uint32_t arg)
{
    uint32_t ret = _atomic_fetch_add_relaxed(addr, arg);
    return ret;
}

static inline uint32_t atomic_load_relaxed(uint32_t *addr)
{
    uint32_t ret = _atomic_load_relaxed(addr);
    return ret;
}


#ifdef __cplusplus
}
#endif
