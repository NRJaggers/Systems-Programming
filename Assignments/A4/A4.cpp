/*******************************************************
PROGRAM NAME - Assignment 4 - Parallel Processing

PROGRAMMER - Nathan Jaggers

DATE - 05/24/22

DESCRIPTION - This program performs matrix multiplication. This program is
              Program 1 and will be called by Program 2, which is an MPI.
              Program 2 calls program 1 several times. Program 1 should share
              the work with its copies over shared memory.

COMPILE - Don't forget to link with -lrt flag at compile
*******************************************************/

// includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

// defines
#define MATRIX_DIMENSION_XY 10

// function prototypes
void set_matrix_elem(float *, int, int, float);
int quadratic_matrix_compare(float *, float *);
void quadratic_matrix_print(float *);
void quadratic_matrix_multiplication(float *, float *, float *);
void quadratic_matrix_multiplication_parallel(int, int, float*, float*, float*);
void synch(int, int, int *, int);

int main(int argc, char *argv[])
{
    //initialize random
    time_t t;
    srand((unsigned) time(&t));

    //initialize varibales
    int par_id = 0;    // the parallel ID of this process
    int par_count = 1; // the amount of processes
    float *A, *B, *C;  // matrices A,B and C
    int *ready;        // needed for synch
    int fd[4];         // file descriptor for shared memory

    // check if proper ammount of arguments called for program
    if (argc != 3)
    {
        printf("no shared\n");
    }
    else
    {
        // format of program call
        //>[program] [id] [count]
        par_id = atoi(argv[1]);
        par_count = atoi(argv[2]);

        // strcpy(shared_mem_matrix,argv[3]);
    }

    // notify if singular process
    if (par_count == 1)
    {
        printf("only one process\n");
    }


    if (par_id == 0)
    {   
        // create shared memory
        fd[0] = shm_open("matrixA", O_CREAT|O_RDWR, 0777);
        fd[1] = shm_open("matrixB", O_CREAT|O_RDWR, 0777);
        fd[2] = shm_open("matrixC", O_CREAT|O_RDWR, 0777);
        fd[3] = shm_open("synchobject", O_CREAT|O_RDWR, 0777);

        // Allocate size in the files
        ftruncate(fd[0], MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float));
        ftruncate(fd[1], MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float));
        ftruncate(fd[2], MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float));
        ftruncate(fd[3], par_count*sizeof(int));

    }
    else
    {
        sleep(2); // needed for initalizing synch

        // create shared memory
        fd[0] = shm_open("matrixA", O_RDWR, 0777);
        fd[1] = shm_open("matrixB", O_RDWR, 0777);
        fd[2] = shm_open("matrixC", O_RDWR, 0777);
        fd[3] = shm_open("synchobject", O_RDWR, 0777);

    }

    // use mmap to allocate space for shared memory
    A = (float*)mmap(NULL, MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd[0], 0);
    B = (float*)mmap(NULL, MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd[1], 0);
    C = (float*)mmap(NULL, MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd[2], 0);
    ready = (int*)mmap(NULL, par_count*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED, fd[3], 0);

    //check if memory is null
    if (A == NULL)
    {
        printf("No memory to allocate for matrix A.\n Exiting...");
        return -1;
    }
    else if (B == NULL)
    {
        printf("No memory to allocate for matrix B.\n Exiting...");
        return -1;
    }
    else if (C == NULL)
    {
        printf("No memory to allocate for matrix C.\n Exiting...");
        return -1;
    }
    else if (ready == NULL)
    {
        printf("No memory to allocate for synch ready array.\n Exiting...");
        return -1;
    }

    //synch all processes to this point
    synch(par_id, par_count, ready, 1);

    int randNum;
    if (par_id == 0)
    {
        //initialize the matrices A and B
        for(int rows = 0; rows < MATRIX_DIMENSION_XY; rows++)
        {
            for(int cols = 0; cols < MATRIX_DIMENSION_XY; cols++)
            {
                randNum = (rand()%10);
                set_matrix_elem(A, rows, cols, randNum);
                set_matrix_elem(B, rows, cols, randNum);
            }
        }
    }

    //synch all processes to this point
    synch(par_id, par_count, ready, 2);

    //perform this instances portion of matrix multiplication
    quadratic_matrix_multiplication_parallel(par_id, par_count,A,B,C);

    //synch all processes to this point
    synch(par_id, par_count, ready, 3);

    if (par_id == 0)
        quadratic_matrix_print(C);

    //synch all processes to this point
    synch(par_id, par_count, ready, 4);

    // lets test the result:
    float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
    quadratic_matrix_multiplication(A, B, M);
    if (quadratic_matrix_compare(C, M))
        printf("full points!\n");
    else
        printf("buuug!\n");

    //close and clean up
    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");
    munmap(A,MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float));
    munmap(B,MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float));
    munmap(C,MATRIX_DIMENSION_XY*MATRIX_DIMENSION_XY*sizeof(float));
    munmap(ready,par_count*sizeof(int));

    return 0;
}

//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M, int x, int y, float f)
{
    M[x + y * MATRIX_DIMENSION_XY] = f;
}
//************************************************************************************************************************
// lets see it both are the same
int quadratic_matrix_compare(float *A, float *B)
{
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            if (A[a + b * MATRIX_DIMENSION_XY] != B[a + b * MATRIX_DIMENSION_XY])
                return 0;

    return 1;
}
//************************************************************************************************************************
// print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
    {
        printf("\n");
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            printf("%.2f,", C[a + b * MATRIX_DIMENSION_XY]);
    }
    printf("\n");
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A, float *B, float *C)
{
    // nullify the result matrix first
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            C[a + b * MATRIX_DIMENSION_XY] = 0.0;
    // multiply
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)         // over all cols a
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)     // over all rows b
            for (int c = 0; c < MATRIX_DIMENSION_XY; c++) // over all rows/cols left
            {
                C[a + b * MATRIX_DIMENSION_XY] += A[c + b * MATRIX_DIMENSION_XY] * B[a + c * MATRIX_DIMENSION_XY];
            }
}
//************************************************************************************************************************
// multiply parts of two matrices
 void quadratic_matrix_multiplication_parallel(int id, int count, float *A, float *B, float *C)
 {
    // define start and stop portions for this instance
    int stop  = (((float)id+1)/count)*MATRIX_DIMENSION_XY;
    int start  = (((float)id)/count)*MATRIX_DIMENSION_XY;
    
    // nullify the result matrix first
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        for (int b = start; b < stop; b++)
            C[a + start * MATRIX_DIMENSION_XY] = 0.0;
    // multiply
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)         // over all cols a
        for (int b = start; b < stop; b++)                // over all rows b
            for (int c = 0; c < MATRIX_DIMENSION_XY; c++) // over all rows/cols left
            {
                C[a + b * MATRIX_DIMENSION_XY] += A[c + b * MATRIX_DIMENSION_XY] * B[a + c * MATRIX_DIMENSION_XY];
            }
}
//************************************************************************************************************************
void synch(int par_id, int par_count, int *ready, int ri)
{
    //ALL processes get stuck here until all ARE here
    ready[par_id]++;
    int leave = 1;

    while (leave)
    {
        leave = 0;
        for (int i = 0; i < par_count; i++)
        {
            if (ready[i] < ri)
                leave = 1;
        }
    }
}