/*
 * 1. system call (handle_read)
 * 2. receive data (receive_read_data)
 * 3. retrieve data (retrieve_read_data)
 */
#include <stdio.h>
#include "read.h"
#include "sched.h"

static struct task_t* queue = NULL;

static int retrieve_read_data(uintptr_t* regs, task_t* p, int c)
{
    uintptr_t va = regs[REG_CTX_A2];
    uintptr_t pa = va_to_pa(p->pte, va, 0);
    *(char*)(PAGE_OFFSET(va) + pa) = c;
    regs[REG_CTX_A0] = 1; // return size
    return 0;
}

void handle_read(uintptr_t* regs, task_t* curr)
{
    (void)regs;
    enqueue(&queue, curr);
    block_current_task();
}

int receive_read_data(char c)
{
    if (queue == NULL) {
        return -1;
    }

    task_t* p = dequeue(&queue);
    ready_task(p);
    retrieve_read_data(p->regs, p, c);
    return 0;
}

