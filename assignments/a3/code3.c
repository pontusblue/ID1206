#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int volatile done;

void handler(int sig, siginfo_t *siginfo, void *context)
{
    printf("signal %d ouch that hurt\n", sig);
    
    printf("your UID is %d\n", siginfo->si_uid);
    printf("your PID is %d\n", siginfo->si_pid);
    
    done = 1;
}

int not_so_good()
{
    int x = 0;
    return 1 % x;
}

int main()
{
    struct sigaction sa;

    printf("My PID: %d\n", getpid());
    printf("Ok, let's go - I'll catch my own error.\n");

    /* we're using the more elaborated sigaction handler */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;

    sigemptyset(&sa.sa_mask);

    if(sigaction(SIGINT, &sa, NULL) != 0) {
	return 1;
    }

    while(!done) {}

    printf("Told you so!\n");
    return 0;

}
