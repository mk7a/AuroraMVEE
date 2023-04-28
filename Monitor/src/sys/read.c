#include "read.h"
#include "../debug.h"

#include <string.h>
#include <stdio.h>
#include <sys/user.h>
#include <sys/syscall.h>

int execSyscall_read(variant *master, variant *replica) {
    int eq = 1;

    /* int fd */
    int masterFd = (int) master->syscallInfo.entry.args[0];
    eq &= masterFd == replica->syscallInfo.entry.args[0];

    /* void *buf */
    void *masterBuf = (void *) master->syscallInfo.entry.args[1];
    void *replicaBuf = (void *) replica->syscallInfo.entry.args[1];
    /* Equality not required */

    /* size_t count */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    if (!eq) {
        return -1;
    }


    /* Complete read in master */
    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);
    ssize_t bytesRead = master->syscallInfo.exit.rval;


    /* Copy read data to replica */
    if (bytesRead != -1) {

#ifdef USE_DIRECT_TRANSFER
        int ret = transferVariantMemDirect(master, masterBuf,
                                           replica, replicaBuf, bytesRead);
#else
        int ret = transferVariantMem(master, masterBuf, replica, replicaBuf, bytesRead);
#endif
        if (ret != 0) return -1;
    }

    /* lseek in replica to update file position */
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, replica->pid, NULL, &regs);
    regs.rdi = masterFd; //unsigned int fd
    regs.rsi = bytesRead; //off_t offset
    regs.rdx = SEEK_CUR; //unsigned int whence
    ptrace(PTRACE_SETREGS, replica->pid, NULL, &regs);
    setSyscall(replica, SYS_lseek);

    resumeVariantFromWait(replica);
    if (waitVariant(replica) != 0) return -1;

#ifndef USE_DIRECT_TRANSFER
    /* Set return value in replica*/
    setSyscallReturnValue(replica, bytesRead);
#endif

    /* Resume from syscall exit */
    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}
