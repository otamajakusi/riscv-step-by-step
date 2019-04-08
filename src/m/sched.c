#include <string.h>
#include <stdio.h>
#include "sched.h"
#include "arch/riscv/csr.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"

extern void _idle();

static task_t* root_task = NULL;
static task_t* curr_task = NULL; // NULL if idle

void sched_init(task_t *t)
{
    root_task = curr_task = t;
}

task_t* sched_curr_task()
{
    return curr_task;
}

void sched_schedule(uintptr_t* regs, uintptr_t epc)
{
    task_t* curr = curr_task ? curr_task->next : root_task;
    task_t* next = NULL;
    do {
        if (curr->status <= task_stat_running) {
            next = curr;
            break;
        }
        curr = curr->next;
    } while (curr->next != curr_task);

    if (next == curr_task) {
        return;
    }

    if (curr_task) { // curr_task is not idle and next is not curr_task
        // save current regs and epc to the context
        memcpy(curr_task->regs, regs, sizeof(curr_task->regs));
        curr_task->epc = epc;
    }

    curr_task = next;

    if (next == NULL) { // next is idle and curr_task is not next(=idle)
        write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_M << 11));
        write_csr(mepc, _idle);
        return;
    }

    // next is not idle
    write_csr(mepc, next->epc);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (next->mode << 11));
    memcpy(regs, next->regs, sizeof(next->regs));
}

