#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <stdio.h>
#include <sys/mman.h>

int main() {


    char* buf = mmap((void *) 0xa0000000, 10, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    getchar();
    syscall(SYS_getrandom, buf, 10, 0);
    // hexdump
    printf("Bytes: ");
    for (int i = 0; i < 10; i++) {
        printf("%02x ", (unsigned char) buf[i]);
    }
    printf("\n");
    getchar();
    return 0;
}