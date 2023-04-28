#include "security_manager.h"
#include "debug.h"

#include <sys/mman.h>
#include <stdio.h>

int checkMMapPerms(int prot, int flags) {

    if ((prot & PROT_WRITE) && !(flags & MAP_PRIVATE)) {
        fprintf(stderr, "mmap: Shared writable mmap not supported\n");
        return -1;
    }

    return 0;
}