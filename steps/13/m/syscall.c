#include <stdio.h>
#include <string.h>
#include "../u/syscall.h"
#include "syscall.h"
#include "regs.h"
#include "femto.h"
#include "arch/riscv/encoding.h"
#include "arch/riscv/machine.h"
#include "arch/riscv/csr.h"
#include "consts.h"
#include "sched.h"
#include "read.h"
#include "task.h"
#include "futex.h"

static void handle_write(uintptr_t* regs, const task_t* curr)
{
    uintptr_t va = regs[REG_CTX_A2];
    char c;
    load_8_from_user(curr, va, (uint8_t*)&c);
    putchar(c);
}

static void handle_clone(uintptr_t* regs, const task_t* curr)
{
    (void)curr;
    regs[REG_CTX_A0] = clone_current_task(
            regs[REG_CTX_A1],
            regs[REG_CTX_A2],
            regs[REG_CTX_A3]);
}

void handle_syscall(uintptr_t* regs, uintptr_t mepc, task_t* curr)
{
    switch (regs[REG_CTX_A0]) {
    case SYSCALL_READ:
        handle_read(regs, curr);
        return schedule(regs, mepc + 4);
    case SYSCALL_WRITE:
        handle_write(regs, curr);
        break;
    case SYSCALL_EXIT:
        terminate_current_task(regs[REG_CTX_A1]);
        return schedule(regs, mepc);
    case SYSCALL_CLONE:
        handle_clone(regs, curr);
        return schedule(regs, mepc + 4);
    case SYSCALL_WAITPID:
        handle_waitpid(regs, curr);
        return schedule(regs, mepc + 4);
    case SYSCALL_FUTEX:
        handle_futex(regs, curr);
        return schedule(regs, mepc + 4);
    default:
        break;
    }
    write_csr(mepc, mepc + 4);
    return;
}


