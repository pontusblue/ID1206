#include "dlmall.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARG_COUNT 3
#define ARENA (64*1024)

#define GET_COUNT(size) (allocationCounts[size])
#define ADD_COUNT(size) (allocationCounts[size]++)
#define SUB_COUNT(size) (allocationCounts[size]--)
#define RANDOM(min, max) (rand() % (max - min + 1) + min)

int testCount;
int minSize;
int maxSize;
int maxAllocCount;
int allocCount;
int testId;

int *allocationCounts;

struct node
{
    int *data;
    struct node *next;
    struct node *prev;
};

struct node *lh;
struct node *lt;
unsigned int ll = 0;

void push(int *e)
{
    struct node *nn = (struct node*) calloc(1, sizeof(struct node));
    if(nn == NULL) {
        printf("CALLOC FAILED!");
        return;
    }
    ll++;
    nn->data = e;
    nn->next = lt;
    if(lt != NULL) {
        lt->prev = nn;
    }
    lt = nn;
    if(lh == NULL) lh = lt;
}

int* peek()
{
    return lh->data;
}

int* pop()
{
    if(ll == 0) return NULL;
    ll--;
    int *d = lh->data;
    struct node *nlh = lh->prev;
    if(nlh != NULL) {
        nlh->next = NULL;
    }
    free(lh);
    lh = nlh;
    return d;
}

int count()
{
    return ll;
}

void freeAll()
{
    while(ll > 0)
    {
        pop();
    }
}

int main(int argc, char *argv[])
{
    argc--;
    if(argc < ARG_COUNT) {
        printf("Usage: ./bench <test id> <min malloc size> <max malloc size>\
         [max number of allocations] [number of allocations]\n");
        return -1;
    }
    argv = &argv[1];
    init();

    testId  = atoi(argv[0]);  
    minSize = atoi(argv[1]);
    maxSize = atoi(argv[2]);

    if(argc < ARG_COUNT + 1) {
        maxAllocCount = ARENA/maxSize/2;
    } else {
        maxAllocCount = atoi(argv[3]);
    }

    if(argc < ARG_COUNT + 2) {
        allocCount = ARENA/maxSize * 10;
    } else {
        allocCount = atoi(argv[4]);
    }

    allocationCounts = malloc(sizeof(int) * ARENA);

    clock_t start, end;
    double time;

    srand(clock());
    start = clock();

    switch(testId)
    {
        case 1:
            for(int i = 0; i < allocCount; i++)
            {
                if(rand()%2 && count() > 0) {
                    dfree(pop());
                } else {
                    if(count() >= maxAllocCount && count() > 0) {
                        dfree(pop());
                    }
                    int *d = dalloc(RANDOM(minSize, maxSize));
                    if(d != NULL) {
                        push(d);
                    } else if(count() > 0) {
                        dfree(pop());
                    }
                }
            }
            break;
        default:
            printf("No test with id %d\n", testId);
    }

    end = clock();
    time = ((double) (end - start)) / CLOCKS_PER_SEC;

    struct head *ar = arena;
    switch (testId)
    {
    case 1:
        while(ar < (struct head*) ((char*)arena + ARENA))
        {
            ADD_COUNT(ar->size);
            ar = after(ar);
        }
        printf("Total time: %f ms", (time * 1000));
        for(int i = 0; i < ARENA; i++)
        {
            if(GET_COUNT(i) != 0) {
                printf("%d %d\n", i, GET_COUNT(i));
            }
        }
        break;
    default:
        break;
    }

    free(allocationCounts);
    freeAll();
}