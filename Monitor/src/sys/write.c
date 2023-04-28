#include "write.h"
#include "../debug.h"

#include <stdio.h>
#include <sys/syscall.h>
#include <sys/user.h>

int execSyscall_write(variant *master, variant *replica) {
    /*
     * ssize_t write(int fd, const void *buf, size_t count)
     */

    int eq = 1;

    /* int fd */
    int masterFd = (int) master->syscallInfo.entry.args[0];
    eq &= (masterFd == replica->syscallInfo.entry.args[0]);

    /* size_t count */
    size_t masterCount = (size_t) master->syscallInfo.entry.args[2];
    eq &= (masterCount == (size_t) replica->syscallInfo.entry.args[2]);

    /* void *buf */
    if (eq) {
        eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[1], replica,
                            (void *) replica->syscallInfo.entry.args[1], masterCount) == 0;
    }

    if (!eq) {
        return -1;
    }

    /* Complete write in master */
    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);
    ssize_t bytesWritten = master->syscallInfo.exit.rval;

    /* lseek in replica to update file position */
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, replica->pid, NULL, &regs);
    regs.rdi = masterFd; //unsigned int fd
    regs.rsi = bytesWritten; //off_t offset
    regs.rdx = SEEK_CUR; //unsigned int whence
    ptrace(PTRACE_SETREGS, replica->pid, NULL, &regs);
    setSyscall(master, SYS_lseek);

    resumeVariantFromWait(replica);
    if (waitVariant(replica) != 0) return -1;
    /* Set return value in replica*/
    setSyscallReturnValue(replica, bytesWritten);

    /* Resume from syscall exit */
    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}