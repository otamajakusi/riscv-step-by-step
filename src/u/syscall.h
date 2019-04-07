/*
 * system call number
 * 0: read
 * 1: write
 * 2: open
 * 3: close
 */
#include <stddef.h>

#define SYSCALL_READ    0
#define SYSCALL_WRITE   1
#define SYSCALL_OPEN    2
#define SYSCALL_CLOSE   3
#define SYSCALL_EXIT    32

#if !defined(__ASSEMBLER__)
#define __SYSCALL0(number)                  __syscall0((number))
#define __SYSCALL1(number, a0)              __syscall1((number), (int)(a0))
#define __SYSCALL2(number, a0, a1)          __syscall2((number), (int)(a0), (int)(a1))
#define __SYSCALL3(number, a0, a1, a2)      __syscall3((number), (int)(a0), (int)(a1), (int)(a2))

int __syscall0(int number);
int __syscall1(int number, int a0);
int __syscall2(int number, int a0, int a1);
int __syscall3(int number, int a0, int a1, int a2);

static inline int __read(int fd, void *buf, size_t count) {
    return __SYSCALL3(SYSCALL_READ, fd, buf, count);
}

static inline int __write(int fd, const void *buf, size_t count) {
    return __SYSCALL3(SYSCALL_WRITE, fd, buf, count);
}

static inline int __exit(int status) {
    return __SYSCALL1(SYSCALL_EXIT, status);
}

#endif
