#include "lseek.h"
#include "../debug.h"

int execSyscall_lseek(variant *master, variant *replica) {
    /* off_t lseek(int fd, off_t offset, int whence); */

    int eq = 1;

    /* int fd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* off_t offset */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* int whence */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    if (eq) {
        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;

    } else {
        return -1;
    }

    return 0;
}