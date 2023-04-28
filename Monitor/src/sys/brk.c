#include "brk.h"

int execSyscall_brk(variant *master, variant *replica) {

    /* Syscall arguments differ in variants due to ASLR + Disjoint Addr Space
     * So resume normally */

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return 0;
}
