/*******************************************************
PROGRAM NAME - Lab 4 - Redirecting Pipes

PROGRAMMER - Nathan Jaggers

DATE - 04/23/22

DESCRIPTION - This program reads in input from the keyboard
              and prints it out again with exclamation marks
              on both sides of the sentence. If there is no activity
              read from the keyboard for over 10 seconds, a message
              is displayed to the console indicating no activity detected.
*******************************************************/

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <wait.h>
#include <signal.h>

using namespace std;

//define size for message
#define SIZE 100

//global variables
int fileDescriptor[2];
    

//function prototypes
void signalHandler(int i);
void modifyText(char[], int);

int main()
{
    cout << "Welcome!\n"
         << "Messages in the program longer than\n"
         << "100 characters will be cut off.\n"
         << "If \"quit\" is anywhere in the message\n"
         << "text, the program will quit\n"
         << "--------------------------------------\n";

//SET UP FOR COMMUNICATION BETWEEN PARENT AND CHILD
    //save standard input file descriptor
    int restore_stdin = dup(STDIN_FILENO);
    
    //set up signals
    signal(SIGUSR1, signalHandler); // to handle change from standard input to child input

    //set up pipes
    pipe(fileDescriptor);

    //set up shared variables
    timeval *start = (timeval *) mmap(NULL, sizeof(timeval), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    timeval *end = (timeval *) mmap(NULL, sizeof(timeval), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    int *continueFlag = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    int parentPID = getpid();
    char inactive[] = "\n---Inactivity Detected---\n";

    //initialize shared vars before forking
    gettimeofday(start,NULL);
    *continueFlag = 1;

//FORK --> PARENT HANDLES USER INPUT --> CHILD HANDLES INACTIVITY
    //fork; close any unused pipes for each process
    if (fork() == 0)
    {
        //child process
        //define child variables
        //timeval *end;


        //close unused pipes
        close(fileDescriptor[0]);

        while(*continueFlag)
        {
            //check if 10 seconds have passed since last input
            gettimeofday(end,NULL);
            if ((end->tv_sec-start->tv_sec) >= 10)
            {
                //if time is greater than 10 seconds,
                //take over standard input and write
                //inactivity message to pipe
                kill(parentPID,SIGUSR1);
                write(fileDescriptor[1],inactive ,sizeof(inactive));
            }

        }

        //close pipe
        close(fileDescriptor[1]);

        return 0;
    }
    else
    {
        //parent process
        //close unused pipes
        close(fileDescriptor[1]);

        //set up parent variables
        char text[SIZE+3]; //max message size of 100 with extra space for ! and null terminator
        int bytesRead = 0;
        string test;
        int timeDiff;

        while(true)
        {
            //get input from keyboard
            bytesRead = read(STDIN_FILENO, text, SIZE);

            //update time for most recent input and indicate active parent
            gettimeofday(start,NULL);
            //timeDiff = end->tv_sec-start->tv_sec;

            //test input to see if it is quit command
            test = text;
            if (!((test.find("quit") ) == -1))
            {
                //if true, break out of while loop
                break;
            }
            else if((test.find(inactive) ) == -1)
            {
                //modify text and prepare for printing
                modifyText(text, bytesRead);
            }

            //print out what is in text buffer
            printf("%s\n",text);

            //restore parent
            dup2(restore_stdin,STDIN_FILENO);
        }

        //toggle flag and allow kid to return and exit
        *continueFlag = 0;

        //wait for child to finish
        wait(0);

    }

//QUIT CONDITION REACHED, CLEAN UP AND EXIT
    //free allocated memory
    munmap(start,sizeof(timeval));
    munmap(end,sizeof(timeval));
    munmap(continueFlag,sizeof(int));

    //close pipes
    close(fileDescriptor[0]);
    return 0;
}

void signalHandler(int i)
{
    dup2(fileDescriptor[0],STDIN_FILENO);
}

void modifyText(char message[], int messageSize)
{
    //modify input by placing ! at either end
    //also include null terminator
    for (int i=messageSize; i>0; i--)
    {
        message[i] = message[i-1]; 
    }
    message[messageSize+1] = '\0';
    message[messageSize+0] = '!';
    message[0] = '!';
}