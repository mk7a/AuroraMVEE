#include "mprotect.h"
#include "../mappings.h"
#include "../security_manager.h"

int execSyscall_mprotect(variant *master, variant *replica) {
    /*
     * int mprotect(void *addr, size_t len, int prot);
     */

    int eq = 1;

    /* void *addr */
    /* Equivalence not required */
    void *targetAddr = (void *) master->syscallInfo.entry.args[0];

    /* size_t len */
    eq &= (master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1]);

    /* int prot */
    int newProt = (int) master->syscallInfo.entry.args[2];
    eq &= (newProt == replica->syscallInfo.entry.args[2]);

    if (eq) {

        variantMMap *map;
        if ((map = findMapping(targetAddr)) != NULL) {

            if (checkMMapPerms(newProt, map->flags) != 0) {
                return -1;
            }
        }

        resumeVariantFromWait(master);
        resumeVariantFromWait(replica);

        return 0;

    } else {

        return -1;
    }
}