// See LICENSE for license details.

#include "femto.h"
#include "plic.h"

#define IRQ_UART0               (10) // qemu/include/hw/riscv/virt.h
#define UART0_INTERRUPT_PRIO    (7)

auxval_t __auxv[] = {
    { UART0_CLOCK_FREQ,         1843200    },
    { UART0_BAUD_RATE,          115200     },
    { NS16550A_UART0_CTRL_ADDR, 0x10000000 },
    { SIFIVE_TEST_CTRL_ADDR,    0x100000   },
    { 0, 0 }
};

/* see, riscv-probe/libfemto/drivers/ns16550a.c */
enum {
    /* UART Registers */
    UART_IER      = 0x01,  /* Interrupt Enable Register */
};
/* qemu/hw/char/serial.c */
#define UART_IER_RDI	0x01	/* Enable receiver data interrupt */

static void enable_uart0_interrupt()
{
    volatile uint8_t *uart = (uint8_t *)getauxval(NS16550A_UART0_CTRL_ADDR);
    uart[UART_IER] = UART_IER_RDI;
    plic_enable_interrupt(UART0_INTERRUPT_PRIO, IRQ_UART0);
}

void arch_setup()
{
    register_console(&console_ns16550a);
    enable_uart0_interrupt();
    register_poweroff(&poweroff_sifive_test);
}
