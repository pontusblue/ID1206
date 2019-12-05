#include <stdint.h>
#include <sys/mman.h>

#ifdef SANITY_DEBUG
#define sanity() (insanity(__FILE__, __LINE__, __FUNCTION__));
#else
#define sanity() (1)
#endif

struct head 
{
    uint16_t bfree;
    uint16_t bsize;
    uint16_t free;
    uint16_t size;
    struct head *next;
    struct head *prev;    
};

void *dalloc(size_t request);
void dfree(void *memory);
void insanity(char* file, int line, char* func);
void init();
struct head* after(struct head*);

struct head *arena;
int flistSize;