#include <stdio.h>
#include <sys/mman.h>

int main() {

    // mmap a 10 byte r+w buffer at 0xa0000000
    void *buf = mmap((void *) 0xa0000000, 10, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return 0;



}