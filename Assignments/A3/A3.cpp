/*******************************************************
PROGRAM NAME - Assignment - Find File

PROGRAMMER - Nathan Jaggers

DATE - 05/17/22

DESCRIPTION - This program ...
*******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>


using namespace std;

//write description
//write psuedo code
//make array to hold PIDs of children. defualt value 0? make shared mem? so all can access?

//defines
#define ARRAY_SIZE 5
#define INPUT_SIZE 100
#define PATH_SIZE 1000
#define CHILD_MAX 10

//global variables
int fd[2];
char print = 0;

//function prototypes
void welcome();
int getInput(char**);
int getArgLen(char**);
void findFile(char*, char*, char*);
void searchDir(char*, char*, char*);
void signalHandler(int i);


int main()
{
//WELCOME MESSAGE
    welcome();

//SET UP FOR COMMUNICATION BETWEEN PARENT AND CHILD
    //save standard input file descriptor
    int restore_stdin = dup(STDIN_FILENO);
    
    //set up signals
    signal(SIGUSR1, signalHandler); // to handle change from standard input to child input
    int parentPID = getpid(); //pid to send signal to parent

    //setup pipe
    pipe(fd); // fd[0] and fd[1]

    //set up shared variables
    //holds pids of children that are searching
    int *searches = (int*) mmap(NULL, sizeof(int)*CHILD_MAX, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

//OTHER SET UP
    char *parsedInput[ARRAY_SIZE];
    char  currDir[PATH_SIZE];
    char  filePaths[PATH_SIZE] = {0};
    int   searchTask[CHILD_MAX] = {0};
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
                //need to keep track of the children
                //create child to start searching for file
                //if(fork() == 0) 
                {
                    //close unused pipes
                    //close(fileDescriptor[0]);

                    if(!strcmp(parsedInput[2], "-f")) {
                        //search the root directory and subdirectories
                        //searchDir("~/", parsedInput[1], filePaths[0]);

                        //write to pipe fd[1]
                        //write(fd[1],filePaths[num] ,PATH_MAX);
                        //interrupt the parent
                        //kill(parentPID,SIGUSR1);
                        //close everything and return
                        //close pipe
                        //close(fileDescriptor[1]);
                        //return;

                    }
                    else if(!strcmp(parsedInput[2], "-s")) {
                        //search current directory and subdirectories
                        getcwd(currDir, PATH_SIZE);
                        searchDir(currDir, parsedInput[1], filePaths);

                        //write to pipe fd[1]
                        //write(fd[1],filePaths[num] ,PATH_MAX);
                        //interrupt the parent
                        //kill(parentPID,SIGUSR1);
                        //close everything and return
                        //close pipe
                        //close(fileDescriptor[1]);                       
                        //return;
                        
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
                    //close unused pipes
                    //close(fileDescriptor[0]);

                    //search for file in current directory
                    getcwd(currDir, PATH_SIZE);
                    findFile(currDir, parsedInput[1], filePaths);
                    
                    //write to pipe fd[1]
                    //write(fd[1],filePaths[num] ,PATH_MAX);
                    //interrupt the parent
                    //kill(parentPID,SIGUSR1);
                    //close everything and return
                    //close pipe
                    //close(fileDescriptor[1]);                    
                    //return;
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
            //for i < CHILD MAX, kill children
            //wait for the children
            //for i < CHILD MAX, wait();

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
        //for i < child max, if pid valid, wait no hang
        //waitpid()
    }

//QUIT CONDITION REACHED, CLEAN UP AND EXIT
    munmap(searches,sizeof(int)*CHILD_MAX);

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
        {
            arg_tokens[num_args] = token;
            //strcpy(arg_tokens[num_args], token);
        }

        //token = strtok(NULL, " ");
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

void findFile(char* cwd, char *searchName, char* fileFoundPath)
{
    //define variables to help traverse files in directory and return result
    DIR *directory;
    dirent *entry;
    //getting weird behavior with searchName. Saving data to temp.
    char temp[100];
    strcpy(temp,searchName);

    //open current directory
    directory = opendir(cwd);

    //read first dir from directory (cwd)
    entry = readdir(directory);

    //while entry is not null continue searching directory
    while(entry) 
    {
        //printf("%s %s",entry->d_name, searchName);
        //fflush(stdout);

        if ((entry->d_type == DT_REG) && (!strcmp(entry->d_name, temp)))
        {
            strcat(fileFoundPath, cwd);
            strcat(fileFoundPath, "/");
            strcat(fileFoundPath, temp);
            strcat(fileFoundPath, "\n");
        }

        // read next entry in directory
        entry = readdir(directory);
    }

    closedir(directory);

}

void searchDir(char* cwd, char *searchName, char* fileFoundPath)
{
    //weird behavior in loosing search name
    char temp[INPUT_SIZE];
    strcpy(temp,searchName);

    //search current directory
    findFile(cwd, temp, fileFoundPath);

    //search other directories
    //define variables to help traverse files in directory
    DIR *directory = opendir(cwd);
    dirent *entry= readdir(directory);

    while(entry) {
        //is the entry a directory? recursion to traverse directories!
        if(entry -> d_type == DT_DIR) 
        {
            if (!((strcmp(entry->d_name,".")==0)||(strcmp(entry->d_name,"..")==0)))
            {
                //debugging mark
                if(strcmp(entry->d_name,"Lab2")==0)
                {
                    int num = 0; 
                }

                char newcwd[PATH_SIZE] = {0};
                strcat(newcwd, cwd);
                strcat(newcwd, "/");
                strcat(newcwd, entry -> d_name); 
                searchDir(newcwd, temp, fileFoundPath);
            }

        }

        // read next entry in directory
        entry = readdir(directory);
    }

    closedir(directory);
}

void signalHandler(int i)
{
    dup2(fd[0], STDIN_FILENO);
    print = 1;
    return;
}