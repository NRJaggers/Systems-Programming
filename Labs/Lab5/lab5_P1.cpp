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

Program - This is P1.
*******************************************************/

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>  
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

        string temp;
    
        // create shared memory
        fd = shm_open("secretMessageMem", O_CREAT|O_RDWR, 0777);

        // Allocate size in the file
        ftruncate(fd, SIZE);

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

        //initialize first char as null terminator as flag for when message is ready
        secret[0] = 0;

    //GET INPUT FROM USER AND PUT IN SHARED MEMORY
        //loop until user decides to quit
        while (true)
        {
            //prompt user for input
            cout << "Enter a message to send to the other program\n"
                 << "or type quit to exit:\n>> ";
            getline(cin,temp);

            if (!((temp.find("quit") ) == -1))
            {
                //if true, break out of while loop
                break;
            }

            //transfer contents from temp to secret
            for (int i = 1; i < SIZE; i++)
            {
                secret[i] = temp[i-1];
            }

            //once transfer is finish, set first char in secret to 1
            //and indicate message is ready
            secret[0] = '1';
 
        }

    //CLEAN UP
        //close file descriptor, unlink memory and release it.
        close(fd);
        shm_unlink("secretMessageMem");
        munmap(secret, SIZE);
       
    return 0;
}

void welcome()
{
    cout << "Welcome! This is Program 1 in Name Shared Memory exercise.\n"
         << "Message size can be " << SIZE-1 << " charcters long.\n"
         << "Remember to start P1 before P2.\n\n";
}