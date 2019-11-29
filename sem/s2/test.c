#include "dlmall.h"

#define DEBUG

int main()
{
    int lightDebug = 1;

    init();
    int *test = dalloc(sizeof(int) * 10);
    
    int i;
    for(i = 0; i < 10; i++)
    {
        test[i] = i + 1;
    }

#ifndef LDEBUG
    char *stringtest = dalloc(sizeof(char) * 13);
    char *stringtest2 = dalloc(sizeof(char) * 13);
    char *stringtest3 = dalloc(sizeof(char) * 13);
#endif

    printf("Value test: ");
    for (i = 0; i < 10; i++)
    {
        printf("%d, ", test[i]);
    }

#ifndef LDEBUG
    i = 0;
    stringtest[i++] = 'H';
    stringtest[i++] = 'e';
    stringtest[i++] = 'l';
    stringtest[i++] = 'l';
    stringtest[i++] = 'o';
    stringtest[i++] = ' ';
    stringtest[i++] = 'W';
    stringtest[i++] = 'o';
    stringtest[i++] = 'r';
    stringtest[i++] = 'l';
    stringtest[i++] = 'd';
    stringtest[i++] = '!';
    stringtest[i] = '\0';

    printf("\nTest: %s", stringtest);
#endif

    dfree(test);
    test = 0;

#ifndef LDEBUG
    dfree(stringtest);
    stringtest = 0;
    sanity();
    dfree(stringtest2);
    stringtest2 = 0;
    sanity();
    dfree(stringtest3);
    stringtest3 = 0;
#endif

    return 0;
}