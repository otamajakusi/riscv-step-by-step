STACK_SIZE = 1024

.section .text.init,"ax",@progbits
.global _start
.global _bss_end
.global _bss_start
_start:
    la      sp, stacks + STACK_SIZE
    # clear .bss
    la      a0, _bss_start
    la      a1, _bss_end
1:
    beq     a0, a1, 1f
    sw      zero, 0(a0)
    addi    a0, a0, 4
    j       1b

1:
    jal     arch_setup
    jal     main
    j       exit

    .bss
    .p2align 2
stacks:
    .skip STACK_SIZE
    
