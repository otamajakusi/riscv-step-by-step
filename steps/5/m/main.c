#include <stdio.h>
#include "femto.h"
#include "arch/riscv/trap.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"
#include "arch/riscv/csr.h"
#include "elfldr.h"

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_CLINT_TIMEBASE_FREQ  10000000

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_TIMECMP_BASE 0x4000
#define SIFIVE_TIME_BASE    0xBFF8

#define SIFIVE_TIMECMP_ADDR (CLINT_BASE + SIFIVE_TIMECMP_BASE)
#define SIFIVE_TIME_ADDR    (CLINT_BASE + SIFIVE_TIME_BASE)

extern uintptr_t u_elf_start;

static void handle_timer_interrupt()
{
    volatile uintptr_t *mtimecmp = (uintptr_t*)(SIFIVE_TIMECMP_ADDR);
    // volatile uintptr_t *mtime = (uintptr_t*)(SIFIVE_TIME_ADDR);
    uint32_t tick = SIFIVE_CLINT_TIMEBASE_FREQ;
    uint64_t next = (*(uint64_t*)mtimecmp) + tick;
    uint32_t mtimecmp_lo = next;
    uint32_t mtimecmp_hi = next >> 32;
    *(mtimecmp + 0) = -1;
    *(mtimecmp + 1) = mtimecmp_hi;
    *(mtimecmp + 0) = mtimecmp_lo;
}

static void handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    (void)regs;
    if (mcause == cause_machine_ecall) {
        printf("ecall by machine mode at: %x\n", mepc);
    } else if ((mcause & ~(1u << 31)) == intr_m_timer) {
        handle_timer_interrupt();
        return;
    } else {
        printf("unknown exception or interrupt: %x, %x, %lx\n",
                mcause, mepc, read_csr(mstatus) & MSTATUS_MPP);
    }
    exit(0);
}

/*
 * Set RWX 0..3GiB for each 1GiB-region.
 * Note: 0..4GiB by one 4GiB-region doesn't work, by mis-configuration or bug?
 */
static void setup_pmp()
{
    uint32_t base = 0u;
    uint32_t len = 0x40000000u;
    for (size_t i = 0; i < 3; i ++) {
        uint32_t pmpaddr = ((base + len * i) >> 2) | ((len >> 3) - 1);
        write_csr_enum(csr_pmpaddr0 + i, pmpaddr);
        uint32_t cfg0 = read_csr(pmpcfg0);
        write_csr(pmpcfg0, cfg0 | ((PMP_NAPOT | PMP_X | PMP_W | PMP_R) << (i * 8)));
    }
}

int main()
{
    printf("Hello RISC-V M-Mode.\n");
    set_trap_fn(handler);
    write_csr(mie, read_csr(mie) | MIP_MTIP);
    handle_timer_interrupt();
    const void* entry = load_elf((void*)&u_elf_start);
    setup_pmp();
    // jump entry with U-Mode
    write_csr(mepc, entry);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11) | MSTATUS_MPIE);
    asm volatile("fence.i");
    mret();
    return 0;
}
