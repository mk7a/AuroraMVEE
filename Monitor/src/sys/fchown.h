#ifndef MVEE_FCHOWN_H
#define MVEE_FCHOWN_H

#include "../variant.h"

int execSyscall_fchown(variant *master, variant *replica);

#endif //MVEE_FCHOWN_H
