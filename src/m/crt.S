STACK_SIZE = 1024

.section .text.init,"ax",@progbits
.global _start
_start:
    csrr    a0, mhartid
    bnez    a0, idle
    la      sp, stacks + STACK_SIZE
    j       libfemto_start_main

idle:
    wfi
    j       idle

    .bss
    .align 4
stacks:
    .skip STACK_SIZE
    
