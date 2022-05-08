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
            - need for that space anymore, it is "freed".bb
            - What happens when a space is "freed" actually
            - depends on where it is in memory. It may just
            - be marked as a free space or actually deallocated
            - from the program break.
*******************************************************/

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

using namespace std;

#define PAGESIZE 4096
typedef unsigned char BYTE;

//global variables
void *retAdd = NULL;        //var to hold return address for program break
void *startofheap = NULL;   //var to hold start address of heap

//struct definition for memory block info
typedef struct memoryBlockInfo
{
    int size;           //size of memory block
    int status;         //0 for free, 1 for occupied
    memoryBlockInfo *next, *prev;  //holds addresses to adjacent blocks of memory (starting at info address not data)

}blockInfo;

//function prototypes
void welcome();
void analyze();
BYTE* mymalloc(int demand);
void myfree(BYTE *address);
blockInfo* merge(blockInfo*,blockInfo*);
void testCase1();
void testCase2();
void testCase3();

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

    //print "no heap" if heap is empty
    if(!startofheap) 
    { 
        printf("no heap\n"); 
        printf("Program break on %p\n", sbrk(0));
        return; 
    } 

    //if not empty, store start of heap in variable and step through
    //memory chunks
    blockInfo* chunk = (blockInfo*)startofheap; 

    for (int no=0; chunk; chunk = chunk->next,no++) 
    { 
        printf("%d | current addr: %p |", no, chunk); 
        printf("size: %d | ", chunk->size); 
        printf("info: %d | ", chunk->status); 
        printf("next: %p | ", chunk->next); 
        printf("prev: %p", chunk->prev); 
        printf("      \n"); 
    } 

    //show where program break is
    printf("program break on address: %p\n\n",sbrk(0)); 
} 
//***what happens if you pass in zero? What should happen? negative? What happend and what should?
//maybe return null on zero and change int parameter to unsigned int to handle negatives? - test after getting some stuff going
BYTE* mymalloc(int demand)
{
    /**
     * allocate space in memory for desired size and return start address for chunk in memory 
     */

    //declare and initialize variables to help create new chunk in memory
    int demand_bytes = demand + sizeof(blockInfo);
    int page_required = demand_bytes / PAGESIZE + 1;
    int real_demand = page_required * PAGESIZE;

    //different cases for status of heap when adding/allocating memory for data   
    if (startofheap == NULL)
    {
        //before allocating anything, save return address for program break
        retAdd = sbrk(0);

        //move program break and initialize chunk with start of new memory info
        blockInfo *block = (blockInfo*) sbrk(sizeof(blockInfo));
        
        //fill in meta data about memory block
        block->size = real_demand;
        block->status = 1;
        block->prev = NULL;
        block->next = NULL;

        //define as start of heap
        startofheap = block;

        //move program break for data portion of block
        sbrk(real_demand - sizeof(blockInfo));
        
        //return address for start of data in memory block
        return (BYTE*) block + sizeof(blockInfo);
    }

    //if startofheap has an address, go through memory chunks and find the first best fit
    blockInfo *current = (blockInfo*) startofheap;
    blockInfo *bestfit = NULL;
    blockInfo *last = NULL;

    while(current != NULL) {
        //while steping though, keep track of last memory block
        last = current;

        //check if current chunk is free and is greater or equal to memory demand
        if(current->status == 0 && current->size >= real_demand) {
            
            //check if best fit is null or if best fit size is greater than current size
            if(bestfit == NULL) 
            {
                bestfit = current;
            }
            else if(bestfit->size > current->size)
            {
                bestfit = current;
            }
        }
        //step to next memory block
        current = current->next;
    }

    //if best fit is null, that means we must add a new memory chunk
    //if not we need to check the best fit block and our demand to determine if we split the block
    if(bestfit == NULL) {
        //create a new chunk
        //move program break and initialize chunk with start of new memory info
        blockInfo *block = (blockInfo*) sbrk(sizeof(blockInfo));
        
        //fill in meta data about memory block
        block->size = real_demand;
        block->status = 1;
        block->prev = last;
        block->next = NULL;

        //finish linking memory blocks
        last->next = block;

        //move program break for data portion of block
        sbrk(real_demand - sizeof(blockInfo));
        
        //return address for start of data in memory block
        return (BYTE*) block + sizeof(blockInfo);
    }
    else {
        //does our best fit need to be split?
        //does it have more pages than our real demand requires?

        //mark block as occupied
        bestfit->status = 1;

        //check for split
        if(bestfit->size > real_demand)
        {
            //start at best fit address and move to address after demand is met
            blockInfo *remaining = (blockInfo*)((BYTE*)bestfit + real_demand);

            //update remaining info
            remaining->size = bestfit->size - real_demand;
            remaining->status = 0; 

            //update best fit info
            bestfit->size = real_demand;

            //relink memory
            remaining->next = bestfit->next;
            remaining->prev = bestfit;
            bestfit->next = remaining;

        }

        //return address for requested memory
        return (BYTE*) bestfit + sizeof(blockInfo);

    }

    return NULL;
}
//make sure to parallel with myMalloc
//if you add a case for null there, do it here too!
void myfree(BYTE *address)
{
    /**
     * deallocate space in memory for data at input address 
     */

    //ensure address given isn't null
    if(address == NULL)
    {
        printf("\nCannot free NULL address.\n");
        return;
    }

    BYTE *target_address = address - sizeof(blockInfo);
    blockInfo *block = (blockInfo*) target_address;

    //free the chunk
    block->status = 0;

    //save pointers for next and previous from current
    blockInfo *prev_block = (blockInfo*)block->prev;
    blockInfo *next_block = (blockInfo*)block->next;

    //check if previous block exists and is free
    if(prev_block != NULL && prev_block->status == 0) {
        //previous block exits and is free. Merge previous and current block
        block = merge(prev_block, block);
    }

    //check if next block exists
    if(next_block == NULL) {

        //if previous block is also null, we are at startofheap
        //reassign startofheap to null, otherwise unlink current block from memory
        if(block->prev == NULL)
            startofheap = NULL;
        else
            block->prev->next = NULL;

        //at end of list, move program break
        sbrk(-(block->size));


    }
    else if(next_block->status == 0) {
        //next block exists and is free. Merge current and next block
        block = merge(block, next_block);

    }

}

blockInfo* merge(blockInfo *firstAddress, blockInfo *secondAddress)
{
    //ensure both addresses are not NULL
    // if(firstAddress == NULL || secondAddress == NULL)
    //     return NULL;
    
    //add the sizes of the addresses together
    firstAddress->size += secondAddress->size;

    //relink addresses
    firstAddress->next = secondAddress->next;
    if(secondAddress->next != NULL)
        secondAddress->next->prev = firstAddress;
    

    //return start address of new merged block
    return firstAddress;
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
    timeval start, end;


    //get time before starting process
    //ca = clock();
    gettimeofday(&start,NULL);

    for(int i=0;i<100;i++)
        a[i]= mymalloc(1000);

    for(int i=0;i<90;i++)
        myfree(a[i]);

    myfree(a[95]);
    a[95] = mymalloc(1000);

    for(int i=90;i<100;i++)
        myfree(a[i]);

    //get time after finishing process
    //cb = clock();
    gettimeofday(&end,NULL);

    //print time results of process
    //printf("\nduration: % f\n", (double)(cb - ca));
    printf("\nduration: % f\n", (double)(1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)));

}

void testCase3()
{

}