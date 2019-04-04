// See LICENSE for license details.

#include "femto.h"

auxval_t __auxv[] = {
    { UART0_CLOCK_FREQ,         32000000   },
    { UART0_BAUD_RATE,          115200     },
    { SIFIVE_UART0_CTRL_ADDR,   0x10013000 },
    { SIFIVE_TEST_CTRL_ADDR,    0x100000   },
    { 0, 0 }
};

enum {
    UART_REG_IE     = 4,
};

static void enable_uart_interrupt()
{
    uint32_t IRQ_UART0 = 3;
    // UART0 IE
	volatile int *uart = (int *)(void *)getauxval(SIFIVE_UART0_CTRL_ADDR);
    uart[UART_REG_IE] = 1 | 2; /* txwm | rxwm */
    // https://sifive.cdn.prismic.io/sifive%2F4d063bf8-3ae6-4db6-9843-ee9076ebadf7_fe310-g000.pdf
    // Platform-Level Interrupt Controller (PLIC)
    // UART0: IRQ3
    
    volatile uint32_t *plic_prio = (uint32_t*)0x0C000000; // PLIC Interrupt Priority Register
    volatile uint32_t *plic_ie = (uint32_t*)0x0C002000; // PLIC Interrupt Enable Register 1
    volatile uint32_t *plic_pend = (uint32_t*)0x0C200000; // PLIC Interrupt Priority Threshold Register
    *(plic_prio + IRQ_UART0) = 1; // lowest
    *plic_ie = 1u << IRQ_UART0;
    *plic_pend = 0;
}

void arch_setup()
{
    register_console(&console_sifive_uart);
    enable_uart_interrupt();
    register_poweroff(&poweroff_sifive_test);
}
