#include <stdio.h>
#include <stdlib.h>
#include "green.h"

int flag = 0;
green_cond_t cond;

#define LOOP_COUNT 10000
#define THREADS 2

void *test(void *arg)
{
    int id = *(int*) arg;
    int loop = LOOP_COUNT;
    while(loop > 0)
    {
        if(flag == id)
        {
            printf("thread %d: %d\n", id, loop);
            loop--;
            flag = (id + 1) % THREADS;
            green_cond_signal(&cond);
        } else {
            green_cond_wait(&cond);
        }
    }
}

int main()
{
    green_cond_init(&cond);

    green_t *gthreads = malloc(sizeof(struct green_t) * THREADS);
    int *args = malloc(sizeof(int) * THREADS);

    for(int i = 0; i < THREADS; i++)
        args[i] = i;

    for(int i = 0; i < THREADS; i++)
        green_create(&gthreads[i], test, &args[i]);

    for(int i = 0; i < THREADS; i++)
        green_join(&gthreads[i], NULL);

    printf("donk\n");
    return 0;
}