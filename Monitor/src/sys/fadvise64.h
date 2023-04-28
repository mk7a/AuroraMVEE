#ifndef MVEE_FADVISE64_H
#define MVEE_FADVISE64_H

#include "../variant.h"

int execSyscall_fadvise64(variant *master, variant *replica);

#endif //MVEE_FADVISE64_H
