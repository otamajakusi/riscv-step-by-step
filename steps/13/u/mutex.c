/*
    +------------------+
    |                  |
    v       (1)        |(3)
    UNLOCKED--+-->LOCKED_UNCONTENDED
    ^      |           |(4)
    |   (2)|           v
    |      +--->LOCKED_CONTENDED<---+
    |(6)               |         (5)|
    +------------------+------------+

(1) mutex_trylock() on UNLOCKED state
(2) mutex_lock()    on UNLOCKED state
(3) mutex_unlock()  on LOCKED_UNCONTENDED state
(4) mutex_lock()    on LOCKED_UNCONTENDED state
(5) mutex_lock()    on LOCKED_CONTENDED state
(6) mutex_unlock()  on LOCKED_CONTENDED state
*/
#include "thread.h"
#include "syscall.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "atomic.h"

#define MUTEX_UNLOCKED              (0)
#define MUTEX_LOCKED_UNCONTENDED    (1)
#define MUTEX_LOCKED_CONTENDED      (2)

/* mutex: Note: Compare to linux, we don't have attr param. */
int thread_mutex_init(thread_mutex_t *mutex)
{
    memset(mutex, 0, sizeof(*mutex));
    return 0;
}

int thread_mutex_destroy(thread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}

int thread_mutex_lock(thread_mutex_t *mutex)
{
    if (thread_mutex_trylock(mutex) == 0) {
        return 0;
    }
    while (1) {
        int lock = atomic_exchange((uint32_t*)mutex, MUTEX_LOCKED_CONTENDED);
        if (lock == MUTEX_UNLOCKED) {
            break;
        }
        __futex(mutex, FUTEX_WAIT, MUTEX_LOCKED_CONTENDED);
    }
    return 0;
}

int thread_mutex_trylock(thread_mutex_t *mutex)
{
    if (atomic_compare_exchange((uint32_t*)mutex, MUTEX_UNLOCKED, MUTEX_LOCKED_UNCONTENDED)) {
        return 0;
    }
    return -EAGAIN;
}

int thread_mutex_unlock(thread_mutex_t *mutex)
{
    uint32_t ret = atomic_exchange((uint32_t*)mutex, MUTEX_UNLOCKED);
    if (ret == MUTEX_LOCKED_CONTENDED) {
        int n = __futex(mutex, FUTEX_WAKE, 1);
        (void)n;
    }
    return 0;
}


