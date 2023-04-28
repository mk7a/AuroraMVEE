#include "exit_group.h"

int execSyscall_exit_group(variant *master, variant *replica) {
    /*
     * noreturn void syscall(SYS_exit_group, int status);
     */

    /* int status */
    if (master->syscallInfo.entry.args[0] != replica->syscallInfo.entry.args[0]) {
        return -1;
    }

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return 0;
}