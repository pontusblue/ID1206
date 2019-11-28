#include <stdint.h>
#include <sys/mman.h>

#define sanity() (insanity(__FILE__, __LINE__));

void *dalloc(size_t request);
void dfree(void *memory);
void insanity(char* file, int line);
void init();