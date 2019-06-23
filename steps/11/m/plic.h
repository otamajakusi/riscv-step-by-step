#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void plic_enable_interrupt(int prio, int irq);
uint32_t plic_claim();
void plic_complete(uint32_t irq);

#ifdef __cplusplus
}
#endif
