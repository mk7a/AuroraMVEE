#include <unistd.h>
#include "getsockname.h"

int execSyscall_getsockname(variant *master, variant *replica) {
    /*
     *        int getsockname(int sockfd, struct sockaddr *restrict addr,
                       socklen_t *restrict addrlen);
     */

    int eq = 1;

    /* int sockfd */
    eq &= (master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0]);

    /* struct sockaddr *restrict addr */
    void* masterBuf = (void*) master->syscallInfo.entry.args[1];
    void* replicaBuf = (void*) replica->syscallInfo.entry.args[1];

    /* socklen_t *restrict addrlen */
    socklen_t masterLen;
    readVariantMem(master, &masterLen,
                   (void*) master->syscallInfo.entry.args[2], sizeof(socklen_t));

    eq &= variantMemCmp(master, (void*) master->syscallInfo.entry.args[2],
                        replica, (void*) replica->syscallInfo.entry.args[2],
                        sizeof(socklen_t)) == 0;
    if (!eq) {
        return -1;
    }

    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);

    setNopSyscall(replica);
    resumeVariantFromWait(replica);
    if (waitVariant(replica) != 0) return -1;

    setSyscallReturnValue(replica, master->syscallInfo.exit.rval);
    if (master->syscallInfo.exit.rval == 0) {
        transferVariantMem(master, masterBuf, replica,
                           replicaBuf, masterLen);
    }

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}