#include "rt_sigprocmask.h"
#include "../debug.h"
#include <signal.h>

int execSyscall_rt_sigprocmask(variant *master, variant *replica) {
    /*
     * int syscall(SYS_rt_sigprocmask, int how, const kernel_sigset_t *set,
                       kernel_sigset_t *oldset, size_t sigsetsize);
     */

    int eq = 1;

    /*int how */
    if ((void *) master->syscallInfo.entry.args[1] != NULL) {
        eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];
    }

    /* const kernel_sigset_t *set */
    if (eq) {
        eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[1], replica,
                            (void *) replica->syscallInfo.entry.args[1],
                            master->syscallInfo.entry.args[3]) == 0;
    }

    if (eq) {
        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;

    } else {

        return -1;
    }
}