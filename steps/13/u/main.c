#include <stdio.h>
#include "thread.h"

#define THREAD_NUM  (5)

typedef struct {
    int arg;
    int ret;
    int tag;
} thread_arg_t;

static uint32_t thread_stack[THREAD_NUM][512 / sizeof(uint32_t)];

static volatile int count = 0;

static void __attribute__((noinline)) increment_count(int i)
{
    int c = count;
    if ((i % 0x1000) == 0) putchar('.');
    c ++;
    count = c;
}

static void *thread_entry(void *arg)
{
    thread_arg_t *thread_arg = (thread_arg_t*)arg;
    (void)thread_arg;
    for (int i = 0; i < 10; i ++) {
        //printf("Hello from thread(%d) %d. arg is: %x\n", thread_arg->tag, i, thread_arg->arg);
    }
    for (int i = 0; i < 1000000; i ++) {
        increment_count(i);
    }
    //return (void*)thread_arg->ret;
    return (void*)count;
}

int main()
{
    printf("Hello RISC-V U-Mode from ELF.\n");
    thread_arg_t thread_arg[THREAD_NUM];
    thread_t thread[THREAD_NUM];
    thread_attr_t thread_attr[THREAD_NUM];
    thread_mutex_t mutex;
    thread_mutex_init(&mutex);
    for (int i = 0; i < THREAD_NUM; i ++) {
        thread_arg[i].arg = 0xdeadbeef;
        thread_arg[i].ret = 0xcafebebe;
        thread_arg[i].tag = i;
        thread_attr_setstack(&thread_attr[i], thread_stack[i], sizeof(thread_stack[i]));
        int ret = thread_create(&thread[i], &thread_attr[i], thread_entry, &thread_arg[i]);
        printf("ret %d, thread id %d\n", ret, thread[i].id);
    }
    for (int i = THREAD_NUM - 1; i >= 0; i --) { // by reverse order
        int retval;
        thread_join(&thread[i], (void**)&retval);
        printf("joined %x(%d), thread id %d\n", retval, retval, thread[i].id);
    }
    return 0;
}
