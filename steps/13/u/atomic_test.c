#include <stdint.h>
#include <stdio.h>
#include "atomic.h"

static int __attribute__ ((noinline))
    _lr_sc_test_1(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)addr;
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "sw      a0, 0(a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;

}

static int __attribute__ ((noinline))
    _lr_sc_test_2(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)addr;
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;
}

void atomic_test()
{
    int ret;
    uint32_t val;
    val = 0;
    ret = _lr_sc_test_1(&val, 0, 1);
    printf("ret %d, expect 0, val %lx, expect %p\n", ret, val, &val);
    val = 0;
    ret = _lr_sc_test_2(&val, 0, 1);
    printf("ret %d, expect 1, val %ld, expect 1\n", ret, val);
    val = 0;
    ret = _atomic_compare_exchange(&val, 100, 200);
    printf("ret %d, expect 0, val %ld, expect 0\n", ret, val);
    val = 100;
    ret = _atomic_compare_exchange(&val, 100, 200);
    printf("ret %d, expect 1, val %ld, expect 200\n", ret, val);
    val = 100;
    ret = _atomic_exchange(&val, 200);
    printf("ret %d, expect 100, val %ld, expect 200\n", ret, val);
    val = 300;
    ret = _atomic_exchange(&val, 400);
    printf("ret %d, expect 300, val %ld, expect 400\n", ret, val);
}
