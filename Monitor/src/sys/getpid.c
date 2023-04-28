#include "getpid.h"

int execSyscall_getpid(variant *master, variant *replica) {

    return runMasterReplicateRet(master, replica);
}