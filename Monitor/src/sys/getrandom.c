#include "getrandom.h"


int execSyscall_getrandom(variant *master, variant *replica) {
    /*
     * ssize_t getrandom(void *buf, size_t buflen, unsigned int flags);
     */

    int eq = 1;

    /* void *buf */
    /* Equivalence not required */
    void *masterBuf = (void *) master->syscallInfo.entry.args[0];
    void *replicaBuf = (void *) replica->syscallInfo.entry.args[0];

    /* size_t buflen */
    eq &= (master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1]);

    /* unsigned int flags */
    eq &= (master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2]);

    if (eq) {

        /* Complete in master */
        resumeVariantFromWait(master);
        if (waitVariant(master) != 0) return -1;
        requestSyscallInfo(master);
        ssize_t bytesWritten = master->syscallInfo.exit.rval;


        /* Copy read data to replica */
        if (bytesWritten != -1) {

            int ret = transferVariantMem(master, masterBuf,
                                         replica, replicaBuf, bytesWritten);
            if (ret != 0) return -1;

        }

        setNopSyscall(replica);
        resumeVariantFromWait(replica);
        if (waitVariant(replica) != 0) return -1;
        /* Set return value in replica*/
        setSyscallReturnValue(replica, bytesWritten);

        /* Resume from syscall exit */
        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;

    } else {

        return -1;
    }
}