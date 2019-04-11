#pragma once

#include <stdint.h>
#include "regs.h"

#define TASK_STATUS_READY

#ifdef __cplusplus
extern "C" {
#endif

enum {
    /* currently, ready and running are not different */
    task_stat_ready,
    task_stat_running,
    task_stat_blocking,
    task_stat_terminated,
};

typedef struct task_t {
    char tag[8];
    uintptr_t regs[REG_CTX_NUM];
    uint32_t cause; // interrupted cause
    uintptr_t epc;   // entry or interrupted address
    uint32_t status; // ready(0), running(1) and blocked(2)
    uint32_t count;
    uint32_t mode;
    uint32_t paddr; // 32-bit physical address is supported
    uint32_t offset; // 32-bit physical address is supported
    struct task_t *prev;
    struct task_t *next;
} task_t;

#ifdef __cplusplus
}
#endif
