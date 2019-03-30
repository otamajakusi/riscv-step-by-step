STACK_SIZE = 1024

.section .text.init,"ax",@progbits
.global _start
_start:
    # setup default trap vector
    la      t0, trap_vector
    csrw    mtvec, t0

    csrr    a0, mhartid
    bnez    a0, idle
    la      sp, stack_bottom
    j       libfemto_start_main

idle:
    wfi
    j       idle

    .align 4
trap_vector:
    # https://riscv.org/wp-content/uploads/2015/01/riscv-calling.pdf
    # x0     zero Hard-wired zero
    # x1     ra Return address Caller
    # x2     sp Stack pointer Callee
    # x3     gp Global pointer
    # x4     tp Thread pointer
    # x5-7   t0-2 Temporaries Caller
    # x8     s0/fp Saved register/frame pointer Callee
    # x9     s1 Saved register Callee
    # x10-11 a0-1 Function arguments/return values Caller
    # x12-17 a2-7 Function arguments Caller
    # x18-27 s2-11 Saved registers Callee
    # x28-31 t3-6 Temporaries Caller
    # Save registers.
    csrw    mscratch, x1 # save x1 first
    la      x1, trap_context
    sw      x2, 2*4(x1)
    sw      x3, 3*4(x1)
    sw      x4, 4*4(x1)
    sw      x5, 5*4(x1)
    sw      x6, 6*4(x1)
    sw      x7, 7*4(x1)
    sw      x8, 8*4(x1)
    sw      x9, 9*4(x1)
    sw      x10, 10*4(x1)
    sw      x11, 11*4(x1)
    sw      x12, 12*4(x1)
    sw      x13, 13*4(x1)
    sw      x14, 14*4(x1)
    sw      x15, 15*4(x1)
    sw      x16, 16*4(x1)
    sw      x17, 17*4(x1)
    sw      x18, 18*4(x1)
    sw      x19, 19*4(x1)
    sw      x10, 20*4(x1)
    sw      x21, 21*4(x1)
    sw      x22, 22*4(x1)
    sw      x23, 23*4(x1)
    sw      x24, 24*4(x1)
    sw      x25, 25*4(x1)
    sw      x26, 26*4(x1)
    sw      x27, 27*4(x1)
    sw      x28, 28*4(x1)
    sw      x29, 29*4(x1)
    sw      x30, 30*4(x1)
    sw      x31, 31*4(x1)
    csrr    x31, mscratch # load x1 from mscratch
    sw      x31, 1*4(x1)

    # Invoke the handler.
    la      sp, trap_stack_bottom
    mv      a0, x1
    csrr    a1, mcause
    csrr    a2, mepc
    jal     trap_handler

    # Restore registers.
    la      x1, trap_context
    lw      x2, 2*4(x1)
    lw      x3, 3*4(x1)
    lw      x4, 4*4(x1)
    lw      x5, 5*4(x1)
    lw      x6, 6*4(x1)
    lw      x7, 7*4(x1)
    lw      x8, 8*4(x1)
    lw      x9, 9*4(x1)
    lw      x10, 10*4(x1)
    lw      x11, 11*4(x1)
    lw      x12, 12*4(x1)
    lw      x13, 13*4(x1)
    lw      x14, 14*4(x1)
    lw      x15, 15*4(x1)
    lw      x16, 16*4(x1)
    lw      x17, 17*4(x1)
    lw      x18, 18*4(x1)
    lw      x19, 19*4(x1)
    lw      x10, 20*4(x1)
    lw      x21, 21*4(x1)
    lw      x22, 22*4(x1)
    lw      x23, 23*4(x1)
    lw      x24, 24*4(x1)
    lw      x25, 25*4(x1)
    lw      x26, 26*4(x1)
    lw      x27, 27*4(x1)
    lw      x28, 28*4(x1)
    lw      x29, 29*4(x1)
    lw      x30, 30*4(x1)
    lw      x31, 31*4(x1)
    lw      x1, 1*4(x1) # restore x1 last
    # Return
    mret

    .bss
    .align 4
stack:
    .skip STACK_SIZE
stack_bottom:
trap_context:
    .skip 4 * 32
trap_stack:
    .skip STACK_SIZE
trap_stack_bottom:
