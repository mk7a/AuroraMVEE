#include "set_tid_address.h"

int execSyscall_setTidAddress(variant *master, variant *replica) {
    /*
     * pid_t syscall(SYS_set_tid_address, int *tidptr);
     */

    void *masterPtr = (void *) master->syscallInfo.entry.args[0];
    void *replicaPtr = (void *) replica->syscallInfo.entry.args[0];

    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    resumeVariantFromWait(replica);
    if (waitVariant(replica) != 0) return -1;

    int tid;
    readVariantMem(master, &tid, masterPtr, sizeof(int));
    writeVariantMem(replica, &tid, replicaPtr, sizeof(int));

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}