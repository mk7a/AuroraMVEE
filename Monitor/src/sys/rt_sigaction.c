#include "rt_sigaction.h"
#include <signal.h>
#include <string.h>

int execSyscall_rt_sigaction(variant *master, variant *replica) {
    /*
     * int sigaction(int signum, const struct sigaction *restrict act,
                         struct sigaction *restrict oldact);
     */

    /* int signum */
    if (master->syscallInfo.entry.args[0] != replica->syscallInfo.entry.args[0]) return -1;


    /* const struct sigaction *restrict act
     * addr of handlers will differ.*/

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return 0;
}