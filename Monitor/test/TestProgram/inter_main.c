#define _GNU_SOURCE

#include <sys/uio.h>
#include <sys/prctl.h>
#include <sys/capability.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/uio.h>
#include <stdlib.h>
#include <assert.h>


pid_t makechild() {
    pid_t pid;
    if ((pid = fork()) == 0) {

//        cap_t caps;
//        cap_value_t cap_list[] = {CAP_SYS_PTRACE};
//        if ((caps = cap_get_proc()) == NULL) {
//            perror("cap_get_proc");
//            exit(1);
//        }
//        if (cap_set_flag(caps, CAP_PERMITTED, 1, cap_list, CAP_SET) == -1 ||
//            cap_set_flag(caps, CAP_INHERITABLE, 1, cap_list, CAP_SET) == -1) {
//            perror("cap_set_flag");
//            exit(1);
//        }
//        if (cap_set_proc(caps) == -1) {
//            perror("cap_set_proc");
//            exit(1);
//        }
//        cap_free(caps);
//
//        if (syscall(SYS_prctl, PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, CAP_SYS_PTRACE, 0, 0) == -1) {
//            perror("prctl");
//        }

        // Execute the non-root executable
        char *argv[] = {"/home/mk/CLionProjects/MVEE/TestProgram/inter_child", NULL};
        char *envp[] = {NULL};

        execve(argv[0], argv, envp);

        perror("execve");
        exit(1);
    }
    return pid;
}

int requestSyscallInfo(struct ptrace_syscall_info *i, pid_t p) {
    memset(i, 0, sizeof(struct ptrace_syscall_info));
    long r = ptrace(PTRACE_GET_SYSCALL_INFO, p, sizeof(struct ptrace_syscall_info), i);
    if (r < 0) {
        perror("ptrace");
        return -1;
    }
    return 0;

}

ssize_t readVariantMem(pid_t p, void *localBuf, void *remoteBuf, size_t size) {

    struct iovec local_iov[1];
    local_iov[0].iov_base = localBuf;
    local_iov[0].iov_len = size;
    struct iovec remote_iov_master[1];
    remote_iov_master[0].iov_base = remoteBuf;
    remote_iov_master[0].iov_len = size;
    return process_vm_readv(p, local_iov, 1, remote_iov_master, 1, 0);
}

void printSyscallDebug(struct ptrace_syscall_info *i, char* t) {

    if (i->op == PTRACE_SYSCALL_INFO_ENTRY) {
        printf("[%s] Syscall: ENT (%llu)\n", t, i->entry.nr);
    }
    else if (i->op == PTRACE_SYSCALL_INFO_EXIT) {
        printf("[%s] Syscall: EXIT =%llu\n", t, i->exit.rval);
    }
    else {
        printf("[%s] info (%d) error\n", t, i->op);
    }
}


int main() {

    pid_t mainPid = getpid();
    printf("Main PID: %d\n", mainPid);

    pid_t c1 = makechild();
    waitpid(c1, NULL, __WALL);
    ptrace(PTRACE_SETOPTIONS, c1, 0, PTRACE_O_EXITKILL);
    ptrace(PTRACE_SETOPTIONS, c1, 0, PTRACE_O_TRACESYSGOOD);
    printf("Child 1 PID: %d\n", c1);
    pid_t c2 = makechild();
    waitpid(c2, NULL, __WALL);
    ptrace(PTRACE_SETOPTIONS, c2, 0, PTRACE_O_EXITKILL);
    ptrace(PTRACE_SETOPTIONS, c2, 0, PTRACE_O_TRACESYSGOOD);
    printf("Child 2 PID: %d\n", c2);


    struct ptrace_syscall_info i1;
    ptrace(PTRACE_SYSCALL, c1, 0, 0); //execve
    waitpid(c1, NULL, __WALL);
    requestSyscallInfo(&i1, c1);
    printSyscallDebug( &i1, "M");

    struct ptrace_syscall_info i2;
    ptrace(PTRACE_SYSCALL, c2, 0, 0); //execve
    waitpid(c2, NULL, __WALL);
    requestSyscallInfo(&i2, c2);
    printSyscallDebug(&i2, "R");

    ptrace(PTRACE_SYSCALL, c1, 0, 0); //getrandom entry
    waitpid(c1, NULL, __WALL);
    requestSyscallInfo(&i1, c1);
    printSyscallDebug(&i1, "M");

//    assert(i1.op == PTRACE_SYSCALL_INFO_ENTRY && i1.entry.nr == SYS_getrandom);
    void* c1buf = (void*) i1.entry.args[0];
    // poke

    ptrace(PTRACE_SYSCALL, c2, 0, 0); //getrandom entry
    waitpid(c2, NULL, __WALL);
    requestSyscallInfo(&i2, c2);
    printSyscallDebug(&i2, "R");
//    assert(i2.op == PTRACE_SYSCALL_INFO_ENTRY && i2.entry.nr == SYS_getrandom);
    void* c2buf = (void*) i2.entry.args[0];

    ptrace(PTRACE_SYSCALL, c1, 0, 0); //getrandom exit
    waitpid(c1, NULL, __WALL);
    char c1bufContent[10];
    long r = readVariantMem(c1, c1bufContent, c1buf, 10);
    if (r == -1) {
        perror("readVariantMem");
    }
    // hexdump
    printf("c1bufContent: ");
    for (int i = 0; i < 10; i++) {
        printf("%02x ", c1bufContent[i]);
    }
    printf("\n");

    // Construct process_vm_readv arguments for c2 to read from c1's buffer into its own



    getchar();

    return 0;
}


//    struct __user_cap_header_struct cap_header;
//    struct __user_cap_data_struct cap_data[2];
//
//    cap_header.version = _LINUX_CAPABILITY_VERSION_3;
//    cap_header.pid = 0;
//
//
//    if (syscall(SYS_capget, &cap_header, cap_data) == -1) {
//        perror("capget");
//        exit(1);
//    }
//
//    /* Set permitted and inhertiable capability -- required for ambient */
//    cap_data[0].permitted |= 1 << CAP_SYS_PTRACE;
//    cap_data[0].inheritable |= 1 << CAP_SYS_PTRACE;
//    cap_data[0].effective |= 1 << CAP_SYS_PTRACE;
//
//
//    if (syscall(SYS_capset, &cap_header, cap_data) == -1) {
//        perror("capset");
//        exit(1);
//    }
//
//    /* Ambient capability needed to preserve across execve to a non-root binary */
//    if (syscall(SYS_prctl, PR_CAP_AMBIENT, PR_CAP_AMBIENT_RAISE, CAP_SYS_PTRACE, 0, 0) == -1) {
//        perror("prctl");
//        exit(1);
//    }