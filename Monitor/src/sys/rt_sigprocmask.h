#ifndef MVEE_RT_SIGPROCMASK_H
#define MVEE_RT_SIGPROCMASK_H

#include "../variant.h"

int execSyscall_rt_sigprocmask(variant *master, variant *replica);

#endif //MVEE_RT_SIGPROCMASK_H
