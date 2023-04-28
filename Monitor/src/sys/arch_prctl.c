#include "arch_prctl.h"


int execSyscall_arch_prctl(variant *master, variant *replica) {

    /* int code */
    if (master->syscallInfo.entry.args[0] != replica->syscallInfo.entry.args[0]) return -1;

    /* unsigned long addr */
    /* Equality not required */

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return 0;
}