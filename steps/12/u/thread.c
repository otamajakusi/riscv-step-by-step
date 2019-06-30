#include "thread.h"
#include "syscall.h"
#include <string.h>
#include <stdio.h>

typedef struct {
   void *(*start_routine) (void *);
   void *arg;
} thread_arg_t;

static void *thread_entry(void *arg)
{
    thread_t *thread = (thread_t*)arg;
    int status = (int)thread->start_routine(thread->arg);
    thread_exit(&status);
}

static int thread_clone(thread_t *thread, const thread_attr_t *attr,
        void *(*start_routine) (void *), void *arg)
{
    thread->start_routine = start_routine;
    thread->arg = arg;
    thread->id = __clone(thread_entry, attr->stackaddr + attr->stacksize, thread);
    return thread->id < 0 ? -1 : 0;
}

/* thread_attr */
int thread_attr_init(thread_attr_t *attr)
{
    memset(attr, 0, sizeof(*attr));
    return 0;
}

int thread_attr_destroy(thread_attr_t *attr)
{
    (void)attr;
    return 0;
}

int thread_attr_setstack(thread_attr_t *attr, void *stackaddr, size_t stacksize)
{
    attr->stackaddr = stackaddr;
    attr->stacksize = stacksize;
    return 0;
}

/* thread */
int thread_create(thread_t *thread, const thread_attr_t *attr,
        void *(*start_routine) (void *), void *arg)
{
    return thread_clone(thread, attr, start_routine, arg);
}

/* Note: Compare to linux syscall, we use thread_t *, not thread_t. */
int thread_join(thread_t *thread, void **retval)
{
    return __waitpid(thread->id, (int*)retval);
}

void thread_exit(void *retval)
{
    __exit(*(int*)retval);
}

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
    (void)mutex;
    return 0;
}

int thread_mutex_trylock(thread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}

int thread_mutex_unlock(thread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}

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
    (void)cond;
    (void)mutex;
    return 0;
}

int thread_cond_signal(thread_cond_t *cond)
{
    (void)cond;
    return 0;
}

int thread_cond_broadcast(thread_cond_t *cond)
{
    (void)cond;
    return 0;
}

