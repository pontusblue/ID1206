#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

static ucontext_t main_cntx = {0};
static green_t main_green = {
    &main_cntx,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    FALSE
};

typedef struct green_rq {
    green_t *thread;
    green_rq *next;
    green_rq *prev;
};

struct green_rq rq_head, rq_tail;

void queue(green_t *thread)
{
    struct green_rq *new = malloc(sizeof(struct green_rq));
    new->thread = thread;
    if(rq_head == NULL)
    {
        rq_head = new;
    }
    else
    {
        new->next = rq_tail;
        rq_tail->prev = new;
    }
    rq_tail = new;
}

green_t* pop()
{
    green_t *elem = rq_head->thread;
    green_rq *node = rq_head;
    rq_head = rq_head->prev;
    if(rq_head != NULL)
    {
        rq_head->next = NULL;
    }
    free(node);
    return elem;
}

static green_t *running = &main_green;

static init() __attribute__((constructor));

void init()
{
    getcontext(&main_cntx);
}

int green_create(green_t *new, void *(*fun)(void*), void *arg)
{
    ucontext_t *cntx = (ucontext_t *) malloc(sizeof(ucontext_t));
    getcontext(cntx);

    void *stack = malloc(STACK_SIZE);

    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;
    makecontext(cntx, green_thread, 0);

    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->retval = NULL;
    new->zombie = FALSE;

    queue(new);

    return 0;
}

