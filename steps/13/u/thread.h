#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void *stackaddr;
    size_t stacksize;
} thread_attr_t;

typedef struct {
    int id;
    // following members are used internally.
    void *(*start_routine) (void *);
    void *arg;
} thread_t;

typedef int thread_mutex_t;
typedef int thread_cond_t;

/* thread_attr */
int thread_attr_init(thread_attr_t *attr);
int thread_attr_destroy(thread_attr_t *attr);
/* only setstack is available */
int thread_attr_setstack(thread_attr_t *attr, void *stackaddr, size_t stacksize);

/* thread */
int thread_create(thread_t *thread, const thread_attr_t *attr,
        void *(*start_routine) (void *), void *arg);
int thread_join(thread_t *thread, void **retval);
void thread_exit(void *retval) __attribute__((noreturn));

/* mutex: Note: we don't have attr. */
int thread_mutex_init(thread_mutex_t *mutex);
int thread_mutex_destroy(thread_mutex_t *mutex);
int thread_mutex_lock(thread_mutex_t *mutex);
int thread_mutex_trylock(thread_mutex_t *mutex);
int thread_mutex_unlock(thread_mutex_t *mutex);

/* cond: Note: we don't have attr. */
int thread_cond_init(thread_cond_t *cond);
int thread_cond_destroy(thread_cond_t *cond);
int thread_cond_wait(thread_cond_t *cond, thread_mutex_t *mutex);
int thread_cond_signal(thread_cond_t *cond);
int thread_cond_broadcast(thread_cond_t *cond);

#ifdef __cplusplus
}
#endif
