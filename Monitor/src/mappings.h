#ifndef MVEE_MAPPINGS_H
#define MVEE_MAPPINGS_H

#include <stddef.h>

typedef struct {
    void *addr;
    size_t len;
    int prot;
    int flags;
} variantMMap;

void addMapping(void *addr, size_t len, int prot, int flags);

int removeMapping(void *addr);

variantMMap *findMapping(void *targetAddr);

void printMappings();

#endif //MVEE_MAPPINGS_H
