/* riscv-privileged-v1.10.pdf
 * 4.3.2 Virtual Address Translation Process
 *
 * Sv32
 * 1. a = satp.ppn * 4KiB;
 *    i = 1;
 * 2. pte = a + va.vpn[i] * 4;
 * 3. if pte.v == 0 || (ptr.e && pte.w) then page-fault exception
 * 4. if pte.r == 0 && ptr.x == 0;
 *    i --;
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

#define PAGE_SIZE       (1u << 12) // 4KiB
#define PAGE_NUM(addr)  (((uint32_t)(addr)) >> 12)

static union sv32_pte ptes[PAGE_SIZE / 4] __attribute__((aligned(PAGE_SIZE)));

void init_pte() {
    memset(ptes, 0, sizeof(ptes));
    ptes[0].val = (0x80400000 >> 2) | PTE_V | PTE_R | PTE_W | PTE_X | PTE_U | PTE_A;
    asm volatile ("sfence.vma" : : : "memory");
    write_csr_enum(csr_satp, SPTBR32_MODE | PAGE_NUM(&ptes[0]));
    printf("satp %x, pte %x\n", read_csr_enum(csr_satp), ptes[0].val);
}

