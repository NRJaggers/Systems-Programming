/*******************************************************
PROGRAM NAME - Assignment 1 - Memory Management
                            - Creating myMalloc and myFree

PROGRAMMER - Nathan Jaggers

DATE - 05/03/22

DESCRIPTION - This program manages memory and simulates
            - predefined malloc and free through local
            - definitions of myMalloc and myFree. When 
            - memory is requested, the ammount is analyzed
            - and an allocated free space or new page(s)
            - is created to store data. When there is no
            - need for that space anymore, it is "freed".
            - What happens when a space is "freed" actually
            - depends on where it is in memory. It may just
            - be marked as a free space or actually deallocated
            - from the program break.
*******************************************************/

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

using namespace std;

#define PAGESIZE 4096
typedef unsigned char BYTE;

//global variables
void *startofheap = NULL;

//struct definition for memory chunck
typedef struct structinfo
{
    int size;
    int status;
    BYTE *next, *prev;
}structinfo;

//function prototypes
void welcome();
void analyze();
BYTE* mymalloc(int demand);
void myfree(BYTE *address);
void testCase1();
void testCase2();

int main()
{
//WELCOME MESSAGE
    welcome();

//RUN DESIRED TEST
    //run test 1 - test for proper operation. Code as seen in Assignment document 
    //testCase1();

    //run test 2 - test for speed of operation. Code as seen in Assignment document
    //testCase2();

    //run other tests - feel free to run other tests here


//EXIT
    return 0;
}

void welcome()
{
    /**
     * Prints welcome message for user
     */

    cout << "Hello! Welcome to Assignment 1 where we create and simulate\n"
         << "our own malloc and free functions.\n\n";
}

void analyze() 
{ 
    /**
     * Prints current data in heap
     */

    printf("\n--------------------------------------------------------------\n"); 
    if(!startofheap) 
    { 
        printf("no heap\n"); 
        printf("Program break on %p\n", sbrk(0));
        return; 
    } 

    structinfo* ch = (structinfo*)startofheap; 

    for (int no=0; ch; ch = (structinfo*)ch->next,no++) 
    { 
        printf("%d | current addr: %p |", no, ch); 
        printf("size: %d | ", ch->size); 
        printf("info: %d | ", ch->status); 
        printf("next: %p | ", ch->next); 
        printf("prev: %p", ch->prev); 
        printf("      \n"); 
    } 

    printf("program break on address: %p\n\n",sbrk(0)); 
} 

BYTE* mymalloc(int demand)
{
    /**
     * allocate space in memory for desired size and return start address for chunck in memory 
     */

    int demand_bytes = demand + sizeof(structinfo);
    int page_required = demand_bytes / PAGESIZE + 1;
    int real_demand = page_required * PAGESIZE;

    if (startofheap == NULL)
    {
        structinfo *chunk = (structinfo*) sbrk(sizeof(structinfo));
        chunk->size = real_demand;
        chunk->status = 1;
        startofheap = chunk;
        sbrk(real_demand - sizeof(structinfo));
        
        return (BYTE*) chunk + sizeof(structinfo);
    }

    return NULL;
}

void myfree(BYTE *address)
{
    /**
     * deallocate space in memory for data at input address 
     */

    // Base case where after removing, the start of heap will be NUll
    // This will need to be modified for your own implementation!
    BYTE *target_address = address - sizeof(structinfo);
    structinfo *chunk = (structinfo*) target_address;
    startofheap = NULL;
    sbrk(-chunk->size); // Not always required, only when we need to move the program break back!
}

void testCase1()
{
    /**
     * This function runs code to test the myMalloc and myFree functions
     * to observe if they run properly or not. 
     */

    BYTE*a[100];

    analyze();//50% points
    for(int i=0;i<100;i++)
        a[i]= mymalloc(1000);
    for(int i=0;i<90;i++)
        myfree(a[i]);

    analyze(); //50% of points if this is correct
    myfree(a[95]);
    a[95] = mymalloc(1000);

    analyze();//25% points, this new chunk should fill the smaller free one (best fit)
    for(int i=90;i<100;i++)
        myfree(a[i]);
    analyze();// 25% should be an empty heap now with the start address from the program start

}

void testCase2()
{
    /**
     * Runs a timing analysis on the program to evalute effecient runtimes.
     */

    //create byte pointer and clock variables to hold data and time
    BYTE*a[100];
    clock_t ca, cb;

    //get time before starting process
    ca = clock();

    for(int i=0;i<100;i++)
        a[i]= mymalloc(1000);

    for(int i=0;i<90;i++)
        myfree(a[i]);

    myfree(a[95]);
    a[95] = mymalloc(1000);

    for(int i=90;i<100;i++)
        myfree(a[i]);

    //get time after finishing process
    cb = clock();
    
    //print time results of process
    printf("\nduration: % f\n", (double)(cb - ca));

}