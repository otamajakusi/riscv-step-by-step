#include "regs.h"

STACK_SIZE = 1024

.section .text.init,"ax",@progbits
.global _start
.global _idle
_start:
    # setup default trap vector
    la      t0, trap_vector
    csrw    mtvec, t0

    csrr    a0, mhartid
    bnez    a0, _idle
    la      sp, stack_bottom
    j       libfemto_start_main

_idle:
    wfi
    j       _idle

    .p2align 2
trap_vector:
    # Save registers.
    csrw    mscratch, x1 # save x1 first
    la      x1, trap_context
    sw      x2, REG_CTX_SP*4(x1)
    sw      x3, REG_CTX_GP*4(x1)
    sw      x4, REG_CTX_TP*4(x1)
    sw      x5, REG_CTX_T0*4(x1)
    sw      x6, REG_CTX_T1*4(x1)
    sw      x7, REG_CTX_T2*4(x1)
    sw      x8, REG_CTX_S0*4(x1)
    sw      x9, REG_CTX_S1*4(x1)
    sw      x10, REG_CTX_A0*4(x1)
    sw      x11, REG_CTX_A1*4(x1)
    sw      x12, REG_CTX_A2*4(x1)
    sw      x13, REG_CTX_A3*4(x1)
    sw      x14, REG_CTX_A4*4(x1)
    sw      x15, REG_CTX_A5*4(x1)
    sw      x16, REG_CTX_A6*4(x1)
    sw      x17, REG_CTX_A7*4(x1)
    sw      x18, REG_CTX_S2*4(x1)
    sw      x19, REG_CTX_S3*4(x1)
    sw      x20, REG_CTX_S4*4(x1)
    sw      x21, REG_CTX_S5*4(x1)
    sw      x22, REG_CTX_S6*4(x1)
    sw      x23, REG_CTX_S7*4(x1)
    sw      x24, REG_CTX_S8*4(x1)
    sw      x25, REG_CTX_S9*4(x1)
    sw      x26, REG_CTX_S10*4(x1)
    sw      x27, REG_CTX_S11*4(x1)
    sw      x28, REG_CTX_T3*4(x1)
    sw      x29, REG_CTX_T4*4(x1)
    sw      x30, REG_CTX_T5*4(x1)
    sw      x31, REG_CTX_T6*4(x1)
    csrr    x31, mscratch # load x1 from mscratch
    sw      x31, REG_CTX_RA*4(x1)

    # Invoke the handler.
    la      sp, trap_stack_bottom
    mv      a0, x1
    csrr    a1, mcause
    csrr    a2, mepc
    jal     trap_handler

    # Restore registers.
    la      x1, trap_context
    lw      x2, REG_CTX_SP*4(x1)
    lw      x3, REG_CTX_GP*4(x1)
    lw      x4, REG_CTX_TP*4(x1)
    lw      x5, REG_CTX_T0*4(x1)
    lw      x6, REG_CTX_T1*4(x1)
    lw      x7, REG_CTX_T2*4(x1)
    lw      x8, REG_CTX_S0*4(x1)
    lw      x9, REG_CTX_S1*4(x1)
    lw      x10, REG_CTX_A0*4(x1)
    lw      x11, REG_CTX_A1*4(x1)
    lw      x12, REG_CTX_A2*4(x1)
    lw      x13, REG_CTX_A3*4(x1)
    lw      x14, REG_CTX_A4*4(x1)
    lw      x15, REG_CTX_A5*4(x1)
    lw      x16, REG_CTX_A6*4(x1)
    lw      x17, REG_CTX_A7*4(x1)
    lw      x18, REG_CTX_S2*4(x1)
    lw      x19, REG_CTX_S3*4(x1)
    lw      x20, REG_CTX_S4*4(x1)
    lw      x21, REG_CTX_S5*4(x1)
    lw      x22, REG_CTX_S6*4(x1)
    lw      x23, REG_CTX_S7*4(x1)
    lw      x24, REG_CTX_S8*4(x1)
    lw      x25, REG_CTX_S9*4(x1)
    lw      x26, REG_CTX_S10*4(x1)
    lw      x27, REG_CTX_S11*4(x1)
    lw      x28, REG_CTX_T3*4(x1)
    lw      x29, REG_CTX_T4*4(x1)
    lw      x30, REG_CTX_T5*4(x1)
    lw      x31, REG_CTX_T6*4(x1)
    lw      x1, REG_CTX_RA*4(x1) # restore x1 last
    # Return
    mret

    .bss
    .p2align 4
stack:
    .skip STACK_SIZE
stack_bottom:
trap_context:
    .skip 4 * 32
trap_stack:
    .skip STACK_SIZE
trap_stack_bottom:
