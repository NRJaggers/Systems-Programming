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
int fileDiscriptor[2];
int continueFlag = 1;

//function prototypes
void signalHandler(int i);
void quitHandler(int i);
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
    //set up signals
    signal(SIGUSR1, signalHandler); // to handle change from standard input to child input

    //set up pipes
    pipe(fileDiscriptor);

    //set up shared variables
    time_t *start = (time_t *) mmap(NULL, sizeof(time_t), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);
    time(start);

//FORK --> PARENT HANDLES USER INPUT --> CHILD HANDLES INACTIVITY
    //fork; close any unused pipes for each process
    if (fork() == 0)
    {
        time_t *end;
        //child process
        while(continueFlag)
        {
            //check if 10 seconds have passed (since last input?)
            //can try through shared memory of time_t
            //can try through vars on in child and if statements to choose when reassigned
            //can try sleep
            time(end);
            if (difftime(*end, *start)>=10)
            //check if parent has been active through shared flag
            if(active)
            {
                //if active is true, reset flag
                //reset time?//maybe reset time in outer if statement
            }
            else
            {
                //if active is false, take over standard input
                //and write inactivity message to pipe

                //send signal to parent


            }
        }

        return 0;
    }
    else
    {
        //parent process
        //save standard input file descriptor
        int restore_stdin = dup(STDIN_FILENO);

        //set up parent variables
        char text[SIZE+3]; //max message size of 100 with extra space for ! and null terminator
        int bytesRead = 0;
        string test;

        while(true)
        {

            //get input from keyboard
            bytesRead = read(STDIN_FILENO, text, SIZE);

            //test input to see if it is quit command
            test = text;
            if (!((test.find("quit") ) == -1))
            {
                //if true, break out of while loop
                break;
            }

            //modify text and prepare for printing
            modifyText(text, bytesRead);

            //indicate parent is active either through timestamp or flag

            //print out what is in text buffer
            printf("%s\n",text);

            //restore parent (? is this when signal happens?)

        }
    }

//QUIT CONDITION REACHED, CLEAN UP AND EXIT
    //free allocated memory

    //kill child (send signal to break from loop)

    //wait for child to finish
    wait(0);
    //close pipes

    return 0;
}

void signalHandler(int i)
{
    dup2(fileDiscriptor[0],STDIN_FILENO);
}

void quitHandler(int i)
{
    continueFlag = 0;
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