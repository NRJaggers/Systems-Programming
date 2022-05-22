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

    char test1[100] = "hello there \n\0";
    char test2[] = "hey, whats up?\0";

    strcat(test1, test2);

    printf("%s",test1);

    return 0;
}
