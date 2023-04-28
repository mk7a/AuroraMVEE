#ifndef MVEE_RSEQ_H
#define MVEE_RSEQ_H

#include "../variant.h"

int execSyscall_rseq(variant *master, variant *replica);

#endif //MVEE_RSEQ_H
