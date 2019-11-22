#include <sys/mman.h>
#include <stdint.h>

#define TRUE 1
#define FALSE 0

struct head 
{
    uint16_t bfree;
    uint16_t bsize;
    uint16_t free;
    uint16_t size;
    struct head *next;
    struct head *prev;    
};

#define HEAD (sizeof(struct head))
#define MIN(size) (((size)>(8))?(size):(8))
#define LIMIT(size) (MIN(0) + HEAD + size)
#define MAGIC(memory) ((struct head*)memory - 1)
#define HIDE(block) (void*)((struct head*)block + 1)
#define ALIGN 8
#define ARENA (64*1024)

struct head *arena = NULL;

struct head *after(struct head *block)
{
    return (struct head*)((char*) block + block->size + HEAD);
}

struct head *before(struct head *block)
{
    return (struct head*)((char*) block - block->size - HEAD);
}

struct head *split(struct head *block, int size)
{
    int rsize = block->size - size - HEAD;
    block->size = rsize;

    struct head *splt = malloc(size + HEAD);
    splt->bsize = block->size;
    splt->bfree = block->free;
    splt->size = size + HEAD;
    splt->free = FALSE;

    struct head *aft = after(splt);
    aft->bsize = splt->size;

    return splt;
} 

struct head *new()
{
    if(arena != NULL) {
        printf("one arena already allocated\n");
        return NULL;
    }

    struct head *new = mmap(NULL, ARENA,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
