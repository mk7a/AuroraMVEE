#ifndef MVEE_GETPID_H
#define MVEE_GETPID_H

#include "../variant.h"

int execSyscall_getpid(variant *master, variant *replica);

#endif //MVEE_GETPID_H
