#include <stdlib.h>
#include <ucontext.h>
#include <assert.h>
#include <stdio.h>
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

struct green_t *rq_head;
struct green_t *rq_tail;

struct green_t *jq_head;
struct green_t *jq_tail;

void gimme()
{
    green_t *t;
    t = rq_head;
    t = rq_tail;
    t = jq_head;
    t = jq_tail;
    t = &main_green;


}

void rqueue(green_t *thread)
{
    //if(thread == NULL) return;
    if(rq_tail != NULL)
    {
        rq_tail->next = thread;
    }
    else
    {
        rq_head = thread;
    }
    
    rq_tail = thread;
}

green_t* rpop()
{
    green_t *tmp = rq_head;
    if(rq_head != NULL)
    {
        rq_head = rq_head->next;
    }
    return tmp;
}

void jqueue(green_t *thread)
{
    //if(thread == NULL) return;
    if(jq_tail != NULL)
    {
        jq_tail->next = thread;
    }
    else
    {
        jq_head = thread;
    }
    jq_tail = thread;
}

green_t* jpop()
{
    green_t *tmp = jq_head;
    if(jq_head != NULL)
    {
        jq_head = jq_head->next;
    }
    return jq_head;
}

static green_t *running = &main_green;

static void init() __attribute__((constructor));

void init()
{
    getcontext(&main_cntx);
}

void green_thread();
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

    // add to ready queue
    rqueue(new);

    return 0;
}

void green_thread()
{
    gimme();
    green_t *this = running;

    void *result = (*this->fun)(this->arg);

    // place waiting (joining) thread in ready queue
    if(this->join != NULL)
        rqueue(this->join);

    // save result of execution
    this->retval = result;

    // we're a zombie
    this->zombie = TRUE;

    // find the next thread to run
    green_t *next = rpop();
    //if(next == NULL) return;

    running = next;
    gimme();
    setcontext(next->context);
}

int green_yield()
{
    green_t *susp = running;

    // add susp to ready queue
    rqueue(susp);

    // select the next thread for execution 
    green_t *next = rpop();

    running = next;
    swapcontext(susp->context, next->context);
    return 0;
}

int green_join(green_t *thread, void **res)
{
    if(!thread->zombie)
    {
        green_t *susp = running;

        // add as joining thread 
        //jqueue(susp);
        thread->join = susp;

        // select the next thread for execution
        green_t *next = rpop();

        running = next;
        swapcontext(susp->context, next->context);
    }
    res = thread->retval;

    // free context 
    free(thread->context->uc_stack.ss_sp);
    free(thread->context);
    return 0;
}

void green_cond_init(green_cond_t *cond)
{

}

void green_cond_wait(green_cond_t *cond)
{

}

void green_cond_signal(green_cond_t *cond)
{

}
