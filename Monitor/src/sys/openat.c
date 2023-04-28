#include "openat.h"
#include "../debug.h"

#include <stdio.h>
#include <assert.h>


int execSyscall_openat(variant *master, variant *replica) {
    /*
     *    int openat(int dirfd, const char *pathname, int flags);
     *    int openat(int dirfd, const char *pathname, int flags, mode_t mode);
     */

    int eq = 1;

    /* int dirfd */
    eq &= (master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0]);

    /* int flags */
    eq &= (master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2]);

    /* mode_t mode */
    if (master->syscallInfo.entry.args[3] != 0) { /* If not NULL */
        eq &= (master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3]);
    }

    /* const char *pathname */
    if (eq) {

        int ret = variantStrCmp(master, (void *) master->syscallInfo.entry.args[1],
                                replica, (void *) replica->syscallInfo.entry.args[1]);

        if (ret == -1) return -1;
        eq &= (ret == 0);
    }


    if (eq) {

        return runBothCopyMasterRet(master, replica);

    } else {
        debug_printf(stderr, "M openat(%llu, %p, %llu, %llu)\n",
                     master->syscallInfo.entry.args[0],
                     (void *) master->syscallInfo.entry.args[1],
                     master->syscallInfo.entry.args[2],
                     master->syscallInfo.entry.args[3]);
        debug_printf(stderr, "R openat(%llu, %p, %llu, %llu)\n",
                     replica->syscallInfo.entry.args[0],
                     (void *) replica->syscallInfo.entry.args[1],
                     replica->syscallInfo.entry.args[2],
                     replica->syscallInfo.entry.args[3]);
        return -1;
    }
}