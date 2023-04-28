#include "chdir.h"
#include "../debug.h"

int execSyscall_chdir(variant *master, variant *replica) {
    /* int chdir(const char *path); */

    int eq = (variantStrCmp(master, (void *) master->syscallInfo.entry.args[0],
                            replica, (void *) replica->syscallInfo.entry.args[0]) == 0);

    if (eq == 1) {
        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;
    } else {
        debug_printf(stderr, "chdir: path mismatch\n");
        return -1;
    }
}