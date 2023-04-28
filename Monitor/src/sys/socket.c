#include "socket.h"

int execSyscall_socket(variant *master, variant *replica) {
    /*
     * int socket(int domain, int type, int protocol);
     */

    int eq = 1;

    /* int domain */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* int type */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* int protocol */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    if (eq) {
        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;
    } else {
        return -1;
    }
}