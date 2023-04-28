#include "newfstatat.h"
#include "../debug.h"

#include <sys/stat.h>
#include <sys/uio.h>
#include <string.h>
#include <stdio.h>

#define NEWFSTATAT_BUF_SIZE 10000000
char newfstatat_bufA[NEWFSTATAT_BUF_SIZE];
char newfstatat_bufB[NEWFSTATAT_BUF_SIZE];

int execSyscall_newfstatat(variant *master, variant *replica) {
    /*
     * int fstatat(int dirfd, const char *restrict pathname,
                struct stat *restrict statbuf, int flags)
     */

    int eq = 1;

    /* int dirfd */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];


    /* int path */
    if (eq) {

        size_t curSize = 1;
        size_t stringLen;

        while (curSize < NEWFSTATAT_BUF_SIZE) {

            readVariantMem(master, newfstatat_bufA, (void *) master->syscallInfo.entry.args[1], curSize);
            stringLen = strnlen(newfstatat_bufA, curSize);

            if (stringLen == NEWFSTATAT_BUF_SIZE) {
                debug_printf(stderr, "newstatat variantStrCmp string exceeds buffer size\n");
                return -1;

            } else if (stringLen == curSize) {
                debug_printf(stderr, "newstatat variantStrCmp string too long, increasing...\n");
                curSize += 50;

            } else {
                break;
            }
        }

        readVariantMem(replica, newfstatat_bufB, (void *) replica->syscallInfo.entry.args[1], stringLen);

        eq &= strncmp(newfstatat_bufA, newfstatat_bufB, stringLen) == 0;
    }

    /* int flags */
    eq &= master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3];

    /* Execute in master and replicate struct */
    if (eq) {
        void *masterBuf = (void *) master->syscallInfo.entry.args[2];
        void *replicaBuf = (void *) replica->syscallInfo.entry.args[2];

        resumeVariantFromWait(master);
        if (waitVariant(master) != 0) return -1;
        requestSyscallInfo(master);
        int masterRet = (int) master->syscallInfo.exit.rval;

        int ret = transferVariantMem(master, masterBuf, replica, replicaBuf, sizeof(struct stat));
        if (ret != 0) return -1;

        setNopSyscall(replica);
        resumeVariantFromWait(replica);
        if (waitVariant(replica) != 0) return -1;
        setSyscallReturnValue(replica, masterRet);

        resumeVariantFromExit(master);
        resumeVariantFromExit(replica);

        return 0;

    } else {
        return -1;
    }
}