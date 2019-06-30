#pragma once

#include <stdint.h>
#include "regs.h"
#include "vm.h"
#include "elfldr.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    task_state_created = 0,
    task_state_ready,
    task_state_running,
    task_state_blocked,
    task_state_terminated,
} task_state_t;

typedef struct task_t {
    uintptr_t       entry;
    const Elf32_Ehdr*     ehdr;
    union sv32_pte* pte;
    uintptr_t       regs[REG_CTX_NUM];
    uintptr_t       mepc;
    task_state_t    state;
    uintptr_t       exitcode;
    struct task_t*  prev;
    struct task_t*  next;
} task_t;


/*
(B=)prev - A - next
         prev - B - next(=A)

(C=)prev - A - next
         prev - B - next(=C)
          (B=)prev - C - next(=A)

xxxxxxxxxxxxxxxxxxx
     (C=)prev - B - next(=C)
          (B=)prev - C - next(=B)
          */

static inline void task_single(task_t *elem)
{
    elem->prev = elem;
    elem->next = elem;
}

static inline int task_is_single(task_t *elem)
{
    if (elem->prev == elem && elem->next == elem) {
        return 1;
    }
    return 0;
}

static inline void task_enqueue(task_t *top, task_t *elem)
{
    elem->next = top;
    elem->prev = top->prev;
    top->prev->next = elem;
    top->prev = elem;
}

static inline void task_dequeue(task_t *elem)
{
    elem->next->prev = elem->prev;
    elem->prev->next = elem->next;
}

static inline void enqueue(task_t **root, task_t *curr)
{
    if (*root == NULL) {
        task_single(curr);
        *root = curr;
    } else {
        task_enqueue(*root, curr);
    }
}

static inline task_t* dequeue(task_t **root)
{
    ASSERT(*root != NULL);
    task_t *p = *root;
    if (task_is_single(p)) {
        *root = NULL;
    } else {
        task_dequeue(p);
        *root = p->next;
    }
    return p;
}

#ifdef __cplusplus
}
#endif
