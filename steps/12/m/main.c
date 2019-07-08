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
#include "sched.h"
#include "read.h"
#include "utils.h"
#include "plic.h"

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

static void handle_timer_interrupt()
{
    volatile uintptr_t *mtimecmp = (uintptr_t*)(SIFIVE_TIMECMP_ADDR);
    // volatile uintptr_t *mtime = (uintptr_t*)(SIFIVE_TIME_ADDR);
    uint32_t tick = SIFIVE_CLINT_TIMEBASE_FREQ / 1000;
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

    task_t *curr = get_current_task_safe();
    uintptr_t addr = read_csr(mtval);

    if (mcause == cause_exec_page_fault) {
        exec = 1;
    } else if (mcause == cause_load_page_fault) {
        read = 1;
    } else if (mcause == cause_store_page_fault) {
        write = 1;
    }
    // printf("cause %d, addr %p\n", mcause, addr);

    const Elf32_Phdr* phdr = get_phdr_from_va(curr->ehdr, addr, read, write, exec);
    if (phdr == NULL) {
        printf("error: illegal page fault %d. pc %x, va %x\n", mcause, mepc, addr);
        return -1;
    }
    load_program_segment(curr->ehdr, phdr, addr, curr->pte, 1);
    return 0;
}

static void handle_intr(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    if (mcause == intr_m_timer) {
        handle_timer_interrupt();
        schedule(regs, mepc);
        return;
    } else if (mcause == intr_m_external) {
        /*
         * external interrupt handling
         * 1. read irq number from claim/complete register
         * 2. handle interrupt
         * 3. write irq number handled to claim/complete register
         */
        uint32_t irq = plic_claim();
        int c;
        while ((c = getchar()) != -1) {
            if (receive_read_data(c) < 0) {
                if (c == 3) {
                    puts("\n^C..exit");
                    exit(2);
                }
            }
        }
        plic_complete(irq);
    }
}

static void handler(uintptr_t* regs, uintptr_t mcause, uintptr_t mepc)
{
    if (mcause & (1u << 31)) {
        return handle_intr(regs, mcause & ~(1u << 31), mepc);
    }
    if (mcause == cause_machine_ecall) {
        printf("ecall by machine mode at: %x\n", mepc);
    } else if (mcause == cause_user_ecall) {
        handle_syscall(regs, mepc, get_current_task_safe());
        return;
    } else if (mcause == cause_exec_page_fault ||
               mcause == cause_load_page_fault ||
               mcause == cause_store_page_fault) {
        if (handle_page_fault(mcause, mepc) == 0) {
            return;
        } else {
            printf("unintended page fault: %x, %x, %lx\n",
                    mcause, mepc, read_csr(mstatus) & MSTATUS_MPP);
        }
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
}

static uintptr_t allocate_pa(int num_page)
{
    static uint32_t start = 0;
    uintptr_t pa = USER_PA + start * PAGE_SIZE;
    start += num_page;
    return pa;
}

static int setup_va(const Elf32_Ehdr* ehdr, union sv32_pte *ptes)
{
    const Elf32_Phdr* phdr = (const Elf32_Phdr*)(ehdr + 1);
    for (int i = 0; i < ehdr->e_phnum; i ++) {
        uint32_t va = PAGE_ALIGN(phdr[i].p_vaddr);
        uint32_t size = PAGE_ALIGN_UP(phdr[i].p_memsz);
        uint32_t num_page = PAGE_NUM(size);
        // currently, num_page == 1 is supported.
        ASSERT(num_page == 1);
        uint32_t flags = phdr[i].p_flags;
        uintptr_t pa = allocate_pa(PAGE_NUM(size));
        setup_pmp(pa, size);
        setup_pte(ptes, va, pa, size,
                flags & PF_R,
                flags & PF_W,
                flags & PF_X, 0);
    }
    return 0;

}

int main()
{
    printf("Hello RISC-V M-Mode.\n");
    set_trap_fn(handler);
    write_csr(mie, read_csr(mie) | MIP_MTIP | MIP_MEIP);
    handle_timer_interrupt();
    for (size_t i = 0; i < USER_NUM; i ++) {
        const Elf32_Ehdr* ehdr = check_elf((void*)&u_elf_start, (uintptr_t)&u_elf_size);
        if (ehdr == NULL) {
            printf("error: illegal elf\n");
            return 1;
        }
        init_pte(ptes1st[i], ptes2nd[i]);
        setup_va(ehdr, ptes1st[i]);
        if (create_task(ehdr, ptes1st[i]) < 0) {
            printf("error: create_task\n");
            return 1;
        }
    }
    asm volatile("fence.i");
    start_schedule();
    return 0;
}
