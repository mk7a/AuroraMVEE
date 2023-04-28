#include <sys/poll.h>
#include "poll.h"

int execSyscall_poll(variant *master, variant *replica) {
    /*
     * int poll(struct pollfd *fds, nfds_t nfds, int timeout);
     */

    int eq = 1;

    /* nfds_t nfds */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* int timeout */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    /* struct pollfd *fds */
    void* masterFds = (void*) master->syscallInfo.entry.args[0];
    void* replicaFds = (void*) replica->syscallInfo.entry.args[0];
    size_t fdsSize = master->syscallInfo.entry.args[1] * sizeof(struct pollfd);
    if (eq) {
        eq &= variantMemCmp(master, masterFds,
                            replica, replicaFds,
                            fdsSize) == 0;
    }

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

    /* Copy results back to replica */
    int ret = transferVariantMem(master, masterFds, replica, replicaFds,
                                 fdsSize);
    if (ret != 0) return -1;

    /* Set return value */
    setSyscallReturnValue(replica, master->syscallInfo.exit.rval);

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}