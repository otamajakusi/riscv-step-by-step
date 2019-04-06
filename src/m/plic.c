/*
 * https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf
 * Platform-Level Interrupt Controller (PLIC)
 */

#include "plic.h"

#define PLIC_INTERRUPT_PRIORITY     (0x0C000000) // PLIC Interrupt Priority Register
#define PLIC_INTERRUPT_ENABLE_1     (0x0C002000) // PLIC Interrupt Enable Register 1
#define PLIC_INTERRUPT_THRESHOLD    (0x0C200000) // PLIC Interrupt Priority Threshold Register

void plic_enable_interrupt(int prio, int irq)
{
    volatile uint32_t *thre_reg = (uint32_t*)PLIC_INTERRUPT_THRESHOLD;
    volatile uint32_t *prio_reg = (uint32_t*)PLIC_INTERRUPT_PRIORITY;
    volatile uint32_t *ie_reg = (uint32_t*)PLIC_INTERRUPT_ENABLE_1;
    *thre_reg = 0;
    *(prio_reg + irq) = prio & 0x7;
    *ie_reg = 1u << irq;
}


