#include <stdio.h>
#include "thread.h"

typedef struct {
    int arg;
    int ret;
} thread_arg_t;

static uint32_t thread_stack[512 / sizeof(uint32_t)];

static void *thread_entry(void *arg)
{
    thread_arg_t *thread_arg = (thread_arg_t*)arg;
    for (int i = 0; i < 10; i ++) {
        printf("Hello from thread %d. arg is: %x\n", i, thread_arg->arg);
    }
    return (void*)thread_arg->ret;
}

int main()
{
    printf("Hello RISC-V U-Mode from ELF.\n");
    thread_arg_t thread_arg = {0xdeadbeef, 0xcafebebe};
    int retval;
    int ret;
    thread_t thread;
    thread_attr_t thread_attr;
    thread_attr_setstack(&thread_attr, thread_stack, sizeof(thread_stack));
    ret = thread_create(&thread, &thread_attr, thread_entry, &thread_arg);
    printf("ret %d, thread id %d\n", ret, thread.id);
    thread_join(&thread, (void**)&retval);
    printf("joined %x\n", retval);
    return 0;
}
