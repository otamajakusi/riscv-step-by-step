STACK_SIZE = 512

.section .text.init,"ax",@progbits
.global _start
.global _bss_end
.global _bss_start
_start:
    la      sp, stacks + STACK_SIZE

    # fill stack to inspect the useage
    la      a0, stacks
    mv      a1, sp
1:
    li      a2, 0xaaaaaaaa
    beq     a0, a1, 1f
    sw      a2, 0(a0)
    addi    a0, a0, 4
    j       1b

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
.global _stacks
_stacks:
stacks:
    .skip STACK_SIZE
    
