#ifndef MVEE_MPROTECT_H
#define MVEE_MPROTECT_H

#include "../variant.h"

int execSyscall_mprotect(variant *master, variant *replica);

#endif //MVEE_MPROTECT_H
