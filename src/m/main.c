#include "femto.h"
#include "arch/riscv/trap.h"
#include "arch/riscv/csr.h"
#include "arch/riscv/encoding.h"
#include <stdio.h>
#include <stdint.h>

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_TIMECMP_BASE 0x4000
#define SIFIVE_TIME_BASE    0xBFF8
/*
 * Articles
 * http://embeddedsystems.io/fe310g-open-source-riscv-microcontroller-interrupt-system/
 * http://msyksphinz.hatenablog.com/?page=1512902385
 * https://www.sifive.com/cores/s51 (ARM comparison)
 *
 * Two interrupt types: global and local.
 * CLINT: Core-Level Interruptor
 *        three basic interrupt sources:
 *        - software interrupt(SI)
 *        - timer interrupt(TI)
 *        - external interrupt(EI) (<- All global interrupts form PLIC are applied)
 * PLIC: Platform-Level Interrupt Controller
 */

extern uintptr_t _binary_u_elf_start;
extern int load_elf(void *dst, const void *src);

static volatile int g_counter = 1;

static void handle_timer_intr()
{
    volatile uintptr_t *mtimecmp = (uintptr_t*)(CLINT_BASE + SIFIVE_TIMECMP_BASE);
    volatile uintptr_t *mtime = (uintptr_t*)(CLINT_BASE + SIFIVE_TIME_BASE);
    // 1. mtimecmp set 0x100000 (?sec?)
    // See. riscv-privileged-v1.10.pdf 3.1.15 Machine Timer Register
    uint32_t mtimecmp_incr = 0x10000;
    uint64_t mtimecmp_next = (*(uint64_t*)mtimecmp) + mtimecmp_incr;
    uint32_t mtimecmp_lo = mtimecmp_next;
    uint32_t mtimecmp_hi = mtimecmp_next >> 32;
    *mtimecmp = -1;
    *(mtimecmp + 1) = mtimecmp_hi;
    *mtimecmp = mtimecmp_lo;
    // 2. CSR.IP.MTIP clear
    write_csr_enum(csr_mip, read_csr_enum(csr_mip) | MIP_MTIP);
    //printf("(%2d)%08x\n", g_counter, *mtime);
    putchar('#');
    g_counter --;
}

static void trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    //printf("mpp %x\n", read_csr_enum(csr_mstatus) & MSTATUS_MPP);
    //printf("mpp %x\n", read_csr_enum(csr_mstatus));
    if (mcause == cause_user_ecall) {
        if (regs[1] != 1) { // FIXME: currently write syscall only
            printf("illegal syscall number %d\n", regs[1]);
            for (int i = 0; i < 16; i ++) {
              printf("[%d] %x\n", i, regs[i]);
            }
            exit(1);
        }
        char *c = (char*)regs[3];
        //printf("c %p (%x, %c(%x))\n", c, regs[3], regs[4], regs[4]);
        putchar(*c);
        write_csr_enum(csr_mepc, mepc + 4);
    } else
    if (mcause & (1u << 31) && (mcause & ~(1u << 31)) == intr_m_timer) {
        handle_timer_intr();
    } else {
        const char *cause;
        if (mcause & (1u << 31)) {
            cause = riscv_intr_names[mcause & 0xf];
        } else {
            cause = riscv_excp_names[mcause & 0xf];
        }
        printf("mcause %s, mepc %08x\n", cause, mepc);
        if (mcause == cause_machine_ecall) {
            write_csr_enum(csr_mepc, mepc + 4);
        } else {
            exit(1);
        }
    }
}


int main() {
    printf("hello risc-v (M mode)\n");
    set_trap_fn(trap_handler);
    asm volatile ("ecall");
    printf("returned from trap\n");

    // time
    volatile uintptr_t *mtimecmp = (uintptr_t*)(CLINT_BASE + SIFIVE_TIMECMP_BASE);
    volatile uintptr_t *mtime = (uintptr_t*)(CLINT_BASE + SIFIVE_TIME_BASE);
    printf("mtimecmp %08x, mtime %08x %d\n", mtimecmp, mtime, sizeof(uintptr_t));
    printf("mtimecmp %08x_%08x\n", *(mtimecmp + 1), *mtimecmp);
    printf("mtime %08x_%08x\n", *(mtime + 1), *mtime);
    printf("mie %08x, mip %08x\n", read_csr_enum(csr_mie), read_csr_enum(csr_mip));

    handle_timer_intr();
    // 3. CSR.IE.MTIP set
    write_csr_enum(csr_mie, read_csr_enum(csr_mie) | MIP_MTIP);
    // 4. CSR.MSTATUS.MTIP set
    write_csr_enum(csr_mstatus, read_csr_enum(csr_mstatus) | MSTATUS_MIE);

//    while (g_counter)
//        ;
//    printf("finished\n");

    load_elf(0, (void*)&_binary_u_elf_start);

    return 0;
}
