#include "access.h"

int execSyscall_access(variant *master, variant *replica) {
    /*
     * int access(const char *pathname, int mode)
     */

    int eq = 1;

    /* int mode */
    eq &= (master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1]);

    /* const char *pathname */
    if (eq) {
        eq &= (variantStrCmp(master, (void *) master->syscallInfo.entry.args[0],
                             replica, (void *) replica->syscallInfo.entry.args[0]) == 0);
    }

    if (eq) {
        runMasterReplicateRet(master, replica);
    }

    return eq ? 0 : -1;
}