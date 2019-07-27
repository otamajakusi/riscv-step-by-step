#pragma once

#include <stdint.h>
#include <stddef.h>
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct thread_example_arg_t {
    thread_mutex_t *mutex;
    thread_cond_t *cond;
    int tag;
    // debug members
    struct thread_example_arg_t *top;
    int total;
    int debug;
} thread_example_arg_t;

void thread_example(thread_example_arg_t *arg);

#ifdef __cplusplus
}
#endif
