#include "fchmod.h"

int execSyscall_fchmod(variant *master, variant *replica) {
    /*
     * int fchmod(int fildes, mode_t mode);
     */

    int eq = 1;

    /* int fildes */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* mode_t mode */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    if (eq) {
        return runMasterReplicateRet(master, replica);
    } else {
        return -1;
    }
}