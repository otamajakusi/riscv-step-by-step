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
    printf("Hello from thread. arg=%x\n", thread_arg->arg);
    return (void*)thread_arg->ret;
}

int main()
{
    printf("Hello RISC-V U-Mode from ELF.\n");
    thread_arg_t thread_arg = {0xdeadbeef, 0xcafebebe};
    void *retval;
    thread_t thread;
    thread_attr_t thread_attr;
    thread_attr_setstack(&thread_attr, thread_stack, sizeof(thread_stack));
    thread_create(&thread, &thread_attr, thread_entry, &thread_arg);
    thread_join(thread, &retval);
    printf("joined %x\n", *(int*)retval);
    return 0;
}
