#ifndef MVEE_SHUTDOWN_H
#define MVEE_SHUTDOWN_H

#include "../variant.h"

int execSyscall_shutdown(variant *master, variant *replica);

#endif //MVEE_SHUTDOWN_H
