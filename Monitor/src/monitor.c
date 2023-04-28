#include "monitor.h"

#include "variant.h"
#include "sys/syscall_executors.h"
#include "debug.h"
#include "compile_options.h"

#include <sys/syscall.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include <linux/ptrace.h>
#include <sys/wait.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <sys/user.h>
#include <sys/reg.h>



#ifdef USE_DIRECT_TRANSFER

int firstCall = 1;


int dropVariantsCap(variant *v) {
    int originalCall = (int) v->syscallInfo.entry.nr;
    if (originalCall != SYS_brk) {
        fprintf(stderr, "CAPDROP full cover not implemented: initial syscall not 'brk' \n");
        return -1;
    }

    ptrace(PTRACE_SYSCALL, v->pid, 0, 0);
    if (waitVariant(v) != 0) return -1; // now at exit
    requestSyscallInfo(v);
    int ret = (int) v->syscallInfo.exit.rval;

    struct user_regs_struct regs;
    variantChangePtraceCap(v, &regs, 0);

    /* resume execution */
    setSyscallReturnValue(v, ret); // restore brk return value
    resumeVariantFromWait(v);

    return 0;

}

#endif


int processSyscallSync(variant *master, variant *replica) {

    if (master->syscallInfo.op != replica->syscallInfo.op) {
        debug_printf(stderr, "Syscall op mismatch\n");
        return -1;
    }

    if (master->syscallInfo.entry.nr != replica->syscallInfo.entry.nr) {
        debug_printf(stderr, "Syscall numbers mismatch\n");
        return -1;
    }

#ifdef USE_DIRECT_TRANSFER
    if (firstCall) {
        firstCall = 0;
        dropVariantsCap(master);
        dropVariantsCap(replica);
        debug_printf(stderr, "Dropped capabilities\n");
        return 0;
    }
#endif

    int execResult = 0;

    switch (master->syscallInfo.entry.nr) {

        /* No arguments */
        case SYS_getuid:
        case SYS_geteuid:
        case SYS_getgid:
        case SYS_getegid:
            resumeVariantFromWait(master);
            resumeVariantFromWait(replica);
            break;

            /* Find syscall executor */
        case SYS_brk:
            execResult = execSyscall_brk(master, replica);
            break;
        case SYS_mmap:
            execResult = execSyscall_mmap(master, replica);
            break;
        case SYS_read:
            execResult = execSyscall_read(master, replica);
            break;
        case SYS_arch_prctl:
            execResult = execSyscall_arch_prctl(master, replica);
            break;
        case SYS_openat:
            execResult = execSyscall_openat(master, replica);
            break;
        case SYS_close:
            execResult = execSyscall_close(master, replica);
            break;
        case SYS_write:
            execResult = execSyscall_write(master, replica);
            break;
        case SYS_pread64:
            execResult = execSyscall_pread64(master, replica);
            break;
        case SYS_newfstatat:
            execResult = execSyscall_newfstatat(master, replica);
            break;
        case SYS_access:
            execResult = execSyscall_access(master, replica);
            break;
        case SYS_set_tid_address:
            execResult = execSyscall_setTidAddress(master, replica);
            break;
        case SYS_set_robust_list:
            execResult = execSyscall_setRobustList(master, replica);
            break;
        case SYS_mprotect:
            execResult = execSyscall_mprotect(master, replica);
            break;
        case SYS_munmap:
            execResult = execSyscall_munmap(master, replica);
            break;
        case SYS_rseq:
            execResult = execSyscall_rseq(master, replica);
            break;
        case SYS_fadvise64:
            execResult = execSyscall_fadvise64(master, replica);
            break;
        case SYS_getrandom:
            execResult = execSyscall_getrandom(master, replica);
            break;
        case SYS_exit_group:
            execResult = execSyscall_exit_group(master, replica);
            break;
        case SYS_prlimit64:
            execResult = execSyscall_prlimit64(master, replica);
            break;
        case SYS_rt_sigaction:
            execResult = execSyscall_rt_sigaction(master, replica);
            break;
        case SYS_rt_sigprocmask:
            execResult = execSyscall_rt_sigprocmask(master, replica);
            break;
        case SYS_unlink:
            execResult = execSyscall_unlink(master, replica);
            break;
        case SYS_utimensat:
            execResult = execSyscall_utimensat(master, replica);
            break;
        case SYS_fchown:
            execResult = execSyscall_fchown(master, replica);
            break;
        case SYS_fchmod:
            execResult = execSyscall_fchmod(master, replica);
            break;
        case SYS_lseek:
            execResult = execSyscall_lseek(master, replica);
            break;
        case SYS_getcwd:
            execResult = execSyscall_getcwd(master, replica);
            break;
        case SYS_getpid:
            execResult = execSyscall_getpid(master, replica);
            break;
        case SYS_chdir:
            execResult = execSyscall_chdir(master, replica);
            break;
        case SYS_socket:
            execResult = execSyscall_socket(master, replica);
            break;
        case SYS_setsockopt:
            execResult = execSyscall_setsockopt(master, replica);
            break;
        case SYS_bind:
            execResult = execSyscall_bind(master, replica);
            break;
        case SYS_listen:
            execResult = execSyscall_listen(master, replica);
            break;
        case SYS_accept4:
            execResult = execSyscall_accept4(master, replica);
            break;
        case SYS_poll:
            execResult = execSyscall_poll(master, replica);
            break;
        case SYS_shutdown:
            execResult = execSyscall_shutdown(master, replica);
            break;
        case SYS_getsockname:
            execResult = execSyscall_getsockname(master, replica);
            break;
        case SYS_pselect6:
            execResult = execSyscall_pselect6(master, replica);
            break;

        default:
            debug_printf(stderr, "##ERROR## Syscall %llu not implemented\n",
                         master->syscallInfo.entry.nr);

            return -1;
    }

    return execResult;
}


int processVariantEvent(variant *vSelf, variant *vOther) {

    requestSyscallInfo(vSelf);

    printSyscallDebug(vSelf);

    if (vSelf->syscallInfo.op == PTRACE_SYSCALL_INFO_ENTRY) {

        if (vSelf->syscallNum == vOther->syscallNum) {

            if (vOther->state == VSTATE_WAIT) {
                /* Partner has reached syscall entry and is waiting */

                variant *vmaster = vSelf->type == VTYPE_master ? vSelf : vOther;
                variant *vreplica = vSelf->type == VTYPE_master ? vOther : vSelf;

                int result;
                if ((result = processSyscallSync(vmaster, vreplica)) != 0) {
                    fprintf(stderr, "[MVEE] Syscall sync execution failed. Killing Variants.\n");
                    fprintf(stderr, "[MVEE]  strerror: %s\n", strerror(errno));
                    kill(vmaster->pid, SIGKILL);
                    kill(vreplica->pid, SIGKILL);
                    return result;
                }


            } else {
                /* Wait for other to reach syscall entry*/
                vSelf->state = VSTATE_WAIT;
            }

        } else if (vSelf->syscallNum > vOther->syscallNum) {
            /* Wait for other to fin prev syscall*/
            vSelf->state = VSTATE_WAIT;
            debug_printf(stderr, "\t %s waiting for partner\n",
                         vSelf->type == VTYPE_master ? "[M]" : "{R}");
        } else {
            fprintf(stderr, "[MVEE] Invalid State\n");
            exit(EXIT_FAILURE);
        }

    } else if (vSelf->syscallInfo.op == PTRACE_SYSCALL_INFO_EXIT) {
        /* Syscall exit (if not done by executor), resume */
        resumeVariantFromExit(vSelf);

    } else {
        fprintf(stderr, "[MVEE] Invalid State\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}


int main(int argc, char *argv[]) {


    if (argc < 1) {
        printf("Usage: ./MVEE <Program Path> <Arguments...>\n");
        return 1;
    }

    long systemCores = sysconf(_SC_NPROCESSORS_ONLN);
    if (systemCores == -1) {
        perror("sysconf");
        exit(EXIT_FAILURE);
    }
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(systemCores - 1, &mask);
    if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &mask) == -1) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }

    variant master;
    initVariant(&master, VTYPE_master, argv, systemCores - 2);
    fprintf(stderr, "[MVEE] Master PID: %d\n", master.pid);
    resumeVariantFromWait(&master);

    variant replica;
    initVariant(&replica, VTYPE_replica, argv, systemCores - 3);
    fprintf(stderr, "[MVEE] Replica PID: %d\n", replica.pid);
    resumeVariantFromWait(&replica);

    while (1) {

        // Wait for any child
        int status;
        pid_t pid = waitpid(-1, &status, __WALL);


        if (pid == master.pid) {

            if (checkVariantExit(&master, &status) != 0) break;
            if (processVariantEvent(&master, &replica) != 0) {
                debug_printf(stderr, "Invalid State\n");
                exit(EXIT_FAILURE);
            }


        } else if (pid == replica.pid) {

            if (checkVariantExit(&replica, &status) != 0) break;
            if (processVariantEvent(&replica, &master) != 0) {
                debug_printf(stderr, "Invalid State\n");
                exit(EXIT_FAILURE);
            }

        } else {
            debug_printf(stderr, "Unknown child %d", pid);
            exit(EXIT_FAILURE);
        }

    }

    fprintf(stderr, "[MVEE] Variants exited. Terminating.\n");
    exit(EXIT_SUCCESS);
}
