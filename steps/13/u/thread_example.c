#include <stdio.h>
#include "thread.h"
#include "thread_example.h"

static volatile int count = 0;

static thread_mutex_t printf_mutex;

static void debug_mutex(thread_example_arg_t *arg, int line)
{
    thread_example_arg_t *top = arg->top;
    for (int i = 0; i < arg->total; i ++) {
        if (&top[i] != arg && top[i].debug != -1) {
            printf("#%d %d(%d): multi-lock! exp: %d(%d)\n",
                    line, i, top[i].debug, arg->tag, arg->debug);
        }
    }
}

static void __attribute__((noinline))
    increment_count(int i, thread_example_arg_t *arg)
{
    thread_mutex_t *mutex = arg->mutex;
    thread_mutex_lock(mutex);
#if 0
    arg->debug = mutex->status;
    debug_mutex(arg, __LINE__);
    int c = count;
    if ((i % 0x100) == 0) putchar(0);
    c ++;
    count = c;
    arg->debug = -1;
    debug_mutex(arg, __LINE__);
#else
    (void)i; (void)debug_mutex;
    int c = count;
    c ++;
    count = c;
#endif
    thread_mutex_unlock(mutex);
}

static void mutex_example(thread_example_arg_t *arg)
{
    for (int i = 0; i < 100000; i ++) {
        increment_count(i, arg);
    }
//    thread_mutex_lock(&printf_mutex);
    printf("[%d]: count %d\n", arg->tag, count);
//    thread_mutex_unlock(&printf_mutex);
}

static void cond_example(thread_example_arg_t *arg)
{
    (void)arg;
    // TODO:
}

void thread_example(thread_example_arg_t *arg)
{
    thread_mutex_init(&printf_mutex);
    mutex_example(arg);
    cond_example(arg);
}
