.section .text,"ax",@progbits
.global __syscall
.global __syscall0
.global __syscall1
.global __syscall2
.global __syscall3
.global __syscall4

__syscall:
__syscall0:
__syscall1:
__syscall2:
__syscall3:
__syscall4:
    ecall
    ret
