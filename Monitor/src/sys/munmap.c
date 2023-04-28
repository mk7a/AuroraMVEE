#include <stdio.h>
#include "munmap.h"
#include "../mappings.h"
#include "../debug.h"

int execSyscall_munmap(variant *master, variant *replica) {
    /*
     * int munmap(void *addr, size_t length);
     */

    int eq = 1;

    /* void *addr */
    /* Equality not required */
    void *targetAddr = (void *) master->syscallInfo.entry.args[0];

    /* size_t length */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    if (!eq) {
        return -1;
    }

    variantMMap *map;
    if ((map = findMapping(targetAddr)) != NULL) {
        if (map->addr != targetAddr) {
            debug_printf(stderr, "munmap: TODO partial unmap handling not implemented.\n");
            debug_printf(stderr, "\tTarget: %p, in containing: %p at offset %ld\n",
                         targetAddr, map->addr, targetAddr - map->addr);
            printMappings();
            return -1;
        }
        removeMapping(map->addr);

    } else {
        debug_printf(stderr, "munmap Unexpected Error: no mapping found for address %p\n", targetAddr);
        printMappings();
        return -1;
    }

    resumeVariantFromWait(master);
    resumeVariantFromWait(replica);

    return 0;


}