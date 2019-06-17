#include <stdio.h>
#include "read.h"
#include "sched.h"

#define BUF_SIZE    (32)

static char buf[BUF_SIZE];
static int wp = 0;
static int rp = 0;

static struct task_t* queue = NULL;

static int retrieve_read_data()
{
    if (rp == wp) {
        return -1;
    }
    int c = buf[rp];
    rp = (rp + 1) % BUF_SIZE;
    return c;
}

static void enqueue(task_t *curr)
{
    if (queue == NULL) {
        task_single(curr);
        queue = curr;
    } else {
        task_enqueue(queue, curr);
    }
    blocking_current_task();
}

static task_t* dequeue()
{
    ASSERT(queue != NULL);
    task_t *p = queue;
    if (task_is_single(p)) {
        queue = NULL;
    } else {
        queue = p->next;
    }
    ready_task(p);
    return p;
}

int is_read_queue_empty()
{
    return queue == NULL;
}

void handle_read(uintptr_t* regs, task_t* curr)
{
    uintptr_t va = regs[REG_CTX_A2];
    uintptr_t pa = va_to_pa(curr->pte, va, 0);
    char *d = (char*)(PAGE_OFFSET(va) + pa);
    int c = retrieve_read_data();
    if (c == -1) {
        enqueue(curr);
        return;
    }
    *d = c;
    return;
}

int receive_read_data(char c)
{
    if (queue == NULL) {
        return -1;
    }
    dequeue();

    int wp_next = (wp + 1) % BUF_SIZE;
    if (wp_next == rp) {
        printf("warning: buf is full\n");
        return 0;
    }

    buf[wp] = c;
    wp = wp_next;
    return 0;
}

