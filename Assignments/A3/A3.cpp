/*******************************************************
PROGRAM NAME - Assignment - Find File

PROGRAMMER - Nathan Jaggers

DATE - 05/17/22

DESCRIPTION - This program ...
*******************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>


using namespace std;

//write description
//write psuedo code
//make array to hold PIDs of children. defualt value 0? make shared mem? so all can access?

//defines
#define ARRAY_SIZE 5
#define INPUT_SIZE 100
#define PATH_SIZE 1000
//global variables

//function prototypes
void welcome();
int getInput(char**);
int getArgLen(char**);
char* findFile(char*, char*);
char* searchDir(char*, char*);


int main()
{
//WELCOME MESSAGE
    welcome();
//SET UP FOR COMMUNICATION BETWEEN PARENT AND CHILD
    //set up pipes and memory and whatever

//OTHER SET UP?
    //other stuff? like char arrays or whatever for input?
    char *parsedInput[ARRAY_SIZE];
    char* result, currDir[PATH_SIZE]; 
    int argLen; 

//LOOP PARENT AND GET USER INPUT, CREATE CHILD WHEN SEARCHING FOR FILE
    while(1) 
    {
        //print shell
        printf("A3:findfile$ ");
        fflush(stdout);

        //get parsed input from user
        argLen = getInput(parsedInput);

        //determine which command was input
        if(!strcmp(parsedInput[0], "find")) {
            //process request based on ammount of arguments
            if(argLen == 3)
            {
                // need to keep track of the children
                //create child to start searching for file
                //if(fork() == 0) 
                {

                    if(!strcmp(parsedInput[2], "-f")) {
                        //search the root directory and subdirectories
                        result = searchDir("/", parsedInput[1]);

                        //write to pipe fd[1]
                        //interrupt the parent
                        //close everything and return

                    }
                    else if(!strcmp(parsedInput[2], "-s")) {
                        //search current directory and subdirectories
                        getcwd(currDir, PATH_SIZE);
                        result = searchDir(currDir, parsedInput[1]);

                        //write to pipe fd[1]
                        //interrupt the parent
                        //close everything and return
                        
                    }
                    else {
                        //invalid flag
                        printf("Invalid flag. Please try again\n");
                    }
                }
            }
            else if (argLen == 2)
            {
                //if(fork() == 0) 
                {
                    //search for file in current directory
                    getcwd(currDir, PATH_SIZE);
                    result = findFile(currDir,parsedInput[2]);

                    //write to pipe fd[1]
                    //interrupt the parent
                    //close everything and return
                }
            }
            else if (argLen == 1)
            {
                printf("Not enough aruments. Please provide filename."
                       "\nPlease try again.\n");
            }
            else
            {
                printf("Too many aruments.\nPlease try again.\n");
            }

        }
        else if((!strcmp(parsedInput[0], "quit")) || (!strcmp(parsedInput[0], "q"))) 
        {
            //notify start of quit
            printf("Quitting...\n");

            //kill the children and stop their processes
            //wait for the children

            //print complete message and exit
            printf("Done.\nExiting...\n");

            //break out of loop and finish program
            break;
        }
        else 
        {
            //invalid command
            printf("Invalid command. Please try again\n");
        }

        //wait for the finshed children and take off list
    }

//QUIT CONDITION REACHED, CLEAN UP AND EXIT

    return 0;
}

void welcome()
{
    printf("Welcome to Assignment 3! Use this program to search for a file.\n"
           "---------------------------------------------------------------\n\n");
}

int getInput(char *arg_tokens[ARRAY_SIZE])
{
    /** 
     * read input from user, break it up into tokens.
     * return amount of tokens created.
     */

    //read in input from user
    char input[INPUT_SIZE];
    int bytesRead = read(STDIN_FILENO, input, INPUT_SIZE);
    input[bytesRead-1] = '\0';

    //break up input and return tokens
    int num_args = 0; //num of arguments entered
    char *token = strtok(input, " ");

    while(token != NULL)
    {
        //if token will fit in array, place it in
        if (num_args<ARRAY_SIZE)
            arg_tokens[num_args] = token;

        token = strtok(NULL, " ");
        num_args++;
    }

    return num_args;
}

int getArgLen(char **arg_tokens)
{
    int i = 0;
    while (arg_tokens[i])
        i++;

    return i;
}

char* findFile(char*, char*)
{

}

char* searchDir(char*, char*)
{

}