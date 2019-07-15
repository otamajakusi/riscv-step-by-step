/*
 * 1. system call (handle_read)
 * 2. receive data (receive_read_data)
 * 3. retrieve data (retrieve_read_data)
 */
#include <stdio.h>
#include "read.h"
#include "sched.h"
#include "task.h"

static struct task_t* queue = NULL;

static int retrieve_read_data(uintptr_t* regs, task_t* p, int c)
{
    uintptr_t va = regs[REG_CTX_A2];
    store_8_to_user(p, va, (uint8_t)c);
    regs[REG_CTX_A0] = 1; // return size
    return 0;
}

void handle_read(uintptr_t* regs, task_t* curr)
{
    (void)regs;
    task_enqueue_to_root(&queue, curr);
    block_current_task();
}

int receive_read_data(char c)
{
    if (queue == NULL) {
        return -1;
    }

    task_t* p = task_dequeue_from_root(&queue, queue);
    ready_task(p);
    retrieve_read_data(p->regs, p, c);
    return 0;
}

