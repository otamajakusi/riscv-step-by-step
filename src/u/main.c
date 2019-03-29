#include <stdio.h>

int main() {
    int count = 0;
    while (count < 10) {
        for (volatile int i = 0; i < 10000000; i ++);
        printf("begin hello risc-v (U mode) %d\n", count ++);
        for (volatile int i = 0; i < 10000000; i ++);
    }
    return 0;
}
