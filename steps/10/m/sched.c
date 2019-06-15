#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sched.h"
#include "task.h"
#include "consts.h"

static task_t task[USER_NUM];
static int curr_task_num = 0;

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
    return &task[curr_task_num];
}

void switch_task(uintptr_t* regs, uintptr_t mepc)
{
    task_t *curr;

    curr = &task[curr_task_num];
    // save context
    memcpy(curr->regs, regs, sizeof(curr->regs));
    curr->mepc = mepc;

    // change curr
    curr_task_num = (curr_task_num + 1) % USER_NUM;
    curr = &task[curr_task_num];

    // restore context
    memcpy(regs, curr->regs, sizeof(curr->regs));
    write_csr(mepc, curr->mepc);

    set_satp(curr->pte);
}

void schedule()
{
    // jump entry with U-Mode
    task_t *curr = get_current_task();
    set_satp(curr->pte);
    write_csr(mepc, curr->entry);
    write_csr(mstatus, (read_csr(mstatus) & ~MSTATUS_MPP) | (PRV_U << 11) | MSTATUS_MPIE);
    mret();
}
