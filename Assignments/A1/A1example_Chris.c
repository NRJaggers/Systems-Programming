#include <unistd.h>
#include <stdio.h>
#define PAGESIZE 4096
typedef unsigned char BYTE;


void *startofheap = NULL;

typedef struct structinfo
{
    int size;
    int status;
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



void myfree(BYTE *address)
{
    // Base case where after removing, the start of heap will be NUll
    // This will need to be modified for your own implementation!
    BYTE *target_address = address - sizeof(structinfo);
    structinfo *chunk = (structinfo*) target_address;
    startofheap = NULL;
    sbrk(-chunk->size); // Not always required, only when we need to move the program break back!
}


BYTE* mymalloc(int demand)
{
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



int main()
{

    return 0;
}