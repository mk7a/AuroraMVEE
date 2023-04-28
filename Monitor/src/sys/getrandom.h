#ifndef MVEE_GETRANDOM_H
#define MVEE_GETRANDOM_H

#include "../variant.h"

int execSyscall_getrandom(variant *master, variant *replica);

#endif //MVEE_GETRANDOM_H
