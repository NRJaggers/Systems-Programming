/*******************************************************
PROGRAM NAME - Lab/Assignment - Title

PROGRAMMER - Nathan Jaggers

DATE - 00/00/00

DESCRIPTION - This program ...
*******************************************************/
#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include <wait.h>

using namespace std;

 int fd[2];

 void signalhandler(int i)
 {
     dup2(fd[0],STDIN_FILENO);
     //printf("overwrite stdin\n");
 }

int main()
{
    cout << "Start" << endl;
    pipe(fd);
    int parentPID = getpid();
    char text[100];
    //signal(SIGUSR1,signalhandler);

    if (fork()==0)
    {
        sleep(2);
        close(fd[0]);
        kill(parentPID,SIGUSR1);
        write(fd[1],"hello there",12);
        close(fd[1]);
        return 0;
    }
    else
    {
        signal(SIGUSR1,signalhandler);
        int save_stdin = dup(STDIN_FILENO);
        close(fd[1]);


        read(STDIN_FILENO, text,20);
        printf("%s\n",text);

        printf("Restore stdin\n");
        dup2(save_stdin,STDIN_FILENO);
        text[0] = 0;

        read(STDIN_FILENO,text,20);
        printf("%s\n", text);
        wait(0);
    }

    close(fd[0]);

    return 0;
}
