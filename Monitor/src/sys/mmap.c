#include "mmap.h"
#include "../mappings.h"
#include "../security_manager.h"

#include <sys/mman.h>
#include <stdio.h>

int execSyscall_mmap(variant *master, variant *replica) {
    /* void *addr, size_t length, int prot, int flags,
                  int fd, off_t offset */

    int eq = 1;

    /* void *addr */
    /* Equality not required TODO ?*/

    /* size_t length */
    eq &= master->syscallInfo.entry.args[1] == replica->syscallInfo.entry.args[1];

    /* int prot */
    int masterProt = (int) master->syscallInfo.entry.args[2];
    eq &= masterProt == replica->syscallInfo.entry.args[2];

    /* int flags */
    int masterFlags = (int) master->syscallInfo.entry.args[3];
    eq &= masterFlags == replica->syscallInfo.entry.args[3];

    /* int fd */
    eq &= master->syscallInfo.entry.args[4] == replica->syscallInfo.entry.args[4];

    /* off_t offset */
    eq &= master->syscallInfo.entry.args[5] == replica->syscallInfo.entry.args[5];

    if (!eq) return -1;

    if (checkMMapPerms(masterProt, masterFlags) != 0) {
        return -1;
    }

    resumeVariantFromWait(master);
    if (waitVariant(master) != 0) return -1;
    requestSyscallInfo(master);
    void *mappedAdrmaster = (void *) master->syscallInfo.exit.rval;
    resumeVariantFromExit(master);

    addMapping(mappedAdrmaster,
               master->syscallInfo.entry.args[1],
               masterProt,
               masterFlags);

    resumeVariantFromWait(replica);

    return 0;
}