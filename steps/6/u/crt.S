STACK_SIZE = 1024

.section .text.init,"ax",@progbits
.global _start
_start:
    la      sp, stacks + STACK_SIZE
    j       libfemto_start_main

    .bss
    .p2align 2
stacks:
    .skip STACK_SIZE
    
