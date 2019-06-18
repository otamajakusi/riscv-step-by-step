#include <stdio.h>

int main()
{
    printf("Hello RISC-V U-Mode from ELF.\n");
    for (size_t i = 0; i < 10; i ++) {
        int c = getchar();
        printf("%d: %c\n", i, c);
    }
    printf("exit..\n");
    return 0;
}
