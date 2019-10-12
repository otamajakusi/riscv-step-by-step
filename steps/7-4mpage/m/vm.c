#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "arch/riscv/pte.h"
#include "arch/riscv/csr.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"

#define PAGE_SIZE       (RISCV_PGSIZE) // 4KiB
#define PAGE_NUM(addr)  (((uintptr_t)(addr)) >> RISCV_PGSHIFT)
#define PTE_ENTRY_NUM   (PAGE_SIZE / sizeof(union sv32_pte))

static union sv32_pte ptes1st[PTE_ENTRY_NUM] __attribute__((aligned(PAGE_SIZE)));

/* setup pte for 4MiB page */
int setup_pte(uintptr_t va, uint64_t pa, size_t size,
        int read, int write, int exec)
{
    (void)size;
    uint32_t attr = PTE_V | PTE_U;
    attr |= (read ? PTE_R : 0) | (write ? PTE_W : 0) | (exec ? PTE_X : 0);
    uint32_t va_vpn0 = (va >> 12) & 0x3ff;
    uint32_t va_vpn1 = (va >> (12 + 10)) & 0x3ff;
    union sv32_pte* pte1st = &ptes1st[va_vpn1];
    if (va_vpn0 != 0) {
        printf("error: misaligned. va.vpn[0] %lx should be 0\n", va_vpn0);
        return -1;
    }
    /*
     * VA:  | VPN[1][31:22] | VPN[0][21:12] | page offset[11:0] |
     * PA:  | PPN[1][33:22] | PPN[0][21:12] | page offset[11:0] |
     * PTE: | PPN[1][31:20] | PPN[0][19:10] | attrs[9:0] |
     */
    pte1st->val = attr | ((pa >> 22/*PA.PPN[1]*/) << 20/*PTE.PPN[1]*/);
    asm volatile ("sfence.vma" : : : "memory");
    write_csr(satp, (SPTBR_MODE_SV32 << 31u) | PAGE_NUM(ptes1st));
    return 0;
}
