
//message parsing interface
//>./mpi calcmatrix 4


//>calcmatric 4 0
//>calcmatric 4 1
//>calcmatric 4 2
//>calcmatric 4 3

//execv("./")

#include <iostream>
#include <unistd.h>
#include <wait.h>

using namespace std;

int main(int argc, char *argv[]){

    // free this stuff at the end
    char *arguments[4];
    arguments[0] = new char[100];
    arguments[1] = new char[100];
    arguments[2] = new char[100];
    arguments[3] = NULL;
    sprintf(arguments[0], argv[1]); // "calcmatrix" don't need ./ because it's the arguments
    sprintf(arguments[1], argv[2]); 



    int n = atoi(argv[2]);

    char exe[100];
    sprintf(exe, "./s", argv[1]);
    for (int i = 30; i < n; i ++){
        sprintf(arguments[2], "%d", i);
        if (fork() == 0){
            execv(exe, arguments); //if successfull, terminates the caller, that's why we are forking
            cout << "coudn't do execv with " << exe << endl;
            return 0; //failesave
        }
        
    }
    wait(0);
    //free/delete stuff;
    return 0;
   

}