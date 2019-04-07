#include "femto.h"
#include "syscall.h"

static int console_getchar()
{
    return -1;
}

static int console_putchar(int ch)
{
    __write(0, &ch, 1);
}

console_device_t console_user = {
    NULL,
    NULL,
    console_putchar
};

static void poweroff(int status) {
    __exit(status);
}

poweroff_device_t poweroff_user = {
	NULL,
	poweroff
};

void arch_setup()
{
    register_console(&console_user);
    register_poweroff(&poweroff_user);
}
