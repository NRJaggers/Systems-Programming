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

    sleep(1);
    fd[0] = shm_open("num", O_RDWR, 0600);
    secret = (int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd[0], 0);
    usleep(5000);
    printf("The secret number is: %d\n", *secret);
   
    
    close(fd[0]);
    shm_unlink("num");
    munmap(secret, sizeof(int));
       
    return 0;
}
