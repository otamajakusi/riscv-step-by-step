/* riscv-privileged-v1.10.pdf
 * 4.3.2 Virtual Address Translation Process
 *
 * Sv32 va: | vpn[1] | vpn[0] |  page offset |
 *          |31    22|21    12|11           0|
 * Sv32
 * 1. a = satp.ppn * 4KiB;
 *    i = 1;
 * 2. pte = a + va.vpn[i] * 4;
 * 3. if pte.v == 0 || (ptr.r == 0 && pte.w == 1) then page-fault exception
 * 4. if pte.r == 0 && ptr.x == 0;
 *    i --;
 *    a = pte.ppn * 4KiB
 *    goto 2. // next level page table
 * 5. check pte.r,w,x,u with SMU and MXR of mstatus
 * 6. if i > 0 && pa.ppn[0:0] != 0 then page fault (misaligned superpage)
 * 7.
 * 8. pa.pgoff = va.pgoff, 
 *    if i == 1 then pa.ppn[0:0] = va.vpn[0:0] (superpage translation)
 *    else then pa.ppn[1:0] = pte.ppn[1:0]
 * 
 * superpage(1-level page) translation) supports 4MiB pages, since va.VPN[1] is va[31:22]
 */

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
static union sv32_pte ptes2nd[PTE_ENTRY_NUM] __attribute__((aligned(PAGE_SIZE)));

void init_pte()
{
    ptes1st[0].val = PTE_V | (PAGE_NUM(ptes2nd) << PTE_PPN_SHIFT);
    for (size_t i = 1; i < PTE_ENTRY_NUM; i ++) {
        ptes1st[i].val = 0;
    }
    for (size_t i = 0; i < PTE_ENTRY_NUM; i ++) {
        ptes2nd[i].val = 0;
    }
    asm volatile ("sfence.vma" : : : "memory");
    write_csr(satp, (SPTBR_MODE_SV32 << 31u) | PAGE_NUM(ptes1st));
}

int setup_pte(uintptr_t va, uint64_t pa, size_t size,
        int read, int write, int exec)
{
    (void)size;
    uint32_t attr = PTE_V | PTE_U;
    attr |= (read ? PTE_R : 0) | (write ? PTE_W : 0) | (exec ? PTE_X : 0);
    uint32_t va_vpn0 = (va >> 12) & 0x3ff;
    uint32_t va_vpn1 = (va >> (12 + 10)) & 0x3ff;
    if (va_vpn1 != 0) {
        printf("error: va.vpn[1] %lx should be 0 for now\n", va_vpn1);
        return -1;
    }
    const union sv32_pte* pte1st = &ptes1st[va_vpn1];
    if (pte1st->pte.ppn != PAGE_NUM(ptes2nd)) {
        printf("error: illegal 1st level pte.ppn %x\n", pte1st->pte.ppn);
        return -1;
    }
    union sv32_pte* pte2nd = &ptes2nd[va_vpn0];
    if ((pte2nd->val & PTE_V) != 0) {
        printf("error: pte for va.vpn[0] %lx is in use\n", va_vpn0);
        return -1;
    }
    pte2nd->val = attr | (PAGE_NUM(pa) << PTE_PPN_SHIFT);
    asm volatile ("sfence.vma" : : : "memory");
    return 0;
}
