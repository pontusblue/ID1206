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
struct head *flist;

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
    splt->free = TRUE;

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

    if(new == MAP_FAILED) {
        printf("mmap failed: error %d\n", errno);
        return NULL;
    }

    // make room for head and dummy
    uint size = ARENA - 2*HEAD;
    
    new->bfree = FALSE;
    new->bsize = 0;
    new->free  = TRUE;
    new->size  = size;

    struct head *sentinel = after(new); 

    // only touch after the status fields
    sentinel->bfree = TRUE;
    sentinel->bsize = size;
    sentinel->free  = FALSE;
    sentinel->size  = 0;

    // this is the only arena we have
    arena = (struct head*)new;

    return new;
}

void detach(struct head *block)
{
    if(block->next != NULL) {
        block->next->prev = block->prev;
    }

    if(block->prev != NULL) {
        block->prev->next = block->next;
    } else {
        flist = block->next;
    }
}

void insert(struct head *block)
{
    block->next = flist;
    block->prev = NULL;

    head *newBlock = new();

    if(flist != NULL) {
        flist->prev  = newBlock;
        flist->bfree = newBlock->free;
        flist->bsize = newBLock->size;
    }
    flist = newBlock;
}

void *dalloc(size_t request)
{
    if(request <= 0) {
        return NULL;
    }
    int size = adjust(request);
    struct head *taken = find(size);
    if(taken == NULL)
    {
        return NULL;
    } else {
        return (void*) (taken + HEAD); // Return the memory address of the block
    }
}

int adjust(int size)
{
    return size + ALIGN - (size % ALIGN); 
}

struct head *find(int size)
{
    struct head *h = flist;
    while(h->size != 0)
    {
        if(!h->free || h->size < size)
        {
            h = after(h);
            continue;
        }
        else return h;
    }
    return NULL;
} 

