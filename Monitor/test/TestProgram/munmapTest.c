#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    void* a = mmap(NULL, 4096*10, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char* str = "AAAA";
    memcpy(a, str, 5);
    memcpy(a+4096*2, str, 5);

    munmap(a+4096, 4096);
    printf("a: %s\n", (char*)a);
    printf("a: %s\n", (char*)a+4096*2);
    printf("a: %s\n", (char*)a+4096);
}