/*******************************************************
PROGRAM NAME - Final - Message Passing Interface for BMPs

PROGRAMMER - Nathan Jaggers

DATE - 05/31/22

DESCRIPTION - This program calls the matrix multiply for BMP function
              multiple times
*******************************************************/

#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[]){

    /** 
     * argv[0] - //name of this program
     * argv[1] - //name of program to be executed
     * argv[2] - //number of instances to run
     */

    //allocate variables to pass to execv
    char *arguments[4];
    arguments[0] = new char[100]; //name of program to be executed
    arguments[1] = new char[100]; //number of current instance
    arguments[2] = new char[100]; //number of instances to run
    arguments[3] = NULL;          //null to terminate array



    //grab input from terminal and pass into arguments array
    strcpy(arguments[0], argv[1]);
    strcpy(arguments[2], argv[2]); 

    //convert max number of instances to number for looping purposes
    int n = atoi(argv[2]);

    //allocate array to hold pids of children
    int *children = new int[n];

    //format first argument for execv
    char exe[100] = {0};
    strcat(exe, "./");
    strcat(exe, arguments[0]);

    //loop through to run multiple instances of program 
    for (int i = 0; i < n; i ++)
    {
        //format current instance and place in arguments array
        sprintf(arguments[1], "%d", i);

        //fork because execv kills calling process on success
        children[i] = fork(); //parent saves pids of children
        if (children[i] == 0)
        {
            execv(exe, arguments); //if successfull, terminates the caller, that's why we are forking

            //if not successful, notify and return.
            cout << "coudn't do execv with " << exe << endl;
            return 0; //failesave
        }
        
    }

    //wait for all children to finish
    int allDone = 0, status, endId;
    while(allDone == 0)
    {
        //set flag to true and flip if all processes havent finished
        allDone = 1;

        //iterate through children
        for(int i = 0; i < n; i++)
        {
            //if at index i, there is a pid, process it
            if (children[i] != 0)
            {
                endId = waitpid(children[i], &status, WNOHANG);
                //if successfull wait, take pid off list
                //else flip flag because child is not finished
                if(endId != 0)
                {
                    children[i] = 0;
                }
                else
                {
                    allDone = 0;
                }
            }
        }
    }
    
    //free/delete stuff;
    delete []arguments[0];
    delete []arguments[1];
    delete []arguments[2];
    delete []children;

    return 0;

}