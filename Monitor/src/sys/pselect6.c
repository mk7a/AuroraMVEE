#include "pselect6.h"
#include "../debug.h"


int execSyscall_pselect6(variant *master, variant *replica) {
    /*
     * int pselect(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
     *             fd_set *restrict exceptfds, const struct timespec *restrict timeout,
     *             const sigset_t *restrict sigmask);
     */

    int eq = 1;

    /* int nfds */
    int masterNfds = (int) master->syscallInfo.entry.args[0];
    eq &= masterNfds == (int) replica->syscallInfo.entry.args[0];

    /* fd_set *restrict readfds */
    void* masterReadFds = (void*) master->syscallInfo.entry.args[1];
    void* replicaReadFds = (void*) replica->syscallInfo.entry.args[1];
    eq &= variantMemCmp(master, masterReadFds, replica, replicaReadFds,
                        sizeof(fd_set)) == 0;

    /* fd_set *restrict writefds */
    void* masterWriteFds = (void*) master->syscallInfo.entry.args[2];
    void* replicaWriteFds = (void*) replica->syscallInfo.entry.args[2];
    eq &= variantMemCmp(master, masterWriteFds, replica, replicaWriteFds,
                        sizeof(fd_set)) == 0;

    /* fd_set *restrict exceptfds */
    void* masterExceptFds = (void*) master->syscallInfo.entry.args[3];
    void* replicaExceptFds = (void*) replica->syscallInfo.entry.args[3];
    eq &= variantMemCmp(master, masterExceptFds, replica, replicaExceptFds,
                        sizeof(fd_set)) == 0;

    /* const struct timespec *restrict timeout */
    eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[4],
                        replica, (void *) replica->syscallInfo.entry.args[4],
                        sizeof(struct timespec)) == 0;

    /* const sigset_t *restrict sigmask */
    eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[5],
                        replica, (void *) replica->syscallInfo.entry.args[5],
                        sizeof(sigset_t)) == 0;

    if (!eq) return -1;

    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);
    int masterRVal = (int) master->syscallInfo.exit.rval;

    setNopSyscall(replica);
    if (waitVariant(replica) != 0) return -1;
    setSyscallReturnValue(replica, masterRVal);

    if (masterRVal > 0) {
        transferVariantMem(master, masterReadFds, replica, replicaReadFds,
                           sizeof(fd_set));
        transferVariantMem(master, masterWriteFds, replica, replicaWriteFds,
                           sizeof(fd_set));
        transferVariantMem(master, masterExceptFds, replica, replicaExceptFds,
                           sizeof(fd_set));
    }

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}
