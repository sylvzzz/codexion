#include "../codexion.h"
#include "sys/wait.h"
#include "sys/types.h"

int main()
{
    int pid = fork();
    int i = 2;
    
    if (pid == -1)
    {
        printf("Something went wrong...");
    }

    if (pid == 0)
        i++;

    sleep(2);
    printf("Value of i: %i\n", i);
    printf("Hello from process. PID: %i\n", getpid());

    if (pid != 0)
    {
        wait(NULL);
    }
}