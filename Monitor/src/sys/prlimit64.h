#ifndef MVEE_PRLIMIT64_H
#define MVEE_PRLIMIT64_H

#include "../variant.h"

int execSyscall_prlimit64(variant *master, variant *replica);

#endif //MVEE_PRLIMIT64_H
