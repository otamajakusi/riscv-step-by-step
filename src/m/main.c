#include "regs.h"
#include "task.h"
#include "sched.h"
#include "../u/syscall.h"
#include "femto.h"
#include "arch/riscv/trap.h"
#include "arch/riscv/csr.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define USER_PA     (0) // user executes in physical address
#define USER_VA     (!(USER_PA)) // user executes in virtual address

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_CLINT_TIMEBASE_FREQ  10000000 / 16

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_TIMECMP_BASE 0x4000
#define SIFIVE_TIME_BASE    0xBFF8
/*
 * Articles
 * http://embeddedsystems.io/fe310g-open-source-riscv-microcontroller-interrupt-system/
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
extern uintptr_t _binary_u_va_elf_start;
extern void* load_elf(uintptr_t dst_offset, const void *src);

static volatile int g_counter = 1;

#define USER_NUM    2

static task_t g_uctx[USER_NUM];

static void handle_timer_intr()
{
    volatile uintptr_t *mtimecmp = (uintptr_t*)(CLINT_BASE + SIFIVE_TIMECMP_BASE);
    volatile uintptr_t *mtime = (uintptr_t*)(CLINT_BASE + SIFIVE_TIME_BASE);
    // 1. mtimecmp set 0x100000 (?sec?)
    // See. riscv-privileged-v1.10.pdf 3.1.15 Machine Timer Register
    uint32_t mtimecmp_incr = SIFIVE_CLINT_TIMEBASE_FREQ;
    uint64_t mtimecmp_next = (*(uint64_t*)mtimecmp) + mtimecmp_incr;
    uint32_t mtimecmp_lo = mtimecmp_next;
    uint32_t mtimecmp_hi = mtimecmp_next >> 32;
    *mtimecmp = -1;
    *(mtimecmp + 1) = mtimecmp_hi;
    *mtimecmp = mtimecmp_lo;
    // 2. CSR.IP.MTIP clear
    write_csr_enum(csr_mip, read_csr_enum(csr_mip) | MIP_MTIP);
    //putchar('#');
    g_counter --;
}

static void trap_handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    task_t *next = NULL;
    //printf("mpp %x\n", read_csr_enum(csr_mstatus) & MSTATUS_MPP);
    //printf("mpp %x\n", read_csr_enum(csr_mstatus));
    if (mcause == cause_user_ecall) {
        switch (regs[REG_CTX_A0]) {
        case SYSCALL_READ:
            // TODO: handle task status
            break;
        case SYSCALL_WRITE: {
#if USER_PA
            char *c = (char*)regs[REG_CTX_A2];
#elif USER_VA
            const task_t* curr_task = sched_curr_task();
            char *c = (char*)(regs[REG_CTX_A2] + curr_task->paddr - curr_task->offset);
#endif
            putchar(*c);
            break;
        }
        case SYSCALL_EXIT:
            // TODO: handle task status, remove from next link
            break;
        default:
            printf("illegal syscall number %x\n", regs[1]);
            for (int i = 0; i < 16; i ++) {
              printf("[%d] %x\n", i, regs[i]);
            }
            exit(1);
        }
        write_csr_enum(csr_mepc, mepc + 4);
    } else
    if (mcause & (1u << 31) && (mcause & ~(1u << 31)) == intr_m_timer) {
        handle_timer_intr();
        sched_schedule(regs, mepc);
    } else
    if (mcause & (1u << 31) && (mcause & ~(1u << 31)) == intr_m_external) {
        /*
         * external interrupt handling
         * 1. read irq number from claim/complete register
         * 2. handle interrupt
         * 3. write irq number handled to claim/complete register
         */
         volatile uint32_t *plic_claim = (uint32_t*)0x0C200004; // PLIC Claim/Complete Register (claim)
         uint32_t irq = *plic_claim;
         printf("irq %x\n", irq);
         
         while (1) {
            int c = getchar();
            if (c == -1) {
                break;
            }
            printf("%c", c);
         }
         *plic_claim = irq;
    } else {
        const char *cause;
        if (mcause & (1u << 31)) {
            cause = riscv_intr_names[mcause & 0xf];
        } else {
            cause = riscv_excp_names[mcause & 0xf];
        }
        printf("mcause %s, mepc %08x (ra %x, mstatus %x, mtval %x, sepc %x, satp %x)\n", 
                cause, mepc, regs[1],
                read_csr_enum(csr_mstatus), read_csr_enum(csr_mtval),
                read_csr_enum(csr_sepc),
                read_csr_enum(csr_satp));
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
    // 3. CSR.IE.MTIP,MEIP set
    write_csr_enum(csr_mie, read_csr_enum(csr_mie) | MIP_MTIP | MIP_MEIP);
    // 4. CSR.MSTATUS.MIE set -> enabled by mret
    //write_csr_enum(csr_mstatus, read_csr_enum(csr_mstatus) | MSTATUS_MIE);

    memset(g_uctx, 0, sizeof(g_uctx));
    // We can use same binary with different memory layout since ABI is ILP32.
    // TODO: need more investigation.
#if USER_PA
    g_uctx[0].epc = load_elf(0x000000, (void*)&_binary_u_elf_start);
    g_uctx[1].epc = load_elf(0x100000, (void*)&_binary_u_elf_start) + 0x100000;
#elif USER_VA
    g_uctx[0].paddr = 0x80400000;
    g_uctx[1].paddr = 0x80500000;
    g_uctx[0].offset = 0x0;
    g_uctx[1].offset = 0x100000;
    g_uctx[0].epc = load_elf(g_uctx[0].paddr, (void*)&_binary_u_va_elf_start) + g_uctx[0].offset;
    g_uctx[1].epc = load_elf(g_uctx[1].paddr, (void*)&_binary_u_va_elf_start) + g_uctx[1].offset;
#endif
    for (int i = 0;i < USER_NUM; i ++) {
        g_uctx[i].next = &g_uctx[(i + 1) % USER_NUM];
        g_uctx[i].mode = PRV_U;
        g_uctx[i].status = 0;
        g_uctx[i].tag[0] = 'A' + i;
        g_uctx[i].tag[1] = '\0';
    }
    int first = 0;

    sched_init(&g_uctx[first]);

    extern void init_pte();
    init_pte();

    pmp_allow_all();
    uint32_t entry = g_uctx[first].epc;
    printf("entry %p\n", entry);

    // start the world.
    // before mret, set
    // mepc to user's entry,
    // mstatus.mpp to user mode and,
    // mstatus.mpie to enable interrupt
    write_csr(mepc, entry);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11) | (1u << 7));
    mret();

    return 0;
}
