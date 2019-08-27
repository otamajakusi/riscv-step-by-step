/*
 */
#include <stdio.h>
#include "futex.h"
#include "sched.h"
#include "../u/syscall.h"

static struct task_t* queue = NULL;

static int handle_futex_wait(int uaddr, int val, task_t* curr)
{
    int ret;
    uint32_t act;

    ret = load_32_from_user(curr, uaddr, &act);
    if (ret < 0) {
        printf("error: va %x\n", uaddr);
        return -EFAULT;
    }
    //printf("\n[task %p, wait act %ld, val %d]\n", curr, act, val);
    if (act != (uint32_t)val) {
        return -EAGAIN;
    }
    task_enqueue_to_root(&queue, curr);
    block_current_task();
    return 0;
}

static int handle_futex_wake(int uaddr, int val, task_t* curr)
{
    (void)curr;
    int num = 0;
    // search queue
    if (queue == NULL || val <= 0) {
        return num;
    }
    task_t *w = queue;
    do {
        if (w->state == task_state_blocked &&
            w->regs[REG_CTX_A1] == (uintptr_t)uaddr) {
            task_dequeue_from_root(&queue, w);
            //printf("\n[%p wake by task %p]\n", w, curr);
            ready_task(w);
            num ++;
        }
        w = w->next;
    } while (w != queue && val != num);
    return num;
}

void handle_futex(uintptr_t* regs, task_t* curr)
{
    int uaddr = regs[REG_CTX_A1];
    int futex_op = regs[REG_CTX_A2];
    int val = regs[REG_CTX_A3];
    switch (futex_op) {
    case FUTEX_WAIT:
        regs[REG_CTX_A0] = handle_futex_wait(uaddr, val, curr);
        return;
    case FUTEX_WAKE:
        regs[REG_CTX_A0] = handle_futex_wake(uaddr, val, curr);
        return;
    default:
        regs[REG_CTX_A0] = -ENOSYS;
        return;
    }
}

void dump_futex_queue()
{
    printf("dump futex queue\n");
    if (queue == NULL) {
        return;
    }
    task_t *w = queue;
    do {
        printf("w %p(%x)\n", w, w->regs[REG_CTX_A1]);
        w = w->next;
    } while (w != queue);
}
