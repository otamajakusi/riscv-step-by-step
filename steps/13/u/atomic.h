#pragma once

#include <stddef.h>

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
    uintptr_t tmp = *addr;
    if (tmp == expected) {
        *addr = desired;
        return 1;
    } else {
        return 0;
    }
}

/*
 * The following operations are done atomically.
 * 
 *  uintptr_t tmp = *addr;
 *  *addr = desired;
 *  return tmp;
 */
static inline int atomic_exchange(uint32_t *addr, uint32_t desired)
{
    uint32_t tmp = *addr;
    *addr = desired;
    return tmp;
}

