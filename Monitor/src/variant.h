#ifndef MVEE_VARIANT_H
#define MVEE_VARIANT_H

#include "compile_options.h"

#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>

/* Variant synchronization states */
#define VSTATE_RUNNING 0
#define VSTATE_WAIT 1

/* Variant types */
#define VTYPE_master 1
#define VTYPE_replica 0

typedef struct {
    pid_t pid;
    int state;
    int type;
    unsigned long long syscallNum;
    struct ptrace_syscall_info syscallInfo;
} variant;

int requestSyscallInfo(variant *v);

void printSyscallDebug(variant *v);

int initVariant(variant *v, int vtype, char *argv[], long core);

int waitVariant(variant *v);

int checkVariantExit(variant *v, const int *status);

int resumeVariantFromWait(variant *v);

int resumeVariantFromExit(variant *v);

int runMasterReplicateRet(variant *master, variant *replica);

int runBothCopyMasterRet(variant *master, variant *replica);

int setSyscall(variant *v, int call);

int setNopSyscall(variant *v);

int setSyscallReturnValue(variant *v, int64_t returnValue);

ssize_t readVariantMem(variant *v, void *localBuf, void *remoteBuf, size_t size);

ssize_t writeVariantMem(variant *v, void *localBuf, void *remoteBuf, size_t size);

int transferVariantMem(variant *sourceV, void *sourceVBuf, variant *destV, void *destVBuf, size_t size);

int variantStrCmp(variant *v1, void *v1Buf, variant *v2, void *v2Buf);

int variantMemCmp(variant *v1, void *v1Buf, variant *v2, void *v2Buf, size_t size);

#ifdef USE_DIRECT_TRANSFER
int transferVariantMemDirect(variant *sourceV, void *sourceVBuf, variant *destV, void *destVBuf, size_t size);

int variantChangePtraceCap(variant *v, struct user_regs_struct *regs, int newState);


#endif

#endif //MVEE_VARIANT_H
