/*******************************************************
PROGRAM NAME - Lab/Assignment - Title

PROGRAMMER - Nathan Jaggers

DATE - 00/00/00

DESCRIPTION - This program ...
*******************************************************/
#include <iostream>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/types.h>
#include <wait.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

int main(){

int fd = shm_open("namedSharedMem", O_RDWR, 0777);
int *p = (int*) mmap(NULL, 100*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

cout << p[0];

//clean up
close(fd);
unlink("namedSharedMem");
munmap(p,100*sizeof(int));

    return 0;
}
