/*
Makayla Soh | 5/20/2022
Filename: waitpidEX.cpp
Purpose:
    Demonstrate use of WNOHANG with waitpid(). Show that
    WNOHANG returns 0 if child process is still running and a non-zero value if the child process has ended.
Program description: 
    Create one child process. If the child process is still running, the parent process should continue and 
    NOT wait for the child process. When the child process ends, the parent process should wait for that child,
    then fork a new child.
*/

#include <iostream>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

int main()
{
    // VARIABLES
    int status; // disreagard for this class

    // holds child pid
    int *childpid = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
    *childpid = 0;

    // flag to tell if a child process is running: 0 = not running, 1 = running
    int *existchild = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
    *existchild = 0;

    // LOOP
    while(1) {
        
        // if no child is running, create a new child
        if(*existchild == 0) {
            
            if(fork() == 0) {

                // child process
                *existchild = 1;
                *childpid = getpid();
                cout << "This is child " << *childpid << endl;
                sleep(10);
                return 0;
            }
        }

        // parent process
        // notice that this continually prints, so parent process does NOT get stuck at waitpid()
        sleep(1);
        cout << "The parent is running." << endl;

        // use WNOHANG to wait for child process ONLY if child process is finished
        int endId = waitpid(*childpid, &status, WNOHANG);

        // endId == 0 if child process is still running
        // endId != 0 if child process has finished and parent has successfully waited for the child
        if(endId != 0) {
            *existchild = 0; // child no longer running, reset flag
            cout << "Child " << *childpid << "has ended sucessfully." << endl;
        }

    }

    return 0;
}