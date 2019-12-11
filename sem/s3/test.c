#include <stdio.h>
#include <stdlib.h>
#include "green.h"

int flag = 0;
green_cond_t cond;
green_mutex_t mutex;

#define LOOP_COUNT 10000
#define THREADS 2

void *test(void *arg)
{
    int id = *(int*) arg;
    int loop = LOOP_COUNT;
    while(loop > 0)
    {
        green_mutex_lock(&mutex);
        while(flag != id)
        {
            green_mutex_unlock(&mutex);
            green_cond_wait(&cond);
            //maybe add new mutex here??
        }
        flag = (id + 1) % 2;
        green_cond_signal(&cond);
        green_mutex_unlock(&mutex);
        loop--;
    }
}

int main()
{
    green_cond_init(&cond);
    green_mutex_init(&mutex);

    green_t *gthreads = malloc(sizeof(struct green_t) * THREADS);
    int *args = malloc(sizeof(int) * THREADS);
    int **res = malloc(sizeof(int*) * THREADS);

    for(int i = 0; i < THREADS; i++)
        args[i] = i;

    for(int i = 0; i < THREADS; i++)
        green_create(&gthreads[i], test, &args[i]);

    for(int i = 0; i < THREADS; i++)
        green_join(&gthreads[i], (void **) &res[i]);

    printf("donk\n");
    return 0;
}