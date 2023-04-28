#include <unistd.h>
#include "bind.h"
#include "../debug.h"

int execSyscall_bind(variant *master, variant *replica) {
    /*
     * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
     */

    int eq = 1;

    /* int sockfd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];


    /* socklen_t addrlen */
    socklen_t addrlen_master = master->syscallInfo.entry.args[2];
    eq &= addrlen_master == replica->syscallInfo.entry.args[2];


    /* const struct sockaddr *addr */
    eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[1],
                        replica, (void *) replica->syscallInfo.entry.args[1],
                        addrlen_master) == 0;


    if (eq) {

        return runMasterReplicateRet(master, replica);

    } else {
        return -1;
    }
}