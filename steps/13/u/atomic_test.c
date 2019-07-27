#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "atomic.h"

int __attribute__ ((noinline))
    _lr_sc_test_1(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "sw      a0, 0(a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;

}

int __attribute__ ((noinline))
    _lr_sc_test_2(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "sw      a2, 0(a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;

}

int __attribute__ ((noinline))
    _lr_sc_test_3(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;
}

int __attribute__ ((noinline))
    _lr_sc_test_4(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "sc.w    a0, a2, (a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;
}

int __attribute__ ((noinline))
    _lr_sc_test_5(uint32_t *addr, uint32_t expected, uint32_t desired)
{
    (void)expected;
    (void)desired;
    asm volatile (
            "lr.w    a3, (a0);"
            "lr.w    a3, (a0);"
            "sc.w    a0, a2, (a0);"
            "seqz    a0, a0;");
    return (int)addr;
}

#define EXPECT_EQ(a, b) { \
    if ((int)(a) != (int)(b)) { \
        printf("%d: \""#a"\" != \""#b"\"\n", __LINE__); \
        exit(1); \
    } else { \
        putchar('+'); \
    } \
}

void atomic_test()
{
    int ret;
    uint32_t val;

    val = 0;
    ret = _lr_sc_test_1(&val, 0, 1);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(val, &val);

    val = 0;
    ret = _lr_sc_test_2(&val, 0, 0xdeadbeef);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(val, 0xdeadbeef);

    val = 0;
    ret = _lr_sc_test_3(&val, 0, 1);
    EXPECT_EQ(ret, 1);
    EXPECT_EQ(val, 1);

    val = 0;
    ret = _lr_sc_test_4(&val, 0, 1);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(val, 1);

    val = 0;
    ret = _lr_sc_test_5(&val, 0, 1);
    EXPECT_EQ(ret, 1);
    EXPECT_EQ(val, 1);

    val = 0;
    ret = _atomic_compare_exchange(&val, 100, 200);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(val, 0);

    val = 100;
    ret = _atomic_compare_exchange(&val, 100, 200);
    EXPECT_EQ(ret, 1);
    EXPECT_EQ(val, 200);

    val = 100;
    ret = _atomic_exchange(&val, 200);
    EXPECT_EQ(ret, 100);
    EXPECT_EQ(val, 200);

    val = 300;
    ret = _atomic_exchange(&val, 400);
    EXPECT_EQ(ret, 300);
    EXPECT_EQ(val, 400);

    printf("%s: done\n", __func__);
}
