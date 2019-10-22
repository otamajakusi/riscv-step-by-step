#include <stdio.h>
#include "vm.h"
#include "femto.h"
#include "arch/riscv/trap.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"
#include "arch/riscv/csr.h"
#include "elfldr.h"
#include "syscall.h"
#include "consts.h"
#include "task.h"

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_CLINT_TIMEBASE_FREQ  10000000

/* See riscv-qemu/include/hw/riscv/sifive_clint.h */
#define SIFIVE_TIMECMP_BASE 0x4000
#define SIFIVE_TIME_BASE    0xBFF8

#define SIFIVE_TIMECMP_ADDR (CLINT_BASE + SIFIVE_TIMECMP_BASE)
#define SIFIVE_TIME_ADDR    (CLINT_BASE + SIFIVE_TIME_BASE)

extern uintptr_t u_elf_start;

static union sv32_pte ptes1st[USER_NUM][PTE_ENTRY_NUM] __attribute__((aligned(PAGE_SIZE)));
static union sv32_pte ptes2nd[USER_NUM][PTE_ENTRY_NUM] __attribute__((aligned(PAGE_SIZE)));

static task_t task[USER_NUM];
static int curr_task_num;

static void switch_task(uintptr_t* regs, uintptr_t mepc)
{
    task_t *curr;

    curr = &task[curr_task_num];
    // save context
    memcpy(curr->regs, regs, sizeof(curr->regs));
    curr->mepc = mepc;

    // change curr
    curr_task_num = (curr_task_num + 1) % USER_NUM;
    curr = &task[curr_task_num];

    // restore context
    memcpy(regs, curr->regs, sizeof(curr->regs));
    write_csr(mepc, curr->mepc);

    set_satp(curr->pte);
}

static void handle_timer_interrupt()
{
    volatile uintptr_t *mtimecmp = (uintptr_t*)(SIFIVE_TIMECMP_ADDR);
    // volatile uintptr_t *mtime = (uintptr_t*)(SIFIVE_TIME_ADDR);
    uint32_t tick = SIFIVE_CLINT_TIMEBASE_FREQ / 100;
    uint64_t next = (*(uint64_t*)mtimecmp) + tick;
    uint32_t mtimecmp_lo = next;
    uint32_t mtimecmp_hi = next >> 32;
    *(mtimecmp + 0) = -1;
    *(mtimecmp + 1) = mtimecmp_hi;
    *(mtimecmp + 0) = mtimecmp_lo;
}

static void handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    if (mcause == cause_machine_ecall) {
        printf("ecall by machine mode at: %x\n", mepc);
    } else if (mcause == cause_user_ecall) {
        handle_syscall(regs, mepc, &task[curr_task_num]);
        return;
    } else if ((mcause & ~(1u << 31)) == intr_m_timer) {
        handle_timer_interrupt();
        switch_task(regs, mepc);
        return;
    } else {
        printf("unknown exception or interrupt: %x, %x, %lx\n",
                mcause, mepc, read_csr(mstatus) & MSTATUS_MPP);
    }
    exit(0);
}

/*
 * Set RWX addr pa
 */
static void setup_pmp(uint32_t addr, uint32_t len)
{
#ifndef DISABLE_PMP
    uint32_t pmpaddr = (addr >> 2) | ((len >> 3) - 1);
    // find pmp_off
    for (size_t i = 0; i < PMPCFG_COUNT; i ++) {
        uint32_t cfg = read_csr_enum(csr_pmpcfg0 + i);
        for (size_t j = 0; j < 4; j ++) {
            if (((cfg >> (j * 8)) & PMP_NAPOT) == PMP_OFF) {
                write_csr_enum(csr_pmpaddr0 + i * PMPCFG_COUNT + j, pmpaddr);
                write_csr_enum(
                        csr_pmpcfg0 + i,
                        cfg | (PMP_NAPOT | PMP_X | PMP_W | PMP_R) << (j * 8));
                return;
            }
        }
    }
    printf("error: pmp entry off not found\n");
#else
    (void)addr; (void)len;
#endif
}

int main()
{
    printf("Hello RISC-V M-Mode.\n");
    set_trap_fn(handler);
    write_csr(mie, read_csr(mie) | MIP_MTIP);
    handle_timer_interrupt();
    for (size_t i = 0; i < USER_NUM; i ++) {
        uint32_t pa = USER_PA + USER_PA_OFFSET * i;
        const void* entry = load_elf((void*)&u_elf_start, pa);
        setup_pmp(pa, 0x2000);
        init_pte(ptes1st[i], ptes2nd[i]);
        // FIXME: user va and size should be obtained from elf file.
        setup_pte(ptes1st[i], 0x0000, pa,          0x1000, 1, 0, 1);
        setup_pte(ptes1st[i], 0x1000, pa + 0x1000, 0x1000, 1, 1, 0);
        task[i].entry = (uintptr_t)entry;
        task[i].pa[0] = pa;
        task[i].pte = ptes1st[i];
    }
    // jump entry with U-Mode
    curr_task_num = 0;
    set_satp(task[curr_task_num].pte);
    write_csr(mepc, task[curr_task_num].entry);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11) | MSTATUS_MPIE);
    asm volatile("fence.i");
    mret();
    return 0;
}
