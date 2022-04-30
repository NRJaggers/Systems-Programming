#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int fd[1];

int main()
{
    int *secret;
    
    // Creates shared memory
    fd[0] = shm_open("num", O_CREAT|O_RDWR, 0600);
    // Allocate size in the file
    ftruncate(fd[0], sizeof(int));
    // Mmap and make that the secret pointer
    secret = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd[0], 0);
    *secret = 5;
    
    for(;;)
    {
        // No-op
    }


   
    
    close(fd[0]);
    shm_unlink("num");
    munmap(secret, sizeof(int));
       
    return 0;
}
