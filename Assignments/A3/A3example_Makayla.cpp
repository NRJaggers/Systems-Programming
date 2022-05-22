#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <cstring>

using namespace std;

//global variables
int fd[2];
char print = 0;

//sigHandler - redirects stdin
void sigHandler(int i)
{
    dup2(fd[0], STDIN_FILENO);
    print = 1;
    return;
}


int main()
{
    // setup pipe and signals
    pipe(fd); // fd[0] and fd[1]

    while(1) {
        // prompt user for user input
        //get arguments
        // - find, file, -s, -f
        // - q

        if(<input is find>) {
        // find and filename

            // keep track of the children
            if(fork() == 0) {

                if(<flag is f>) {
                    //find the root directory and subdirectories
                    char* result = findFile("/", filename);

                }
                else if(<flag is s>) {
                    //current directory and subdirectories
                    char buffer[1000];
                    getcwd(buffer, 1000);
                    char* result = findFile(buffer, filename);

                    //write to pipe fd[1]
                    //interrupt the parent
                    //close everything and return
                    
                }
                else {
                    //current directory

                }
            }
        }
        else if(<quit>) {
            //kill the children
            //wait for the children
            // quit
        }
        else {
            //invalid
        }

        //wait for the finshed children and take off list
    }
    

    return 0;
}


// find a file in a directory and subdirctory
char* findFile(char* cwd, char* filename, char* retVal) { 

    DIR *dir = opendir(cwd);
    struct dirent *entry;

    while((entry = readdir(dir) )!= NULL) {
        //for each entry check:
        // 1. does the entry name match filename?
        if(entry -> d_name is the same as filename) {
            // strcat(retVal, cwd)
        }

        // 2. is the entry a directory? recursion!
        if(entry -> d_type == DT_DIR) {
            char newcwd[1000];
            // strcat newcwd, cwd
            // strcat newcwd/<entry_name>
            // sprintf
            findFile(newcwd, filename, retVal);
        }
    }

    closedir(dir);
    return retVal;

}