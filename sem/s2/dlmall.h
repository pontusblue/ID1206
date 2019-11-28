#include <stdint.h>
#include <sys/mman.h>

void *dalloc(size_t request);
void dfree(void *memory);
void sanity();