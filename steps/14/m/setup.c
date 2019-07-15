// See LICENSE for license details.

#include "femto.h"
#include "plic.h"

#define IRQ_UART0               (3)
#define UART0_INTERRUPT_PRIO    (7)

auxval_t __auxv[] = {
    { UART0_CLOCK_FREQ,         32000000   },
    { UART0_BAUD_RATE,          115200     },
    { SIFIVE_UART0_CTRL_ADDR,   0x10013000 },
    { SIFIVE_TEST_CTRL_ADDR,    0x100000   },
    { 0, 0 }
};

/* see, riscv-probe/libfemto/drivers/sifive_uart.c */
enum {
    /* UART Registers */
    UART_REG_IE     = 4,

    /* IP register */
    UART_IP_TXWM    = 1,
    UART_IP_RXWM    = 2
};

static void enable_uart0_interrupt()
{
	volatile int *uart = (int *)(void *)getauxval(SIFIVE_UART0_CTRL_ADDR);
    uart[UART_REG_IE] = UART_IP_RXWM; /* only rxwm is fine */
    plic_enable_interrupt(UART0_INTERRUPT_PRIO, IRQ_UART0);
}

void arch_setup()
{
    register_console(&console_sifive_uart);
    enable_uart0_interrupt();
    register_poweroff(&poweroff_sifive_test);
}
