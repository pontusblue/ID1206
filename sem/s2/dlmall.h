#include <stdint.h>
#include <sys/mman.h>

#ifdef SANITY_DEBUG
#define sanity() (insanity(__FILE__, __LINE__, __FUNCTION__));
#else
#define sanity() (1)
#endif

void *dalloc(size_t request);
void dfree(void *memory);
void insanity(char* file, int line, char* func);
void init();