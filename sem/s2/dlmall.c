#include "dlmall.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

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
    return (struct head*)((char*) block - block->bsize - HEAD);
}

struct head *split(struct head *block, int size)
{
    int rsize = block->size - size - HEAD;
    block->size = size;

    struct head *splt = after(block); 
    splt->bsize = block->size;
    splt->bfree = block->free;
    splt->size = rsize;
    splt->free = TRUE;

    struct head *aft = after(splt);
    aft->bsize = splt->size;
    aft->bfree = splt->free;

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
    int size = ARENA - 2*HEAD;
    
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

    if(flist != NULL) {
        flist->prev = block;
    }

    flist = block;
}

int adjust(int size)
{
    if(size % ALIGN == 0) return size;
    return size + ALIGN - (size % ALIGN); 
}

struct head *find(int size)
{
    struct head *h = flist;
    while(h != 0)
    {
        if(h->size < size)
        {
            h = h->next;
            continue;
        }
        else return h;
    }
    return NULL;
} 

void *dalloc(size_t request)
{
    if(request <= 0) {
        return NULL;
    }
    int size = adjust(request);
    struct head *taken = find(size);
    if(taken == NULL) {
        sanity();
        return NULL;
    } else {
        detach(taken);
        taken->free = FALSE;
        if(taken->size > size) {
            struct head *rem = split(taken, size);
            insert(rem);
        } else {
            struct head *aft = after(taken);
            aft->bfree = FALSE;
        }
        sanity();
        return (void*) ((char*)taken + HEAD); // Return the memory address of the block
    }
}

struct head *merge(struct head *block)
{
    struct head *aft = after(block);
    struct head *bfr = before(block);

    if(block->bfree) {
        detach(bfr);
        bfr->size = bfr->size + block->size + HEAD;
        bfr->free = TRUE;
        aft->bsize = bfr->size;

        block = bfr;
    }

    if(aft->free) {
        detach(aft);
        block->size = block->size + aft->size + HEAD;
        aft = after(aft);
        aft->bsize = block->size;
        aft->bfree = TRUE;
    }

    return block;
}

void dfree(void *memory)
{
    if(memory != NULL) {
        struct head *block = (struct head*) ((char*)memory - HEAD);
        struct head *aft = after(block);
        block->free = TRUE;
        aft->bfree = TRUE;

        block = merge(block);

        insert(block);
    }
    sanity();
}

void init()
{
    struct head *a = new();
    if(a != NULL) insert(a);
    sanity();
}

void insanity(char* file, int line, char* func)
{
    if(arena != NULL) {
        printf("\nSanity check! File: %s:%d <-- %s\n", file, line, func);
        struct head *h = arena;
        while(h < (struct head*) ((char*)arena + ARENA))
        {
            printf("-- %p:\tbfree(%d), bsize(%d), dataptr(%p)\n",
                h, h->bfree, h->bsize, (void*) ((char*)h + HEAD));
            printf("\t\t\t_free(%d), _size(%d), mod(%d), next(%p), prev(%p)\n",
                h->free, h->size, h->size%ALIGN, h->next, h->prev);

            h = after(h);
        }
    } else printf("Arena not created! File: %s:%d <-- %s\n", file, line, func);
}