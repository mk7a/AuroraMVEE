#include "listen.h"

int execSyscall_listen(variant *master, variant *replica) {
    /*
     * int listen(int sockfd, int backlog);
     */

    int eq = 1;

    /* int sockfd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* int backlog */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    if (eq) {

        return runMasterReplicateRet(master, replica);
    } else {
        return -1;
    }
}