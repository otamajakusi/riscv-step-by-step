/*
 * system call number
 * 0: read
 * 1: write
 * 2: open
 * 3: close
 */
#define SYSCALL_READ    0
#define SYSCALL_WRITE   1
#define SYSCALL_OPEN    2
#define SYSCALL_CLOSE   3

#if !defined(__ASSEMBLER__)
int __read(int fd, void *buf, size_t count);
int __write(int fd, const void *buf, size_t count);
#endif
