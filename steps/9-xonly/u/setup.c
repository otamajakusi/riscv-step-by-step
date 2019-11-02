#include "femto.h"
#include "syscall.h"

static int console_getchar()
{
    char c;
    __read(0, &c, 1);
    return c;
}

static int console_putchar(int ch)
{
    __write(0, &ch, 1);
    return ch;
}

console_device_t console_user = {
    NULL,
    console_getchar,
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
