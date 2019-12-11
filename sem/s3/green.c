#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include "green.h"

#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096
#define PERIOD 100

static ucontext_t main_cntx = {0};
static green_t main_green = {
        -1,
        &main_cntx,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        FALSE
};

static sigset_t block;

struct green_t *rq_head;
struct green_t *rq_tail;

static green_t *running = &main_green;

void green_thread();
void timer_handler(int);

void queue_thread(green_t **, green_t *);
green_t *pop_thread(green_t **);
void rqueue(green_t *);
green_t* rpop();

static void init() __attribute__((constructor));

void init()
{
    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = {0};
    struct timeval interval;
    struct itimerval period;

    act.sa_handler = timer_handler;
    assert(sigaction(SIGVTALRM, &act, NULL) == 0);

    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &period, NULL);

    getcontext(&main_cntx);
}

void timer_handler(int sig)
{
    green_yield();
}

int green_create(green_t *new, void *(*fun)(void*), void *arg)
{
    ucontext_t *cntx = (ucontext_t *) malloc(sizeof(ucontext_t));
    getcontext(cntx);

    void *stack = malloc(STACK_SIZE);

    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;
    makecontext(cntx, green_thread, 0);

    new->id = *(int*)arg;
    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new->next = NULL;
    new->join = NULL;
    new->retval = NULL;
    new->zombie = FALSE;

    sigprocmask(SIG_BLOCK, &block, NULL);
    // add to ready queue
    rqueue(new);

    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

void green_thread()
{
    green_t *this = running;

    void *result = (*this->fun)(this->arg);

    sigprocmask(SIG_BLOCK, &block, NULL);
    // place waiting (joining) thread in ready queue
    if(this->join != NULL)
    {
        rqueue(this->join);
        this->join = NULL;
    }

    // save result of execution
    this->retval = result;

    // we're a zombie
    this->zombie = TRUE;

    // find the next thread to run
    green_t *next = rpop();
    //if(next == NULL) return;

    running = next;
    setcontext(next->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_yield()
{
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t *susp = running;

    // add susp to ready queue
    rqueue(susp);

    // select the next thread for execution 
    green_t *next = rpop();

    running = next;
    swapcontext(susp->context, next->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

int green_join(green_t *thread, void **res)
{
    sigprocmask(SIG_BLOCK, &block, NULL);
    if(!thread->zombie)
    {
        green_t *susp = running;

        // add as joining thread
        thread->join = susp;
        //jqueue_thread(&thread->join, susp);

        // select the next thread for execution
        green_t *next = rpop();

        running = next;
        if(next != NULL)
            swapcontext(susp->context, next->context);
        else printf("????????\n");
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);

    if(res != NULL)
    {
        *res = thread->retval;
    }

    // free context 
    free(thread->context->uc_stack.ss_sp);
    free(thread->context);
    return 0;
}

void green_cond_init(green_cond_t *cond)
{
    cond->queue = NULL;
}

void green_cond_wait(green_cond_t *cond, green_mutex_t *mutex)
{
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t *susp = running;

    queue_thread(&(cond->queue), susp);

    if(mutex != NULL && mutex->taken)
    {
        mutex->taken = FALSE;
        rqueue(pop_thread(&mutex->queue));
    }

    // select the next thread for execution
    green_t *next = rpop();

    running = next;
    swapcontext(susp->context, next->context);

    if(mutex != NULL)
    {
        if(mutex->taken)
        {

        }
        else
        {
            mutex->taken = TRUE;
        }
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void green_cond_signal(green_cond_t *cond)
{
    sigprocmask(SIG_BLOCK, &block, NULL);
    rqueue(pop_thread(&(cond->queue)));
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_mutex_init(green_mutex_t *mutex)
{
    mutex->taken = FALSE;
    // init other fields
}

int green_mutex_lock(green_mutex_t *mutex)
{
    sigprocmask(SIG_BLOCK, &block, NULL);

    green_t *susp = running;
    if(mutex->taken)
    {
        // suspend current thread
        queue_thread(&mutex->queue, susp);
        // find next thread
        green_t *next = rpop();
        running = next;
        swapcontext(susp->context, next->context);
    }
    else
    {
        // take the lock
        mutex->taken = TRUE;
    }

    sigprocmask(SIG_UNBLOCK, &block, NULL);
}
int green_mutex_unlock(green_mutex_t *mutex)
{
    sigprocmask(SIG_BLOCK, &block, NULL);
    if(mutex->queue != NULL)
    {
        rqueue(pop_thread(&mutex->queue));
    }
    else
    {
        mutex->taken = FALSE;
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

void queue_thread(green_t **queue, green_t *thread)
{
    if(queue == NULL) return;
    else if(*queue == NULL)
    {
        *queue = thread;
    }
    else
    {
        green_t *h = *queue;
        while(h->next != NULL)
        {
            h = h->next;
        }
        h->next = thread;
    }
}

green_t *pop_thread(green_t **queue)
{
    green_t *thread = *queue;
    if(thread != NULL)
    {
        *queue = thread->next;
        thread->next = NULL;
    }
    return thread;
}

void rqueue(green_t *thread)
{
    if(thread == NULL) return;
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
        if(rq_head == NULL)
            rq_tail = NULL;

    }
    return tmp;
}