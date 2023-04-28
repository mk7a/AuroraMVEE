#include <unistd.h>
#include "accept4.h"
#include "../debug.h"

int execSyscall_accept4(variant *master, variant *replica) {
    /*
     *  int accept4(int sockfd, struct sockaddr *restrict addr,
                  socklen_t *restrict addrlen, int flags);
     */

    int eq = 1;

    /* int sockfd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* int flags */
    eq &= master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3];


    /* struct sockaddr *restrict addr */
    void* masterAddr = (void*) master->syscallInfo.entry.args[1];
    void* replicaAddr = (void*) replica->syscallInfo.entry.args[1];
    /* pointer equivalence not required */


    /* socklen_t *restrict addrlen */
    eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[2],
                        replica, (void *) replica->syscallInfo.entry.args[2],
                        sizeof(socklen_t)) == 0;
    void* masterAddrLenAddr = (void *) master->syscallInfo.entry.args[2];

    if (!eq) {
        return -1;
    }

    /* Run in master */
    resumeVariantFromWait(master);
    if (waitVariant(master) == -1) return -1;
    requestSyscallInfo(master);

    /* NOP in replica */
    setNopSyscall(replica);
    resumeVariantFromWait(replica);
    if (waitVariant(replica) == -1) return -1;

    /* Replicate struct fill */
    if (masterAddr != NULL) {
        socklen_t masterAddrLen;
        readVariantMem(master, &masterAddrLen, masterAddrLenAddr, sizeof(socklen_t));
        transferVariantMem(master, masterAddr, replica, replicaAddr, masterAddrLen);
    }
    /* Replicate return value */
    setSyscallReturnValue(replica, master->syscallInfo.exit.rval);

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}