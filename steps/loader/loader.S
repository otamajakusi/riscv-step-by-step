.section .text.init,"ax",@progbits

RAM_START = 0x80000000

.global _binary_start
.global _binary_end

.global _start
_start:
    la  a0, _binary_start
    la  a1, _binary_end 
    la  a2, RAM_START
1:
    bgt     a0, a1, 1f
    lw      t0, 0(a0)
    addi    a0, a0, 4
    sw      t0, 0(a2)
    addi    a2, a2, 4
    j       1b
1:
    fence.i
    li      a0, RAM_START
    jr      a0, 0
