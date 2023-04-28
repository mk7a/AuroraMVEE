#include <unistd.h>
#include "setsockopt.h"

int execSyscall_setsockopt(variant *master, variant *replica) {
    /*
     * int setsockopt(int socket, int level, int option_name,
           const void *option_value, socklen_t option_len);
     */

    int eq = 1;

    /* int socket */
    eq &= master->syscallInfo.entry.args[0] == replica->syscallInfo.entry.args[0];

    /* int level */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* int option_name */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    /* socklen_t option_len */
    socklen_t option_len_master = master->syscallInfo.entry.args[4];
    eq &= option_len_master == replica->syscallInfo.entry.args[4];

    /* const void *option_value */
    if (eq) {
        eq &= variantMemCmp(master, (void *) master->syscallInfo.entry.args[3],
                            replica, (void *) replica->syscallInfo.entry.args[3],
                            option_len_master) == 0;
    }

    if (eq) {
        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;

    } else {


        return -1;
    }
}