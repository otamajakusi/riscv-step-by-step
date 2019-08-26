#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sched.h"
#include "task.h"
#include "consts.h"

static task_t task[USER_NUM_MAX];
static int curr_task_num = 0;

static struct task_t* wait_queue = NULL;

static void __attribute__((noreturn)) idle()
{
    for (;;) {
        asm volatile ("wfi");
    }
}

static void __attribute__((noreturn)) enter_idle()
{
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_M << 11));
    write_csr(mepc, idle);
    mret();
}

static task_t* pickup_next_task()
{
    int next = (curr_task_num + 1) % USER_NUM_MAX;
    task_t *curr = get_current_task();
    for (int i = 0; i < USER_NUM_MAX; i ++, next = (next + 1) % USER_NUM_MAX) {
        if (task[next].state == task_state_ready ||
            task[next].state == task_state_running) {
            if (curr == &task[next]) {
                /* curr_task is selected again */
            } else {
                if (curr && curr->state == task_state_running) {
                    curr->state = task_state_ready;
                }
                task[next].state = task_state_running;
                curr_task_num = next;
            }
            return &task[next];
        }
    }
    /* no tasks are available */
    curr_task_num = -1;
    return NULL;
}

int create_task(const Elf32_Ehdr* ehdr, union sv32_pte* pte)
{
    for (size_t i = 0; i < USER_NUM_MAX; i ++) {
        if (task[i].state == task_state_created) {
            task_t* p = &task[i];
            p->entry = ehdr->e_entry;
            p->ehdr = ehdr;
            p->pte = pte;
            p->state = task_state_ready;
            return 0;
        }
    }
    return -1;
}

int clone_current_task(uintptr_t fn, uintptr_t stack, uintptr_t arg)
{
    task_t *curr = get_current_task();
    for (size_t i = 0; i < USER_NUM_MAX; i ++) {
        if (task[i].state == task_state_created) {
            task_t* p = &task[i];
            memcpy(p, curr, sizeof(task_t));
            p->mepc = fn;
            p->regs[REG_CTX_SP]= stack;
            p->regs[REG_CTX_A0]= arg;
            p->state = task_state_ready;
            return i;
        }
    }
    return -1;
}

task_t* get_current_task()
{
    if (curr_task_num < 0) {
        return NULL;
    }
    return &task[curr_task_num];
}

void terminate_current_task(int code)
{
    task_t* p = get_current_task();
    p->state = task_state_terminated;
    p->exitcode = code;
    // search wait_queue
    if (wait_queue == NULL) {
        return;
    }
    task_t *w = wait_queue;
    do {
        if (w->state == task_state_blocked &&
            w->regs[REG_CTX_A1] == (uintptr_t)curr_task_num) {
            task_dequeue_from_root(&wait_queue, w);
            printf("%p wakeup by exit task %p(%d)\n", w, p, curr_task_num);
            w->state = task_state_ready;
            uintptr_t va = w->regs[REG_CTX_A2];
            uintptr_t pa = va_to_pa(w->pte, va, 0);
            if (pa == -1u) {
                printf("error: va %x\n", va);
                w->regs[REG_CTX_A0] = -1;
                return;
            }
            // FIXME: make sure sizeof(uintptr_r) at (PAGE_OFFSET(va) + pa) is not page boundary.
            *(uintptr_t*)(PAGE_OFFSET(va) + pa) = p->exitcode;
            return;
        }
        w = w->next;
    } while (w != wait_queue);
}

void block_current_task()
{
    task_t* p = get_current_task();
    p->state = task_state_blocked;
}

void ready_task(task_t *p)
{
    p->state = task_state_ready;
}

void handle_waitpid(uintptr_t* regs, task_t* curr)
{
    uintptr_t pid = regs[REG_CTX_A1];
    if (pid > USER_NUM_MAX) {
        // illegal pid
        printf("illegal pid %d\n", pid);
        regs[REG_CTX_A0] = -1;
        return;
    }
    if (task[pid].state == task_state_terminated) {
        uintptr_t va = regs[REG_CTX_A2];
        uintptr_t pa = va_to_pa(curr->pte, va, 0);
        if (pa == -1u) {
            printf("error: va %x\n", va);
            regs[REG_CTX_A0] = -1;
            return;
        }
        // FIXME: make sure sizeof(uintptr_r) at (PAGE_OFFSET(va) + pa) is not page boundary.
        *(uintptr_t*)(PAGE_OFFSET(va) + pa) = task[pid].exitcode;
        return;
    }
    if (task[pid].state != task_state_ready &&
        task[pid].state != task_state_blocked) {
        printf("pid %d not available %d\n", pid, task[pid].state);
        regs[REG_CTX_A0] = -1;
        return;
    }
    task_enqueue_to_root(&wait_queue, curr);
    block_current_task();
}

void schedule(uintptr_t* regs, uintptr_t mepc)
{
    task_t *curr = get_current_task();
    if (curr != NULL) { // curr is not idle
        // save context
        memcpy(curr->regs, regs, sizeof(curr->regs));
        curr->mepc = mepc;
    }

    // change curr
    curr = pickup_next_task();

    if (curr == NULL) {
        enter_idle();
    } else {
        // restore context
        write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11));
        memcpy(regs, curr->regs, sizeof(curr->regs));
        write_csr(mepc, curr->mepc);
        set_satp(curr->pte);
    }
}

void start_schedule()
{
    // jump entry with U-Mode
    task_t *curr = get_current_task();
    curr->state = task_state_running;
    set_satp(curr->pte);
    write_csr(mepc, curr->entry);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11) | MSTATUS_MPIE);
    mret();
}
