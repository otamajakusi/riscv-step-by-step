#pragma once

#include <stdint.h>
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void handle_read(uintptr_t* regs, task_t* curr);
int receive_read_data(char c);

#ifdef __cplusplus
}
#endif
