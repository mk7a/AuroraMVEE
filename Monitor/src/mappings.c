#include "mappings.h"
#include "debug.h"
#include <malloc.h>


typedef struct node node;
struct node {
    variantMMap entry;
    node *next;
    node *prev;
};

node dummy = {{}, NULL, NULL};
node *mappings = &dummy;


void addMapping(void *addr, size_t len, int prot, int flags) {
    node *new = malloc(sizeof(node));
    new->entry.addr = addr;
    new->entry.len = len;
    new->entry.prot = prot;
    new->entry.flags = flags;
    new->next = NULL;

    node *current = mappings;
    while (current->next != NULL) current = current->next;

    current->next = new;
    new->prev = current;
}

int removeMapping(void *addr) {
    node *current = mappings->next;

    while (current != NULL) {

        if (current->entry.addr == addr) {
            current->prev->next = current->next;
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            free(current);
            return 0;
        }

        current = current->next;
    }

    return -1;
}

variantMMap *findMapping(void *targetAddr) {

    node *current = mappings->next;

    while (current != NULL) {

        if ((current->entry.addr == targetAddr && current->entry.len == 0) ||
            (current->entry.len > 0
             && targetAddr >= current->entry.addr
             && targetAddr < current->entry.addr + current->entry.len)) {

            return &current->entry;
        }

        current = current->next;
    }

    return NULL;
}

void printMappings() {
    node *current = mappings->next;

    int count = 0;
    debug_printf(stderr, "==== Mappings ===\n");
    while (current != NULL) {
        debug_printf(stderr, "[%d] addr: %p, len: %lu, prot: %d, flags: %d\n",
                     count++, current->entry.addr, current->entry.len,
                     current->entry.prot, current->entry.flags);
        current = current->next;
    }
}