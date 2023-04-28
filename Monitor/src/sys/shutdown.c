#include "shutdown.h"

int execSyscall_shutdown(variant *master, variant *replica) {
    /*
     * int shutdown(int sockfd, int how);
     */

    int eq = 1;

    /* int sockfd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* int how */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    if (eq) {

        resumeVariantFromExit(master);
        resumeVariantFromExit(replica);

        return 0;

    } else {
        return -1;
    }
}