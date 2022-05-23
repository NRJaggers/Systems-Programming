/*******************************************************
PROGRAM NAME - Assignment - Find File
PROGRAMMER - Nathan Jaggers
DATE - 05/17/22
DESCRIPTION - This program searches for files asked for
              by the user.
*******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
#include <wait.h>


using namespace std;

//defines
#define ARRAY_SIZE 5
#define INPUT_SIZE 100
#define PATH_SIZE 1000
#define CHILD_MAX 10

//global variables
int fd[2];
char print = 0;
char tempSave[PATH_SIZE] = {0};

//function prototypes
void welcome();
int getInput(char**);
int getArgLen(char**);
void findFile(char*, char*, char*);
void searchDir(char*, char*, char*);
int freeChild(int*);
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
    //indicates when print is ready
    int *interruptFlag = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    //initialize vars
    for (int i = 0; i < CHILD_MAX; i++)
    {
        searches[i] = 0;
    }

    //*interruptFlag = 0;

//OTHER SET UP
    char *parsedInput[ARRAY_SIZE];
    char  currDir[PATH_SIZE];
    char  filePaths[PATH_SIZE] = {0};
    int   searchTask[CHILD_MAX] = {0};
    int argLen; 

//LOOP PARENT AND GET USER INPUT, CREATE CHILD WHEN SEARCHING FOR FILE
    while(1) 
    {
        //test if 10 children are searching
        if(freeChild(searches) == -1)
        {
            //if 10 children are searching,
            //forward until one is free
            printf("\nChild Limit Reached. Waiting for Free Child.\n");
            sleep(3);
        }
        else if (!(print))
        {
            //disable interrupts
            *interruptFlag = 0;

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
                    if(fork() == 0) 
                    {
                        //close unused pipes
                        close(fd[0]);

                        //find free child
                        int free = freeChild(searches);

                        //save child pid
                        searches[free] = getpid();

                        if(!strcmp(parsedInput[2], "-f")) {
                            //search the root directory and subdirectories
                            filePaths[0] = '\0';
                            char rootDir[] = "/";
                            char stringHold[PATH_SIZE];
                            strcpy(stringHold,parsedInput[1]);
                            searchDir(rootDir, parsedInput[1], filePaths);
                            
                            //if nothing found, return message
                            if (filePaths[0] == '\0')
                            {
                                strcat(filePaths, "/");
                                strcat(filePaths, stringHold);
                                strcat(filePaths, " file was not found\n");
                            }

                            //wait till interrupt flag is high
                            while(interruptFlag == 0){};

                            //interrupt the parent
                            kill(parentPID,SIGUSR1);
                            //write to pipe fd[1]
                            write(fd[1],filePaths, PATH_SIZE);
                            //close everything and return
                            //close pipe
                            close(fd[1]);
                            //return;
                            return 0;

                        }
                        else if(!strcmp(parsedInput[2], "-s")) {
                            //search current directory and subdirectories
                            getcwd(currDir, PATH_SIZE);
                            filePaths[0] = '\0';
                            char stringHold[PATH_SIZE];
                            strcpy(stringHold,parsedInput[1]);
                            searchDir(currDir, parsedInput[1], filePaths);

                            //if nothing found, return message
                            if (filePaths[0] == '\0')
                            {
                                strcat(filePaths, "/");
                                strcat(filePaths, stringHold);
                                strcat(filePaths, " file was not found\n");
                            }

                            //wait till interrupt flag is high
                            while(interruptFlag == 0){};
                            
                            //interrupt the parent
                            kill(parentPID,SIGUSR1);
                            //write to pipe fd[1]
                            write(fd[1],filePaths, PATH_SIZE);
                            //close everything and return
                            //close pipe
                            close(fd[1]);
                            //return;
                            return 0;
                            
                        }
                        else {
                            //invalid flag
                            printf("Invalid flag. Please try again\n");
                        }
                    }
                }
                else if (argLen == 2)
                {
                    if(fork() == 0) 
                    {
                        //close unused pipes
                        close(fd[0]);

                        //find free child
                        int free = freeChild(searches);

                        //save child pid
                        searches[free] = getpid();

                        //search for file in current directory
                        getcwd(currDir, PATH_SIZE);
                        filePaths[0] = '\0';
                        char stringHold[PATH_SIZE];
                        strcpy(stringHold,parsedInput[1]);
                        findFile(currDir, parsedInput[1], filePaths);
                        
                        //if nothing found, return message
                        if (filePaths[0] == '\0')
                        {
                            strcat(filePaths, "/");
                            strcat(filePaths, stringHold);
                            strcat(filePaths, " file was not found\n");
                        }

                        //wait till interrupt flag is high
                        while(interruptFlag == 0){};
                            
                        //interrupt the parent
                        kill(parentPID,SIGUSR1);
                        //write to pipe fd[1]
                        write(fd[1],filePaths, PATH_SIZE);
                        //close everything and return
                        //close pipe
                        close(fd[1]);
                        //return;
                        return 0;
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
                for(int i = 0; i < CHILD_MAX; i++)
                {
                    if (searches[i] != 0)
                    {
                        kill(searches[i],SIGKILL);
                    }
                }
                //wait for the children
                int status;
                for(int i = 0; i < CHILD_MAX; i++)
                {
                    if (searches[i] != 0)
                    {
                        int endId = waitpid(searches[i], &status, WNOHANG);
                    }
                }

                //print complete message and exit
                printf("Done.\nExiting...\n");

                //break out of loop and finish program
                break;
            }
            else 
            {
                if (strcmp(tempSave,"\0") == 0)
                {
                    //invalid command
                    printf("Invalid command. Please try again\n");
                }

            }

            //enable interrupts
            *interruptFlag = 1;
        }
        else
        {
            char text[1000];
            if (strcmp(tempSave,"\0") == 0)
            {
                //read from pipe and print out results
                read(STDIN_FILENO, text, PATH_SIZE);
            }
            else
            {
                strcpy(text, tempSave);
            }

            //print found paths
            printf("%s",text);

            //reset print flag
            print = 0;

            //do this till print stays zero?

            //reset temp save
            strcpy(tempSave,"\0");

            //restore parent
            dup2(restore_stdin,STDIN_FILENO);


        }



        //wait for the finshed children and take off list
        int status;
        for(int i = 0; i < CHILD_MAX; i++)
        {
            if (searches[i] != 0)
            {
                int endId = waitpid(searches[i], &status, WNOHANG);
                if(endId != 0)
                {
                    searches[i] = 0;
                }
            }
        }
    }

//QUIT CONDITION REACHED, CLEAN UP AND EXIT
    close(fd[0]);
    close(fd[1]);
    munmap(searches,sizeof(int)*CHILD_MAX);
    munmap(interruptFlag,sizeof(int));

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
    char input[PATH_SIZE];
    int bytesRead = read(STDIN_FILENO, input, PATH_SIZE);
    input[bytesRead-1] = '\0';

    if (input[0] == '/')
        {
            strcpy(tempSave,input);
            strcpy(arg_tokens[0],"\0");
            //arg_tokens[0] = "\0";
            return 0;
        }

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
    DIR *directory = NULL;
    dirent *entry = NULL;
    //getting weird behavior with searchName. Saving data to temp.
    char temp[100];
    strcpy(temp,searchName);

    //open current directory
    directory = opendir(cwd);

    //read first dir from directory (cwd)
    if(directory != NULL)
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
            strcat(fileFoundPath, "\n\0");
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
    dirent *entry = NULL;
    if(directory != NULL)
        entry = readdir(directory);

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

int freeChild(int *kidsPIDs)
{
    int freeIndex = -1;

    for(int i = 0; i < CHILD_MAX; i++)
    {
        if(kidsPIDs[i] == 0)
        {
            freeIndex = i;
            break;
        }
    }

    return freeIndex;
}

void signalHandler(int i)
{
    dup2(fd[0], STDIN_FILENO);
    print = 1;
    return;
}
