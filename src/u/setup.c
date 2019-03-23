#include "femto.h"

static int console_getchar()
{
    return -1;
}

static int console_putchar(int ch)
{
    asm volatile ("ecall");
}

console_device_t console_user = {
    NULL,
    NULL,
    console_putchar
};

void arch_setup()
{
    register_console(&console_user);
}
