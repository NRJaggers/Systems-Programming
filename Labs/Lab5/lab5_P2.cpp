/*******************************************************
PROGRAM NAME - Lab 5 - Named Shared Memory

PROGRAMMER - Nathan Jaggers

DATE - 04/30/22

DESCRIPTION - This program uses Named Shared Memory to
              send data from one program to another.
              P1 - creates shared memory and gives it 
                   input in the form of a message.
              P2 - pulls that message and prints it out
                   to the user.

Program - This is P2.
*******************************************************/

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

//define for size of bytes to allocate for message memory
#define SIZE 101

//function prototypes for program
void welcome();

int main()
{
    //WELCOME
        welcome();

    //SETUP SHARED MEMORY
        // declare file descriptor and pointer to hold message
        int fd;
        char *secret;
        char temp[SIZE-1];
    
        // create shared memory
        fd = shm_open("secretMessageMem", O_RDWR, 0777);

        // use mmap to allocate space for shared memory
        secret = (char*)mmap(NULL, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

        //check if memory is null
        if (secret == NULL)
        {
            cout << "No memory to allocate.\n Exiting...";
            return -1;
        }
    
        //notify user
        cout << "Shared memory ready!\n";

    //GET DATA FROM SHARED MEMORY AND DISPLAY TO USER
        //loop until data is ready
        cout << "Waiting...\n";
        while (secret[0] == 0)
        {/*wait here until message comes*/};

        //transfer contents from secret to temp
        for (int i = 0; i < SIZE-1; i++)
        {
            temp[i] = secret[i+1];
        }

        //print out message to user
        cout << "Here is the message from Program 1:\n"
             << "P1 << \"" << temp << "\"\n";


    //CLEAN UP
        //close file descriptor, unlink memory and release it.
        close(fd);
        shm_unlink("secretMessageMem");
        munmap(secret, SIZE);
       
    return 0;
}

void welcome()
{
    cout << "Welcome! This is Program 2 in Name Shared Memory exercise.\n"
         << "Message size can be " << SIZE-1 << " charcters long.\n"
         << "Remember to start P1 before P2.\n\n";
}