
#include "rseq.h"

int execSyscall_rseq(variant *master, variant *replica) {
    /*
     * int syscall(SYS_rseq, struct rseq* rseq, uint32_t rseq_len,
                int flags, uint32_t sig);
     */

    int eq = 1;

    /* struct rseq* rseq */
    /* Equality not required*/

    /* uint32_t rseq_len */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* int flags */
    eq &= master->syscallInfo.entry.args[2] == replica->syscallInfo.entry.args[2];

    /* uint32_t sig */
    eq &= master->syscallInfo.entry.args[3] == replica->syscallInfo.entry.args[3];

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return eq ? 0 : -1;
}