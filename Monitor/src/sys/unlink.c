#include "unlink.h"

int execSyscall_unlink(variant *master, variant *replica) {
    /* int unlink(const char *pathname); */

    int eq = 1;

    /* const char *pathname */
    eq &= variantStrCmp(master, (void *) master->syscallInfo.entry.args[0],
                        replica, (void *) replica->syscallInfo.entry.args[0]) == 0;

    if (eq) {
        return runMasterReplicateRet(master, replica);
    } else {
        return -1;
    }
}