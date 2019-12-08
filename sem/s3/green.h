#include <ucontext.h>

typedef struct green_t {
    ucontext_t *context;
    void *(*fun)(void*);
    void *arg;
    struct green_t *next;
    struct green_t *join;
    void *retval;
    int zombie;
} green_t;

void green_create(green_t *thread, void *(*fun)(void*), void *arg);
void green_yield();
void green_join(green_t *thread, void** val);

