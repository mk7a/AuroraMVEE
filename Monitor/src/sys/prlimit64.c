#include "prlimit64.h"

int execSyscall_prlimit64(variant *master, variant *replica) {
    /*
     * int prlimit(pid_t pid, int resource, const struct rlimit *new_limit,
                   struct rlimit *old_limit);
     */

    /* Resource */
    if (master->syscallInfo.entry.args[1] != replica->syscallInfo.entry.args[1]) return -1;

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return 0;
}