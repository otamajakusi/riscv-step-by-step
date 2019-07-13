#include <stdio.h>
#include "task.h"

#define VA_TO_PA(task, va) do { \
    pa = va_to_pa(task->pte, va, 0); \
    if (pa == -1u) { \
        printf("error: va %x\n", va); \
        return -1; \
    } \
} while (0)

int store_32_to_user(const task_t* task, uintptr_t va, uint32_t val)
{
    uintptr_t pa;
    VA_TO_PA(task, va);
    // FIXME: make sure sizeof(uint32_t) at (PAGE_OFFSET(va) + pa) is not page boundary.
    *(uint32_t*)(PAGE_OFFSET(va) + pa) = val;
    return 0;
}

int load_32_from_user(const task_t* task, uintptr_t va, uint32_t *val)
{
    uintptr_t pa;
    VA_TO_PA(task, va);
    // FIXME: make sure sizeof(uint32_t) at (PAGE_OFFSET(va) + pa) is not page boundary.
    *val = *(uint32_t*)(PAGE_OFFSET(va) + pa);
    return 0;
}

int store_8_to_user(const task_t* task, uintptr_t va, uint8_t val)
{
    uintptr_t pa;
    VA_TO_PA(task, va);
    *(uint8_t*)(PAGE_OFFSET(va) + pa) = val;
    return 0;
}

int load_8_from_user(const task_t* task, uintptr_t va, uint8_t *val)
{
    uintptr_t pa;
    VA_TO_PA(task, va);
    *val = *(uint8_t*)(PAGE_OFFSET(va) + pa);
    return 0;
}


