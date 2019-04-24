#include <stdio.h>
#include "femto.h"
#include "arch/riscv/trap.h"

static void trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    if (mcause == cause_machine_ecall) {
        printf("ecall by machine mode at: %p\n", mepc);
    } else {
        printf("unknown exception or interrupt: %d, %p\n", mcause, mepc);
    }
    exit(0);
}

int main()
{
    printf("Hello RISC-V M-Mode.\n");
    set_trap_fn(trap_handler);
    asm volatile ("ecall");
    return 0;
}
