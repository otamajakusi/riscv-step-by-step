#include "thread.h"
#include "syscall.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "atomic.h"

/* cond: Note: Compare to linux, we don't have attr param. */
int thread_cond_init(thread_cond_t *cond)
{
    memset(cond, 0, sizeof(*cond));
    return 0;
}

int thread_cond_destroy(thread_cond_t *cond)
{
    (void)cond;
    return 0;
}

int thread_cond_wait(thread_cond_t *cond, thread_mutex_t *mutex)
{
    uint32_t old = atomic_load_relaxed((uint32_t*)cond);
    thread_mutex_unlock(mutex);
    __futex(cond, FUTEX_WAIT, old);
    thread_mutex_lock(mutex);
    return 0;
}

int thread_cond_signal(thread_cond_t *cond)
{
    atomic_fetch_add_relaxed((uint32_t*)cond, 1);
    __futex(cond, FUTEX_WAKE, 1);
    return 0;
}

int thread_cond_broadcast(thread_cond_t *cond)
{
    atomic_fetch_add_relaxed((uint32_t*)cond, 1);
    __futex(cond, FUTEX_WAKE, INT_MAX);
    return 0;
}

