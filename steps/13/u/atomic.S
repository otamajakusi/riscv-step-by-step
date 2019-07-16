.global _atomic_compare_exchange
.global _atomic_exchange
.global _atomic_fetch_add_relaxed
.global _atomic_load_relaxed

/*
uint32_t atomic_compare_exchange(
        uint32_t *addr, uint32_t expected, uint32_t desired)
*/
_atomic_compare_exchange:
    lr.w    a3, (a0)
    bne     a3, a1, 1f
    sc.w    a0, a2, (a0)
    seqz    a0, a0
    j 2f
1:
    li      a0, 0 # return false
2:
    ret

/*
uint32_t atomic_exchange(uint32_t *addr, uint32_t desired)
*/
_atomic_exchange:
1:
    lr.w    a3, (a0)
    sc.w    a2, a1, (a0)
    bne     a2, zero, 1b
    mv      a0, a3
    ret

/*
uint32_t atomic_fetch_add(uint32_t *addr, uint32_t arg)
*/
_atomic_fetch_add_relaxed:
    amoadd.w a0, a1, (a0)
    ret

/*
 */
_atomic_load_relaxed:
    lw a0, 0(a0)
    ret
