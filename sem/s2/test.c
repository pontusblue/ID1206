#include "dlmall.h"

int main()
{
    init();
    int *test = dalloc(sizeof(int) * 10);
    
    int i;
    for(i = 0; i < 10; i++)
    {
        test[i] = i + 1;
    }

    char *stringtest = dalloc(sizeof(char) * 13);

    printf("Value test: ");
    for (i = 0; i < 10; i++)
    {
        printf("%d, ", test[i]);
    }


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

    dfree(test);
    dfree(stringtest);
    test = 0;
    stringtest = 0;

    return 0;
}