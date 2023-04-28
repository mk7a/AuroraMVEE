#ifndef MVEE_UTIMENSAT_H
#define MVEE_UTIMENSAT_H

#include "../variant.h"

int execSyscall_utimensat(variant *master, variant *replica);

#endif //MVEE_UTIMENSAT_H
