#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

char global[] = "This is a global string";

const int coolConstant = 1337;

int main()
{
    int pid = getpid();
  foo:
    printf("Process id: %d\n", pid);
    printf("Global string: %p\n", &global);
    printf("Cool constant: %p\n", &coolConstant);
    printf("The code: %p\n", &&foo);

    printf("\n\n /proc/%d/maps \n\n", pid);

    char command[50];

    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);

    return 0;
}