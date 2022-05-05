#include <unistd.h>
#include <stdio.h>

#define PAGESIZE 4096
typedef unsigned char BYTE;
void *startofheap = NULL;


typedef struct structinfo
{
    int size;
    int status; // 0 for free, 1 for occupied
    BYTE *next, *prev;
}structinfo;


void analyze() 
{ 
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


// finding last chunk in heap
structinfo* get_last_chunk()
 {   
 if(!startofheap)
  return NULL;   
 structinfo* ch = (structinfo*)startofheap; 
 for (; ch->next; ch = (structinfo*)ch->next);
 return ch; 
 }



// freeing
void myfree(BYTE *address)
{
    BYTE *target_address = address - sizeof(structinfo);
    structinfo *chunk = (structinfo*) target_address;

    //free the chunk
    chunk->status = 0;

    structinfo *prev_chunk = (structinfo*)chunk->prev; // null?
    structinfo *next_chunk = (structinfo*)chunk->next; // null?

    // merge with the previous
    if(prev_chunk != NULL && prev_chunk->status == 0) {
        //merge
    }

    //merge with the next
    if(next_chunk == NULL) {
        //move the program break
    }
    else if(next_chunk->status == 0) {
        //merge
    }


    // Base case where after removing, the start of heap will be NUll
    // This will need to be modified for your own implementation!
    // BYTE *target_address = address - sizeof(structinfo);
    // structinfo *chunk = (structinfo*) target_address;
    // startofheap = NULL;
    // sbrk(-chunk->size); // Not always required, only when we need to move the program break back!
}


// allocating
BYTE* mymalloc(int demand)
{
    // calculating bytes needed to allocate (accounting for structinfo and PAGESIZE)
    int demand_bytes = demand + sizeof(structinfo);
    int page_required = demand_bytes / PAGESIZE + 1;
    int real_demand = page_required * PAGESIZE;

    // no memory allocated yet
    if (startofheap == NULL)
    {
        structinfo *chunk = (structinfo*) sbrk(sizeof(structinfo));
        chunk->size = real_demand;
        chunk->status = 1;
        startofheap = chunk;
        sbrk(real_demand - sizeof(structinfo));
        
        return (BYTE*) chunk + sizeof(structinfo);
    }

    //memory has been previously allocated (must find free memory to allocate)
    // traverse the chunks looking for best fit - is the status 0, size < real_demand
    structinfo *current = (structinfo*) startofheap;
    structinfo *bestfit = NULL; 
    while(current != NULL) {

        if(current->status == 0 && current->size >= real_demand) {
            // potentially the best-fit
            if(bestfit == NULL && bestfit->size > current->size) {
                bestfit = current;
            }
        }
        current = (structinfo*) current->next;
    }
    // bestfit the chunk you want use

    if(bestfit == NULL) {
        //create a new chunk
    }
    else {
        //does real_demand fit perfectly into chunk?
        //splitting - only if the "left over" is bigger than a pagesize
    }


    return NULL;
}



int main()
{

    return 0;
}