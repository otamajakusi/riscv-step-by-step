#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void plic_enable_interrupt(int prio, int irq);

#ifdef __cplusplus
}
#endif
