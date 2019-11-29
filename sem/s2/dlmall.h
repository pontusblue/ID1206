#include <stdint.h>
#include <sys/mman.h>

#define sanity() (insanity(__FILE__, __LINE__, __FUNCTION__));

void *dalloc(size_t request);
void dfree(void *memory);
void insanity(char* file, int line, char* func);
void init();