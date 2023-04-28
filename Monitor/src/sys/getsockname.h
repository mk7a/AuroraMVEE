#ifndef MVEE_GETSOCKNAME_H
#define MVEE_GETSOCKNAME_H

#include "../variant.h"

int execSyscall_getsockname(variant *master, variant *replica);

#endif //MVEE_GETSOCKNAME_H
