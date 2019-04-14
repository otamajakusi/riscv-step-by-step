#include <stdio.h>

int main() {
    int count = 0;
    while (count < 32) {
        printf("begin hello risc-v (U mode) %d main %p\n", count ++, main);
        for (volatile int i = 0; i < 10000000; i ++);
        int c = getchar();
        if (c != -1) {
            printf("%c", c);
        }
    }
    return 0;
}
