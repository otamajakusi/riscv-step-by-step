#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sched.h"
#include "task.h"
#include "consts.h"

static task_t task[USER_NUM];
static int curr_task_num = 0;

static void idle() __attribute__((noreturn));
static void enter_idle() __attribute__((noreturn));

static void idle()
{
    for (;;) {
    }
}

static void enter_idle()
{
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_M << 11));
    write_csr(mepc, idle);
    mret();
}

static task_t* pickup_next_task()
{
    int next = (curr_task_num + 1) % USER_NUM;
    for (int i = 0; i < USER_NUM; i ++, next = (next + 1) % USER_NUM) {
        if (task[next].state == task_state_ready ||
            task[next].state == task_state_running) {
            if (curr_task_num == next) {
                /* curr_task is selected again */
            } else {
                if (task[curr_task_num].state == task_state_running) {
                    task[curr_task_num].state = task_state_ready;
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
    for (size_t i = 0; i < USER_NUM; i ++) {
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

task_t* get_current_task()
{
    if (curr_task_num < 0) {
        return NULL;
    }
    return &task[curr_task_num];
}

void terminate_current_task()
{
    task_t* p = get_current_task();
    p->state = task_state_terminated;
}

void switch_task(uintptr_t* regs, uintptr_t mepc)
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
        memcpy(regs, curr->regs, sizeof(curr->regs));
        write_csr(mepc, curr->mepc);
        set_satp(curr->pte);
    }
}

void schedule()
{
    // jump entry with U-Mode
    task_t *curr = get_current_task();
    curr->state = task_state_running;
    set_satp(curr->pte);
    write_csr(mepc, curr->entry);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11) | MSTATUS_MPIE);
    mret();
}
