#include "fadvise64.h"


int execSyscall_fadvise64(variant *master, variant *replica) {
    /*
     * int syscall(SYS_fadvise64, int fd, off_t offset, size_t len, int advice);
     */

    int eq = 1;

    /* int fd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* off_t offset */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* size_t len */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    /* int advice */
    eq &= master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3];

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return eq ? 0 : -1;
}