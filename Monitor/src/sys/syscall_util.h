#ifndef MVEE_SYSCALL_UTIL_H
#define MVEE_SYSCALL_UTIL_H

#include <sys/syscall.h>

const char *S_READ = "read";
const char *S_CLOSE = "close";
const char *S_MMAP = "mmap";
const char *S_MPROTECT = "mprotect";
const char *S_BRK = "brk";
const char *S_ACCESS = "access";
const char *S_EXECVE = "execve";
const char *S_ARCH_PRCTL = "arch_prctl";
const char *S_SET_TID_ADDRESS = "set_tid_address";
const char *S_FADVISE64 = "fadvise64";
const char *S_OPENAT = "openat";
const char *S_NEWFSTATAT = "newfstatat";
const char *S_SET_ROBUST_LIST = "set_robust_list";
const char *S_PRLIMIT64 = "prlimit64";
const char *S_GETRANDOM = "getrandom";
const char *S_STATX = "statx";
const char *S_RSEQ = "rseq";
const char *S_EXIT_GROUP = "exit_group";
const char *S_MUNMAP = "munmap";
const char *S_WRITE = "write";
const char *S_PREAD64 = "pread64";
const char *S_RT_SIGACTION = "rt_sigaction";
const char *S_RT_SIGPROCMASK = "rt_sigprocmask";
const char *S_UTIMENSTAT = "utimensat";
const char *S_UNLINK = "unlink";
const char *S_FCHOWN = "fchown";
const char *S_FCHMOD = "fchmod";
const char *S_GETUID = "getuid";
const char *S_GETGID = "getgid";
const char *S_GETEUID = "geteuid";
const char *S_GETEGID = "getegid";
const char *S_LSEEK = "lseek";
const char *S_GETCWD = "getcwd";
const char *S_GETPID = "getpid";
const char *S_CHDIR = "chdir";
const char *S_SOCKET = "socket";
const char *S_SETSOCKOPT = "setsockopt";
const char *S_BIND = "bind";
const char *S_LISTEN = "listen";
const char *S_ACCEPT4 = "accept4";
const char *S_POLL = "poll";
const char *S_SHUTDOWN = "shutdown";
const char *S_GETSOCKNAME = "getsockname";
const char *S_PSELECT = "pselect6";

const char *getSyscallName(unsigned long long syscall) {
    switch (syscall) {
        case SYS_read:
            return S_READ;
        case SYS_close:
            return S_CLOSE;
        case SYS_mmap:
            return S_MMAP;
        case SYS_mprotect:
            return S_MPROTECT;
        case SYS_brk:
            return S_BRK;
        case SYS_access:
            return S_ACCESS;
        case SYS_execve:
            return S_EXECVE;
        case SYS_arch_prctl:
            return S_ARCH_PRCTL;
        case SYS_set_tid_address:
            return S_SET_TID_ADDRESS;
        case SYS_fadvise64:
            return S_FADVISE64;
        case SYS_openat:
            return S_OPENAT;
        case SYS_newfstatat:
            return S_NEWFSTATAT;
        case SYS_set_robust_list:
            return S_SET_ROBUST_LIST;
        case SYS_prlimit64:
            return S_PRLIMIT64;
        case SYS_getrandom:
            return S_GETRANDOM;
        case SYS_statx:
            return S_STATX;
        case SYS_rseq:
            return S_RSEQ;
        case SYS_exit_group:
            return S_EXIT_GROUP;
        case SYS_munmap:
            return S_MUNMAP;
        case SYS_write:
            return S_WRITE;
        case SYS_pread64:
            return S_PREAD64;
        case SYS_rt_sigaction:
            return S_RT_SIGACTION;
        case SYS_rt_sigprocmask:
            return S_RT_SIGPROCMASK;
        case SYS_utimensat:
            return S_UTIMENSTAT;
        case SYS_unlink:
            return S_UNLINK;
        case SYS_fchown:
            return S_FCHOWN;
        case SYS_fchmod:
            return S_FCHMOD;
        case SYS_getuid:
            return S_GETUID;
        case SYS_geteuid:
            return S_GETEUID;
        case SYS_getgid:
            return S_GETGID;
        case SYS_getegid:
            return S_GETEGID;
        case SYS_lseek:
            return S_LSEEK;
        case SYS_getcwd:
            return S_GETCWD;
        case SYS_getpid:
            return S_GETPID;
        case SYS_chdir:
            return S_CHDIR;
        case SYS_socket:
            return S_SOCKET;
        case SYS_setsockopt:
            return S_SETSOCKOPT;
        case SYS_bind:
            return S_BIND;
        case SYS_listen:
            return S_LISTEN;
        case SYS_accept4:
            return S_ACCEPT4;
        case SYS_poll:
            return S_POLL;
        case SYS_shutdown:
            return S_SHUTDOWN;
        case SYS_getsockname:
            return S_GETSOCKNAME;
        case SYS_pselect6:
            return S_PSELECT;

        default:
            return "unknown";
    }
}

#endif  //MVEE_SYSCALL_UTIL_H