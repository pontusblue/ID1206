#include "dlmall.h"

int main()
{
    int *test = dalloc(sizeof(int) * 10);
    
    for(int i = 0; i < 10; i++)
    {
        test[i] = i + 1;
    }

    sanity();

    return 0;
}