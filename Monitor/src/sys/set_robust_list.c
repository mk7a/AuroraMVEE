#include "set_robust_list.h"

int execSyscall_setRobustList(variant *master, variant *replica) {
    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);
    return 0;
}