#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

#define BUF_SIZE    (512 * 1024)

static char mem[BUF_SIZE];
static unsigned int mem_pos = 0;

void mem_printf(const char *format, ...)
{
    char buf[64];
    va_list ap;
    va_start(ap, format);
    vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);

    char *str = buf;
    while (*str && mem_pos < sizeof(mem)) {
        mem[mem_pos] = *str;
        mem_pos ++;
        str ++;
    }
}

void mem_flush()
{
    for (size_t i = 0; i < sizeof(mem); i ++) {
        if (mem[i] == NULL) {
            break;
        }
        putchar(mem[i]);
    }
}

