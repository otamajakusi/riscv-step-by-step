#pragma once
/*
 * syscall number
 * 0: read
 * 1: write
 */
#include <stddef.h>

#define SYSCALL_READ    0
#define SYSCALL_WRITE   1
#define SYSCALL_EXIT    32
#define SYSCALL_CLONE   33
#define SYSCALL_WAITPID 34
#define SYSCALL_FUTEX   35

#define FUTEX_WAIT      0
#define FUTEX_WAKE      1
#define FUTEX_WAIT_EXP  0x80 // for experimental
#define FUTEX_WAKE_EXP  0x81 // for experimental

// error
#define EAGAIN          11
#define EFAULT          14
#define EINVAL          22
#define ENOSYS          38

#if !defined(__ASSEMBLER__)
#define __SYSCALL0(number)                  __syscall0((number))
#define __SYSCALL1(number, a0)              __syscall1((number), (int)(a0))
#define __SYSCALL2(number, a0, a1)          __syscall2((number), (int)(a0), (int)(a1))
#define __SYSCALL3(number, a0, a1, a2)      __syscall3((number), (int)(a0), (int)(a1), (int)(a2))
#define __SYSCALL4(number, a0, a1, a2, a3)  __syscall4((number), (int)(a0), (int)(a1), (int)(a2), (int)(a3))

int __syscall0(int number);
int __syscall1(int number, int a0);
int __syscall2(int number, int a0, int a1);
int __syscall3(int number, int a0, int a1, int a2);
int __syscall4(int number, int a0, int a1, int a2, int a3);

static inline int __read(int fd, void *buf, size_t count) {
    return __SYSCALL3(SYSCALL_READ, fd, buf, count);
}

static inline int __write(int fd, const void *buf, size_t count) {
    return __SYSCALL3(SYSCALL_WRITE, fd, buf, count);
}

static inline void __attribute__((noreturn)) __exit(int status) {
    __SYSCALL1(SYSCALL_EXIT, status);
    while (1);
}

/* Note: Compare to linux syscall, we don't have flags param,
 * and type of fn is different. */
static inline int __clone(void *(*fn)(void*), void* stack, void *arg) {
    return __SYSCALL3(SYSCALL_CLONE, fn, stack, arg);
}

/* Note: Compare to linux syscall, we don't have options param. */
static inline int __waitpid(int pid, int *wstatus) {
    return __SYSCALL2(SYSCALL_WAITPID, pid, wstatus);
}

/* Note: No timeout and val3 for this environment. */
/* 
 * FUTEX_WAIT_EXP and FUTEX_WAKE_EXP are experimental operations.
 * FUTEX_WAIT_EXP:
 *   compares if *uaddr is equal to val then store val2 to *uaddr and return 0,
 *   otherwise sleeps waiting for a FUTEX_WAKE_EXP and returns -EAGAIN.
 * FUTEX_WAKE_EXP:
 *   store val2 to *uaddr and wakes at most val of the waiters waiting on the
 *   uaddr.
 *
 * FUTEX_WAIT:
 *   compares if *uaddr is equal to val then sleeps waiting for a FUTEX_WAKE.
 *   otherwise returns -EAGAIN.
 * FUTEX_WAKE:
 *   wakes at most val of the waiters waiting on the uaddr.
 */
static inline int __futex(int *uaddr, int futex_op, int val, int val2) {
    return __SYSCALL4(SYSCALL_FUTEX, uaddr, futex_op, val, val2);
}

#endif
