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
 *    a = pte.ppn
 *    goto 2. // next level page table
 * 5. check pte.r,w,x,u with SMU and MXR of mstatus
 * 6. if i > 0 && pa.ppn[0:0] != 0 then page fault (misaligned superpage)
 * 7.
 * 8. pa.pgoff = va.pgoff, 
 *    if i == 1 then pa.ppn[0:0] = va.vpn[0:0] (superpage translation)
 *    else then pa.ppn[1:0] = pte.ppn[1:0]
 * 
 * superpage(1-level page) translation) supports 4MiB pages, since va.VPN[1] is va[31:22]
 *
 * ex1.
 * 0x00000000(VA) -> 0x80400000(PA)
 * 0x00000000 >> 22 == 0 (va.vpn[1])
 * satp.ppn * 4KiB + 0
 * ex2.
 * 0x00000000(VA) -> 0x80400000(PA)
 * 0x00400000 >> 22 == 1 (va.vpn[1])
 * satp.ppn * 4KiB + 1
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "arch/riscv/pte.h"
#include "arch/riscv/csr.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"

#define PAGE_SIZE       (1u << 12) // 4KiB
#define PAGE_NUM(addr)  (((uintptr_t)(addr)) >> 12)

/*
 * In this implementation, a pair of ptes are used
 * for 4KiB page address translation.
 * ptes[even] is used for the 1st level and
 * ptes[odd] next to the 1st level is used for the 2nd level.
 */
static union sv32_pte ptes[PAGE_SIZE / sizeof(union sv32_pte)] __attribute__((aligned(PAGE_SIZE)));

void init_pte()
{
    memset(ptes, 0, sizeof(ptes));
    asm volatile ("sfence.vma" : : : "memory");
    write_csr(satp, (SPTBR_MODE_SV32 << 31u) | PAGE_NUM(&ptes[0]));
}

static union sv32_pte* search_empty()
{
    for (size_t i = 0; i < sizeof(ptes) / sizeof(union sv32_pte); i ++) {
        if ((ptes[i].val & PTE_V) == 0) {
            return &ptes[i];
        }
    }
    return NULL;
}

int setup_pte(uintptr_t va, uint64_t pa)
{
    union sv32_pte* pte1;
    union sv32_pte* pte2;
    pte1 = search_empty();
    pte2 = search_empty();
    printf("%p %p\n", pte1, pte2);
}
