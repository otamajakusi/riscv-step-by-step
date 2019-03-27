#include "syscall.h"

# TODO: check if `t0` can be used.
.section .text,"ax",@progbits
.global __read
__read:
    li t0, SYSCALL_READ
    ecall
    
.global __write
__write:
    li t0, SYSCALL_WRITE
    ecall
