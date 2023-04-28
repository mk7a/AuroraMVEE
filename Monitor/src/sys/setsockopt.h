#ifndef MVEE_SETSOCKOPT_H
#define MVEE_SETSOCKOPT_H

#include "../variant.h"

int execSyscall_setsockopt(variant *master, variant *replica);

#endif //MVEE_SETSOCKOPT_H
