#include <stdio.h>
#include "thread.h"

#define THREAD_NUM  (3)

typedef struct {
    int arg;
    int ret;
    int tag;
} thread_arg_t;

static uint32_t thread_stack[THREAD_NUM][512 / sizeof(uint32_t)];

static void *thread_entry(void *arg)
{
    thread_arg_t *thread_arg = (thread_arg_t*)arg;
    for (int i = 0; i < 10; i ++) {
        printf("Hello from thread(%d) %d. arg is: %x\n", thread_arg->tag, i, thread_arg->arg);
    }
    return (void*)thread_arg->ret;
}

int main()
{
    printf("Hello RISC-V U-Mode from ELF.\n");
    thread_arg_t thread_arg[THREAD_NUM];
    thread_t thread[THREAD_NUM];
    thread_attr_t thread_attr[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; i ++) {
        thread_arg[i].arg = 0xdeadbeef;
        thread_arg[i].ret = 0xcafebebe;
        thread_arg[i].tag = i;
        thread_attr_setstack(&thread_attr[i], thread_stack[i], sizeof(thread_stack[i]));
        int ret = thread_create(&thread[i], &thread_attr[i], thread_entry, &thread_arg[i]);
        printf("ret %d, thread id %d\n", ret, thread[i].id);
    }
    for (int i = 0; i < THREAD_NUM; i ++) {
        int retval;
        thread_join(&thread[i], (void**)&retval);
        printf("joined %x\n", retval);
    }
    return 0;
}
