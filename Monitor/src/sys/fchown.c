#include "fchown.h"

int execSyscall_fchown(variant *master, variant *replica) {
    /*
     * int fchown(int fildes, uid_t owner, gid_t group);
     */

    int eq = 1;

    /* int fildes */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* uid_t owner */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* gid_t group */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    if (eq) {
        return runMasterReplicateRet(master, replica);
    } else {
        return -1;
    }
}