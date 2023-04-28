#include "utimensat.h"

int execSyscall_utimensat(variant *master, variant *replica) {
    /*
     * int utimensat(int dirfd, const char *pathname,
                     const struct timespec times[2], int flags)
     */

    int eq = 1;

    /* int dirfd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* const char *pathname */
    if (eq) {
        eq &= variantStrCmp(master, (void *) master->syscallInfo.entry.args[1],
                            replica, (void *) replica->syscallInfo.entry.args[1]) == 0;
    }

    /* const struct timespec times[2] */
    /* Equivalence not required */

    /* int flags */
    if (eq) {
        eq &= master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3];
    }

    if (eq) {
        return runMasterReplicateRet(master, replica);
    } else {
        return -1;
    }
}