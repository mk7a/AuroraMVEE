#include "close.h"

int execSyscall_close(variant *master, variant *replica) {
    /*
     * int close(int fd)
     */

    int eq = 1;

    /* int fd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    if (eq) {

        return runBothCopyMasterRet(master, replica);

    } else {
        return -1;
    }
}