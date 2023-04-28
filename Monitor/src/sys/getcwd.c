#include "getcwd.h"

int execSyscall_getcwd(variant *master, variant *replica) {
    /*
     * char *getcwd(char *buf, size_t size);
     */

    int eq = 1;

    /* char *buf */
    eq &= variantStrCmp(master, (void *) master->syscallInfo.entry.args[0],
                        replica, (void *) replica->syscallInfo.entry.args[0]) == 0;

    /* size_t size */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    if (eq) {

        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;

    } else {
        return -1;
    }
}