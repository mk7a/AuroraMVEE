#ifndef MVEE_POLL_H
#define MVEE_POLL_H

#include "../variant.h"

int execSyscall_poll(variant *master, variant *replica);

#endif //MVEE_POLL_H
