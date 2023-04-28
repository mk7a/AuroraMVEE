#include "variant.h"
#include "sys/syscall_util.h"
#include "debug.h"
#include "compile_options.h"

#include <sys/reg.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sched.h>
#include <sys/capability.h>
#include <linux/prctl.h>
#include <errno.h>
#include <assert.h>

/* General purpose buffers for variant memory operations */
#define BUF_SIZE 10000000
char bufA[BUF_SIZE];
char bufB[BUF_SIZE];

/*
 * Forks new variant and sets up ptrace.
 * PID of new variant is stored in *pid.
 */
void createVariant(pid_t *pid, char *argv[], long core) {

    if ((*pid = fork()) == 0) {

#ifdef USE_DIRECT_TRANSFER
        cap_t caps;
        cap_value_t cap_list[] = {CAP_SYS_PTRACE};
        if ((caps = cap_get_proc()) == NULL) {
            perror("cap_get_proc");
            exit(EXIT_FAILURE);
        }
        if (cap_set_flag(caps, CAP_PERMITTED, 1, cap_list, CAP_SET) == -1 ||
            cap_set_flag(caps, CAP_INHERITABLE, 1, cap_list, CAP_SET) == -1) {
            perror("cap_set_flag");
            exit(EXIT_FAILURE);
        }
        if (cap_set_proc(caps) == -1) {
            perror("cap_set_proc");
            exit(EXIT_FAILURE);
        }
        cap_free(caps);
        if (syscall(SYS_prctl, PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, CAP_SYS_PTRACE, 0, 0) == -1) {
            perror("prctl");
        }
#endif

        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(core, &mask);
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &mask) == -1) {
            perror("sched_setaffinity");
            exit(EXIT_FAILURE);
        }

        ptrace(PTRACE_TRACEME, 0, 0, 0); /* Gives parent ptrace access */
        execvpe(argv[1], argv + 1, environ); /* argv + 1: skip first argument (path to MVEE program) */
        exit(EXIT_FAILURE);
    }
}

/*
 * Configures child process for ptrace.
 * PTRACE_O_EXITKILL kills child when parent dies.
 * PTRACE_O_TRACESYSGOOD for enhanced syscall tracing
 */
void configureChildProc(pid_t pid) {
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_EXITKILL);
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
}

void printSyscallDebug(variant *v) {
    int isEntry = v->syscallInfo.op == PTRACE_SYSCALL_INFO_ENTRY;

    if (isEntry) {
        debug_printf(stderr, "%s Syscall: ENT \t%llu \t>>%s<<\n",
                     v->type == VTYPE_master ? "[M]" : "{R}",
                     v->syscallInfo.entry.nr,
                     getSyscallName(v->syscallInfo.entry.nr));
    } else {
        debug_printf(stderr, "%s Syscall: EXIT \t%lld\n",
                     v->type == VTYPE_master ? "[M]" : "{R}", v->syscallInfo.exit.rval);
    }
}

/*
 * Create and configure variant
 */
int initVariant(variant *v, int vtype, char *argv[], long core) {

    v->syscallNum = 0;
    v->type = vtype;
    createVariant(&v->pid, argv, core);
    waitpid(v->pid, NULL, __WALL); /* __WALL: waits for child regardless of its state */
    configureChildProc(v->pid);
    return 0;
}

/*
 * Wait for variant to reach syscall entry or exit
 * Returns 0 if successful, -1 if variant exited
 */
int waitVariant(variant *v) {
    int status;
    waitpid(v->pid, &status, __WALL);
    return checkVariantExit(v, &status);
}

/*
 * Check if variant exited -
 * returns 0 if variant is still running, -1 if variant exited.
 * If exited, prints exit status.
 */
int checkVariantExit(variant *v, const int *status) {

    if (WIFEXITED(*status)) {
        debug_printf(stderr, "%s exited with status %d\n",
                     v->type == VTYPE_master ? "master" : "replica", WEXITSTATUS(*status));
        return -1;
    }

    if (WIFSIGNALED(*status)) {
        debug_printf(stderr, "%s exited with signal %d\n",
                     v->type == VTYPE_master ? "master" : "replica", WTERMSIG(*status));
        return -1;
    }

    return 0;
}


/*
 * Resume variant and mark as running
 */
int resumeVariantFromWait(variant *v) {
    debug_printf(stderr, "\t %s resume from wait [%llu]\n",
                 v->type == VTYPE_master ? "[M]" : "{R}", v->syscallNum);
    ptrace(PTRACE_SYSCALL, v->pid, 0, 0);
    v->state = VSTATE_RUNNING;
    return 0;
}

/*
 * Resume variant and increment syscall number
 */
int resumeVariantFromExit(variant *v) {
    debug_printf(stderr, "\t %s resume from exit [%llu] = %lld\n",
                 v->type == VTYPE_master ? "[M]" : "{R}", v->syscallNum, v->syscallInfo.exit.rval);
    ptrace(PTRACE_SYSCALL, v->pid, 0, 0);
    v->syscallNum++;
    return 0;
}

/*
 * Runs master syscall from entry wait, runs replica through NOP syscall,
 * replicates master's return value to replica, and finally resumes both.
 */
int runMasterReplicateRet(variant *master, variant *replica) {

    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);
    int64_t masterRVal = (int) master->syscallInfo.exit.rval;

    setNopSyscall(replica);
    resumeVariantFromWait(replica);
    if (waitVariant(replica) != 0) return -1;
    setSyscallReturnValue(replica, masterRVal);

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}

/*
 * Runs syscall in both variants and replicates master return value to replica.
 */
int runBothCopyMasterRet(variant *master, variant *replica) {

    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);
    int64_t masterRVal = (int) master->syscallInfo.exit.rval;

    resumeVariantFromWait(replica);
    if (waitVariant(replica) != 0) return -1;
    setSyscallReturnValue(replica, masterRVal);

    resumeVariantFromExit(master);
    resumeVariantFromExit(replica);

    return 0;
}


int setSyscall(variant *v, int call) {
    ptrace(PTRACE_POKEUSER, v->pid, 8 * ORIG_RAX, call);
}

int setNopSyscall(variant *v) {
    setSyscall(v, SYS_getpid);
    return 0;
}


int setSyscallReturnValue(variant *v, int64_t returnValue) {
    ptrace(PTRACE_POKEUSER, v->pid, 8 * RAX, returnValue);
    return 0;
}

/*
 * Read memory from remote variant buffer into local monitor buffer
 */
ssize_t readVariantMem(variant *v, void *localBuf, void *remoteBuf, size_t size) {

    if (size > BUF_SIZE) {
        debug_printf(stderr, "readVariantMem %zu size exceeds buffer size\n", size);
        return -1;
    }

#ifdef USE_PTRACE_TRANSFER
    size_t bytesRemaining = size;
    ssize_t bytesTransferred = 0;
    while (bytesRemaining > 0) {
        size_t bytesToTransfer = bytesRemaining > sizeof(long) ? sizeof(long) : bytesRemaining;

        ((long *) localBuf)[bytesTransferred / sizeof(long)] =
                (long) ptrace(PTRACE_PEEKDATA, v->pid, remoteBuf + bytesTransferred, 0);

        bytesRemaining -= bytesToTransfer;
        bytesTransferred += (ssize_t) bytesToTransfer;
    }
    return bytesTransferred;
#else
    struct iovec local_iov[1];
    local_iov[0].iov_base = localBuf;
    local_iov[0].iov_len = size;
    struct iovec remote_iov_master[1];
    remote_iov_master[0].iov_base = remoteBuf;
    remote_iov_master[0].iov_len = size;
    return process_vm_readv(v->pid, local_iov, 1, remote_iov_master, 1, 0);
#endif
}

/*
 * Write memory from local monitor buffer into remote variant buffer
 */
ssize_t writeVariantMem(variant *v, void *localBuf, void *remoteBuf, size_t size) {

#ifdef USE_PTRACE_TRANSFER
    ssize_t bytesRemaining = (ssize_t) size;
    ssize_t bytesTransferred = 0;
    if (size < sizeof(long)) {

        /* Smallest transfer size is 1 word, for writes smaller than this
         * Must get existing data beyond buffer to prevent POKE overwrite erasing it*/
        long data = (long) ptrace(PTRACE_PEEKDATA, v->pid, remoteBuf, 0);
        /* Apply effect of writing into buffer */
        memcpy(&data, localBuf, size);
        ptrace(PTRACE_POKEDATA, v->pid, remoteBuf, data);
        return (ssize_t) size;

    } else {
        while (bytesRemaining > 0) {

            if (bytesRemaining >= sizeof(long)) {  /* Write full word */
                ptrace(PTRACE_POKEDATA, v->pid, remoteBuf + bytesTransferred,
                       ((long *) localBuf)[bytesTransferred / sizeof(long)]);

                bytesRemaining -= sizeof(long);
                bytesTransferred += sizeof(long);

            } else { /* Write last word of data, counting from last byte */
                ptrace(PTRACE_POKEDATA, v->pid, remoteBuf + size - sizeof(long),
                       ((long *) localBuf)[size - sizeof(long)]);
                bytesTransferred += bytesRemaining;
                bytesRemaining = 0;
            }
        }
        return bytesTransferred;
    }
#endif
    struct iovec local_iov[1];
    local_iov[0].iov_base = localBuf;
    local_iov[0].iov_len = size;
    struct iovec remote_iov_master[1];
    remote_iov_master[0].iov_base = remoteBuf;
    remote_iov_master[0].iov_len = size;
    return process_vm_writev(v->pid, local_iov, 1, remote_iov_master, 1, 0);
}

int requestSyscallInfo(variant *v) {
    memset(&v->syscallInfo, 0, sizeof(struct ptrace_syscall_info));
    ptrace(PTRACE_GET_SYSCALL_INFO, v->pid, sizeof(struct ptrace_syscall_info), &v->syscallInfo);
    return 0;

}

/*
 * Transfer memory between variant buffers, through the monitor
 */
int transferVariantMem(variant *sourceV, void *sourceVBuf, variant *destV, void *destVBuf, size_t size) {

    if (size > BUF_SIZE) {
        debug_printf(stderr, "transferVariantMem %zu size exceeds buffer size\n", size);
        return -1;
    }

    ssize_t r = readVariantMem(sourceV, bufA, sourceVBuf, size);

    if (r != size) {
        debug_printf(stderr, "transferVariantMem read failed\n");
        return -1;
    }

    ssize_t w = writeVariantMem(destV, bufA, destVBuf, size);

    if (w != size) {
        debug_printf(stderr, "transferVariantMem write failed. Wrote %zd out of %zu\n", w, size);
        return -1;
    }

    return 0;
}



/*
 * Read from remote variant buffers into local buffers and perform string compare
 * returns:
 *  -1 on error
 *  0 on equal
 *  1 on not equal
 */
int variantStrCmp(variant *v1, void *v1Buf, variant *v2, void *v2Buf) {

    memset(bufA, 0, BUF_SIZE);
    memset(bufB, 0, BUF_SIZE);

    size_t curSize = 100;
    size_t stringLen;

    while (curSize < BUF_SIZE) {

        readVariantMem(v1, bufA, v1Buf, curSize);
        stringLen = strnlen(bufA, curSize);

        if (stringLen == BUF_SIZE) {
            debug_printf(stderr, "variantStrCmp string exceeds buffer size\n");
            return -1;

        } else if (stringLen == curSize) {
            debug_printf(stderr, "variantStrCmp string too long, increasing...\n");
            curSize += 100;

        } else {
            debug_printf(stderr, "variantStrCmp max buffer size exceeded\n");
            return -1;
        }
    }

    readVariantMem(v2, bufB, v2Buf, curSize);
    int cmpRes = strncmp(bufA, bufB, stringLen);

    if (cmpRes != 0) {
        debug_printf(stderr, "variantStrCmp strings not equal: \n\t%s \n\t%s\n", bufA, bufB);
    }

    return cmpRes == 0 ? 0 : 1;
}

/*
 * Read from remote variant buffers into local buffers and perform memory compare
 */
int variantMemCmp(variant *v1, void *v1Buf, variant *v2, void *v2Buf, size_t size) {

    if (size > BUF_SIZE) {
        debug_printf(stderr, "variantMemCmp size exceeds buffer size\n");
        return -1;
    }

    readVariantMem(v1, bufA, v1Buf, size);
    readVariantMem(v2, bufB, v2Buf, size);

    return memcmp(bufA, bufB, size) == 0 ? 0 : 1;
}


#ifdef USE_DIRECT_TRANSFER

int variantChangePtraceCap(variant *v, struct user_regs_struct *regs, int newState) {

    ptrace(PTRACE_GETREGS, v->pid, 0, regs);
    regs->rip -= 2;
    ptrace(PTRACE_SETREGS, v->pid, 0, regs);

    ptrace(PTRACE_SYSCALL, v->pid, 0, 0);
    if (waitVariant(v) != 0) return -1; // now at entry

    struct __user_cap_header_struct capheader;
    capheader.pid = v->pid;
    capheader.version = _LINUX_CAPABILITY_VERSION_3;
    struct __user_cap_data_struct capdata[2];
    if (capget(&capheader, capdata) == -1) {
        perror("capget");
        return -1;
    }

    /* Modify CAP_SYS_PTRACE in effective */
    if (newState == 0) {
        capdata[CAP_TO_INDEX(CAP_SYS_PTRACE)].effective &= ~CAP_TO_MASK(CAP_SYS_PTRACE);
    } else if (newState == 1) {
        capdata[CAP_TO_INDEX(CAP_SYS_PTRACE)].effective |= CAP_TO_MASK(CAP_SYS_PTRACE);
    } else {
        assert(0);
    }

    /* Transfer capheader and capdata to variant's stack after RSP */
    void* headerAddr = (void*) v->syscallInfo.stack_pointer - sizeof(struct __user_cap_header_struct);
    void* dataAddr = headerAddr - sizeof(struct __user_cap_data_struct) * 2;
    writeVariantMem(v, &capheader, headerAddr, sizeof(struct __user_cap_header_struct));
    writeVariantMem(v, &capdata, dataAddr, sizeof(struct __user_cap_data_struct) * 2);

    /* Setup syscall arguments
     * int syscall(SYS_capset, cap_user_header_t hdrp,
                   const cap_user_data_t datap);*/
    ptrace(PTRACE_GETREGS, v->pid, 0, regs);
    regs->orig_rax = SYS_capset;
    regs->rdi = (unsigned long) headerAddr;
    regs->rsi = (unsigned long) dataAddr;
    ptrace(PTRACE_SETREGS, v->pid, 0, regs);
//    setSyscall(v, SYS_capset);

    /* Execute syscall */
    ptrace(PTRACE_SYSCALL, v->pid, 0, 0);
    if (waitpid(v->pid, NULL, 0) == -1) {
        perror("CAPDROP: waitpid");
        return -1;
    }
    int retCapset = (int)ptrace(PTRACE_PEEKUSER, v->pid, sizeof(long) *RAX, 0);
    debug_printf(stderr, "variantChangePtraceCap: capset returned %d\n", retCapset);

    return 0;
}

int transferVariantMemDirect(variant *sourceV, void *sourceVBuf, variant *destV, void *destVBuf, size_t size) {

    struct user_regs_struct regs;
    if (variantChangePtraceCap(destV, &regs, 1) != 0) return -1;

    ptrace(PTRACE_GETREGS, destV->pid, 0, regs);
    regs.rip -= 2;
    ptrace(PTRACE_SETREGS, destV->pid, 0, regs);
    /* Get to syscall entry.*/
    ptrace(PTRACE_SYSCALL, destV->pid, 0, 0);
    if (waitVariant(destV) != 0) {
        return -1;
    }

    /* Create transfer details structs */
    struct iovec local_iov[1];
    local_iov[0].iov_base = destVBuf;
    local_iov[0].iov_len = size;

    struct iovec remote_iov_master[1];
    remote_iov_master[0].iov_base = sourceVBuf;
    remote_iov_master[0].iov_len = size;

    /* Transfer to dest variant stack */
    void* localAddr = (void*) destV->syscallInfo.stack_pointer - sizeof(struct iovec);
    void* remoteAddr = (void*) sourceV->syscallInfo.stack_pointer - sizeof(struct iovec) * 2;
    writeVariantMem(destV, &local_iov, localAddr, sizeof(struct iovec));
    writeVariantMem(destV, &remote_iov_master, remoteAddr, sizeof(struct iovec));

    /* Setup syscall arguments
     *        ssize_t process_vm_readv(pid_t pid,
                              const struct iovec *local_iov,
                              unsigned long liovcnt,
                              const struct iovec *remote_iov,
                              unsigned long riovcnt,
                              unsigned long flags);*/

    ptrace(PTRACE_GETREGS, destV->pid, 0, &regs);
    regs.orig_rax = SYS_process_vm_readv;
    regs.rdi = sourceV->pid;
    regs.rsi = (unsigned long) localAddr;
    regs.rdx = 1;
    regs.r10 = (unsigned long) remoteAddr;
    regs.r8 = 1;
    regs.r9 = 0;
    ptrace(PTRACE_SETREGS, destV->pid, 0, &regs);


    /* Execute syscall */
    ptrace(PTRACE_SYSCALL, destV->pid, 0, 0);
    if (waitpid(destV->pid, NULL, 0) == -1) {
        perror("transferVariantMemDirect: waitpid");
        return -1;
    }

    requestSyscallInfo(destV);
    if (destV->syscallInfo.exit.rval == -1) {
        perror("transferVariantMemDirect: process_vm_readv");
        return -1;
    } else if (destV->syscallInfo.exit.rval != size) {
        debug_printf(stderr, "transferVariantMemDirect: process_vm_readv read size mismatch\n");
        return -1;
    }

    if (variantChangePtraceCap(destV, &regs, 0) !=0) return -1;

    return 0;
}
#endif