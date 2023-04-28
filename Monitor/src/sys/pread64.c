#include "pread64.h"

#include <stddef.h>

int execSyscall_pread64(variant *master, variant *replica) {
    /*
     * ssize_t pread(int fd, void *buf, size_t count, off_t offset);
     */

    int eq = 1;

    /* int fd */
    eq &= (master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0]);

    /* size_t count */
    size_t masterCount = (size_t) master->syscallInfo.entry.args[2];
    eq &= (masterCount == (size_t) replica->syscallInfo.entry.args[2]);

    /* off_t offset */
    eq &= (master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3]);

    /* void *buf */
    /* Equality not required */
    void *masterBuf = (void *) master->syscallInfo.entry.args[1];
    void *replicaBuf = (void *) replica->syscallInfo.entry.args[1];

    if (eq) {
        resumeVariantFromWait(master);
        if (waitVariant(master) != 0) return -1;
        requestSyscallInfo(master);
        ssize_t bytesRead = (ssize_t) master->syscallInfo.exit.rval;

        int ret = transferVariantMem(master, masterBuf, replica, replicaBuf, bytesRead);

        if (ret != 0) return -1;

        setNopSyscall(replica);
        resumeVariantFromWait(replica);
        if (waitVariant(replica) != 0) return -1;
        setSyscallReturnValue(replica, bytesRead);

        resumeVariantFromExit(master);
        resumeVariantFromExit(replica);

        return 0;

    } else {
        return -1;
    }
}