#include <stdio.h>
#include <string.h>
#include "thread.h"
#include "thread_example.h"

#define THREAD_NUM  (4)
#define STACK_SIZE  (256 + 64)

static uint32_t thread_stack[THREAD_NUM][STACK_SIZE / sizeof(uint32_t)];

static void *thread_entry(void *arg)
{
    thread_example_arg_t *thread_arg = (thread_example_arg_t*)arg;
    thread_example(thread_arg);
    return (void*)thread_arg->tag;
}

static void dump(uint32_t* addr, size_t size)
{
    for (size_t i = 0; i < size / 4; i ++) {
        if ((i % 4) == 0) {
            printf("%p(%x): ", &addr[i], i);
        }
        printf("%08lx:", addr[i]);
        if (((i + 1) % 4) == 0) {
            printf("\n");
        }
    }

}

int main()
{
    printf("Hello RISC-V U-Mode from ELF.\n");
    extern void atomic_test();
    atomic_test();
    thread_example_arg_t thread_arg[THREAD_NUM];
    thread_t thread[THREAD_NUM];
    thread_attr_t thread_attr[THREAD_NUM];
    thread_mutex_t mutex;
    thread_cond_t cond;
    thread_mutex_init(&mutex);
    thread_cond_init(&cond);
    memset(thread_arg, 0, sizeof(thread_arg));
    memset(thread_stack, 0xaa, sizeof(thread_stack));
    for (int i = 0; i < THREAD_NUM; i ++) {
        thread_arg[i].mutex = &mutex;
        thread_arg[i].cond = &cond;
        thread_arg[i].tag = i;
        // debug
        thread_arg[i].top = thread_arg;
        thread_arg[i].total = THREAD_NUM;
        thread_arg[i].debug = -1;
        thread_attr_setstack(&thread_attr[i], thread_stack[i], sizeof(thread_stack[i]));
    }
    for (int i = 0; i < THREAD_NUM; i ++) {
        int ret = thread_create(&thread[i], &thread_attr[i], thread_entry, &thread_arg[i]);
        printf("ret %d, thread id %d\n", ret, thread[i].id);
    }
    for (int i = THREAD_NUM - 1; i >= 0; i --) { // by reverse order
        int retval;
        thread_join(&thread[i], (void**)&retval);
        printf("joined %x(%d), thread id %d\n", retval, retval, thread[i].id);
    }
    //dump((uint8_t*)thread_stack[0], sizeof(thread_stack[0]));
    //dump((uint8_t*)thread_stack[1], sizeof(thread_stack[1]));
    (void)dump;
    //extern uintptr_t _stacks;
    //dump((uint32_t*)&_stacks, 512);
    return 0;
}
