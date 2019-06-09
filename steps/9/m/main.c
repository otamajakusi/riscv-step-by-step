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
extern uintptr_t u_elf_size;

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
    volatile uintptr_t *mtime = (uintptr_t*)(SIFIVE_TIME_ADDR);
    uint32_t tick = SIFIVE_CLINT_TIMEBASE_FREQ / 100;
    uint64_t next = (*(uint64_t*)mtimecmp) + tick;
    uint32_t mtimecmp_lo = next;
    uint32_t mtimecmp_hi = next >> 32;
    *(mtimecmp + 0) = -1;
    *(mtimecmp + 1) = mtimecmp_hi;
    *(mtimecmp + 0) = mtimecmp_lo;
}

static int handle_page_fault(uintptr_t mcause, uintptr_t mepc)
{
    uint32_t mpp = (read_csr(mstatus) & MSTATUS_MPP) >> 11;
    if (mpp != PRV_U) {
        printf("page fault at MPP != U\n");
        return -1;
    }
    int read = 0;
    int write = 0;
    int exec = 0;

    task_t *curr = &task[curr_task_num];
    uintptr_t err_addr = mepc;

    if (mcause == cause_exec_page_fault) {
        exec = 1;
    } else if (mcause == cause_load_page_fault) {
        read = 1;
        err_addr = read_csr(mtval);
    } else if (mcause == cause_store_page_fault) {
        write = 1;
        err_addr = read_csr(mtval);
    }

    const Elf32_Phdr* phdr = get_phdr_from_va(curr->ehdr, err_addr, read, write, exec);
    if (phdr == NULL) {
        printf("error: illegal page fault %d. pc %p, va %p\n", mcause, mepc, err_addr);
        return -1;
    }
    load_program_segment(curr->ehdr, phdr, err_addr, curr->pte, 1);
    return 0;
}

static void handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    if (mcause == cause_machine_ecall) {
        printf("ecall by machine mode at: %p\n", mepc);
    } else if (mcause == cause_user_ecall) {
        handle_syscall(regs, mepc, &task[curr_task_num]);
        return;
    } else if ((mcause & ~(1u << 31)) == intr_m_timer) {
        handle_timer_interrupt();
        switch_task(regs, mepc);
        return;
    } else if (mcause == cause_exec_page_fault ||
               mcause == cause_load_page_fault ||
               mcause == cause_store_page_fault) {
        if (handle_page_fault(mcause, mepc) == 0) {
            return;
        } else {
            printf("unintended page fault: %x, %p, %x\n",
                    mcause, mepc, read_csr(mstatus) & MSTATUS_MPP);
        }
    } else {
        printf("unknown exception or interrupt: %x, %p, %x\n",
                mcause, mepc, read_csr(mstatus) & MSTATUS_MPP);
    }
    exit(0);
}

/*
 * Set RWX addr pa
 */
static void setup_pmp(uint32_t addr, uint32_t len)
{
    uint32_t pmpaddr = (addr >> 2) | ((len >> 3) - 1);
    // find pmp_off
    int csr_pmpcfg = -1;
    int csr_pmpaddr = -1;
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
}

int main()
{
    printf("Hello RISC-V M-Mode.\n");
    set_trap_fn(handler);
    write_csr(mie, read_csr(mie) | MIP_MTIP);
    handle_timer_interrupt();
    for (size_t i = 0; i < USER_NUM; i ++) {
        uint32_t pa = USER_PA + USER_PA_OFFSET * i;
        const Elf32_Ehdr* ehdr = check_elf((void*)&u_elf_start, (uintptr_t)&u_elf_size);
        if (ehdr == NULL) {
            printf("error: illegal elf\n");
            return 1;
        }
        setup_pmp(pa, 0x2000);
        init_pte(ptes1st[i], ptes2nd[i]);
        // FIXME: user va and size should be obtained from elf file.
        setup_pte(ptes1st[i], 0x0000, pa,          0x1000, 1, 0, 1, 0);
        setup_pte(ptes1st[i], 0x1000, pa + 0x1000, 0x1000, 1, 1, 0, 0);
        task[i].ehdr = ehdr;
        task[i].entry = ehdr->e_entry;
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
