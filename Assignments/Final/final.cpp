/*******************************************************
PROGRAM NAME - Final - Matrix Multiplication on BMPs

PROGRAMMER - Nathan Jaggers

DATE - 05/31/22

DESCRIPTION - This program performs matrix multiplication on two BMPs.
              This program is Program 1 and will be called by Program 2,
              which is an MPI. Program 2 can call Program 1 several times.
              Program 1 should share the work with its copies over shared memory.

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
#include <sys/time.h>

// defines
#define MATRIX_DIMENSION_XY 10

#define BLUE 0
#define GREEN 1
#define RED 2

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

typedef struct BITMAPFILEHEADER
{
    WORD bfType;      // specifies the file type
    DWORD bfSize;     // specifies the size in bytes of the bitmap file
    WORD bfReserved1; // reserved; must be 0
    WORD bfReserved2; // reserved; must be 0
    DWORD bfOffBits;  // species the offset in bytes from the bitmapfileheader to the bitmap bits
} BMFH;

typedef struct BITMAPINFOHEADER
{
    DWORD biSize;         // specifies the number of bytes required by the struct
    LONG biWidth;         // specifies width in pixels
    LONG biHeight;        // species heightin pixels
    WORD biPlanes;        // specifies the number of color planes, must be 1
    WORD biBitCount;      // specifies the number of bit per pixel
    DWORD biCompression;  // spcifies the type of compression
    DWORD biSizeImage;    // size of image in bytes
    LONG biXPelsPerMeter; // number of pixels per meter in x axis
    LONG biYPelsPerMeter; // number of pixels per meter in y axis
    DWORD biClrUsed;      // number of colors used by the bitmap
    DWORD biClrImportant; // number of colors that are important
} BMIH;

typedef struct BITMAPFILE
{
    BMFH fileHeader; // holds image file header data
    BMIH infoHeader; // holds image info header data
    BYTE *imageData; // holds image data
} BMP;

// function prototypes
void set_matrix_elem(float *, int, int, float);
int quadratic_matrix_compare(float *, float *);
void quadratic_matrix_print(float *);
void quadratic_matrix_multiplication(float *, float *, float *);
void quadratic_matrix_multiplication_parallel(int, int, float *, float *, float *);
void quadratic_matrix_multiplication_parallel_images(int, int, BMP, BMP, BMP);
void synch(int, int, int *, int);
void readHeaders(BMFH &, BMIH &, FILE *);
void writeHeaders(BMFH &, BMIH &, FILE *);
BMP readImageInfo(FILE *);
void writeImage(BMP, char *);

int main(int argc, char *argv[])
{
    // initialize varibales
    int par_id = 0;             // the parallel ID of this process
    int par_count = 1;          // the amount of processes
    float *A, *B, *C;           // matrices A,B and C
    int *ready;                 // needed for synch
    int fd[4];                  // file descriptor for shared memory
    timeval start, end;         // timer for calculations
    BMP image1, image2, image3; // vars to store image data

    // check if proper ammount of arguments called for program parallel processing
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
    }

    // notify if singular process
    if (par_count == 1)
    {
        printf("only one process\n");
    }

    // read in image meta data here? use metadata to declare sizes of allocated memory down the line
    FILE *file1 = fopen("f1.bmp", "rb");
    FILE *file2 = fopen("f2.bmp", "rb");

    image1 = readImageInfo(file1);
    image2 = readImageInfo(file2);
    image3.fileHeader = image1.fileHeader;
    image3.infoHeader = image1.infoHeader;
    image3.imageData = NULL;

    if (par_id == 0)
    {
        // create shared memory
        fd[0] = shm_open("matrixA", O_CREAT | O_RDWR, 0777);
        fd[1] = shm_open("matrixB", O_CREAT | O_RDWR, 0777);
        fd[2] = shm_open("matrixC", O_CREAT | O_RDWR, 0777);
        fd[3] = shm_open("synchobject", O_CREAT | O_RDWR, 0777);

        // Allocate size in the files
        ftruncate(fd[0], image1.infoHeader.biSizeImage);
        ftruncate(fd[1], image2.infoHeader.biSizeImage);
        ftruncate(fd[2], image3.infoHeader.biSizeImage);
        ftruncate(fd[3], par_count * sizeof(int));
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
    image1.imageData = (BYTE *)mmap(NULL, image1.infoHeader.biSizeImage, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
    image2.imageData = (BYTE *)mmap(NULL, image2.infoHeader.biSizeImage, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
    image3.imageData = (BYTE *)mmap(NULL, image3.infoHeader.biSizeImage, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
    ready = (int *)mmap(NULL, par_count * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);

    // check if memory is null
    if (image1.imageData == NULL)
    {
        printf("No memory to allocate for image 1.\n Exiting...");
        return -1;
    }
    else if (image2.imageData == NULL)
    {
        printf("No memory to allocate for image 2.\n Exiting...");
        return -1;
    }
    else if (image3.imageData == NULL)
    {
        printf("No memory to allocate for image 3.\n Exiting...");
        return -1;
    }
    else if (ready == NULL)
    {
        printf("No memory to allocate for synch ready array.\n Exiting...");
        return -1;
    }

    // synch all processes to this point
    synch(par_id, par_count, ready, 1);

    // initialize the matrices A and B
    if (par_id == 0)
    {
        // read in data from images
        fread(image1.imageData, image1.infoHeader.biSizeImage, 1, file1);
        fread(image2.imageData, image2.infoHeader.biSizeImage, 1, file2);
    }

    // close input file
    fclose(file1);
    fclose(file2);

    // synch all processes to this point
    synch(par_id, par_count, ready, 2);

    // start timer for calculations 
    gettimeofday(&start,NULL);

    // perform this instances portion of matrix multiplication
    quadratic_matrix_multiplication_parallel_images(par_id, par_count, image1, image2, image3);

    // synch all processes to this point
    synch(par_id, par_count, ready, 3);
    
    // end timer for calculations 
    gettimeofday(&end,NULL);

    // take results and write to output file
    if (par_id == 0)
    {
        // write out data from image
        char outFile[] = "result.bmp";
        writeImage(image3, outFile);
        printf("\nduration: %f usec\n", (double)(1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)));

    }

    // close and clean up
    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");
    munmap(image1.imageData, image1.infoHeader.biSizeImage);
    munmap(image2.imageData, image2.infoHeader.biSizeImage);
    munmap(image3.imageData, image3.infoHeader.biSizeImage);
    munmap(ready, par_count * sizeof(int));

    // exit
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
    int stop = (((float)id + 1) / count) * MATRIX_DIMENSION_XY;
    int start = (((float)id) / count) * MATRIX_DIMENSION_XY;

    // nullify the result matrix first
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        for (int b = start; b < stop; b++)
            C[a + b * MATRIX_DIMENSION_XY] = 0.0;
    // multiply
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)         // over all cols a
        for (int b = start; b < stop; b++)                // over all rows b
            for (int c = 0; c < MATRIX_DIMENSION_XY; c++) // over all rows/cols left
            {
                C[a + b * MATRIX_DIMENSION_XY] += A[c + b * MATRIX_DIMENSION_XY] * B[a + c * MATRIX_DIMENSION_XY];
            }
}
//************************************************************************************************************************
// multiply parts of two matrices
void quadratic_matrix_multiplication_parallel_images(int id, int count, BMP imageA, BMP imageB, BMP imageC)
{
    // define start and stop portions for this instance
    int stop = (((float)id + 1) / count) * imageA.infoHeader.biHeight;
    int start = (((float)id) / count) * imageA.infoHeader.biHeight;

    // check for padding on width of pixels
    int widthBytes = imageA.infoHeader.biWidth * 3; // pixels*(Byte/pixels)
    if (widthBytes % 4 != 0)
    {
        widthBytes += (4 - (widthBytes % 4));
    }

    int offsetA = 0, offsetB = 0, offsetC = 0;
    float rA, gA, bA, rB, gB, bB, rC, gC, bC;
    ;

    // nullify the result matrix first
    for (int a = 0; a < imageC.infoHeader.biWidth; a++) // controls cols
        for (int b = start; b < stop; b++)              // controls rows
        {
            offsetC = a * 3 + b * widthBytes;
            imageC.imageData[offsetC + BLUE] = (BYTE)0.0;
            imageC.imageData[offsetC + GREEN] = (BYTE)0.0;
            imageC.imageData[offsetC + RED] = (BYTE)0.0;
        }

    // multiply
    for (int a = 0; a < imageC.infoHeader.biWidth; a++)         // over all cols a
        for (int b = start; b < stop; b++)                      // over all rows b
            for (int c = 0; c < imageC.infoHeader.biWidth; c++) // over all rows/cols left
            {
                // determine offsets
                offsetA = c * 3 + b * widthBytes;
                offsetB = a * 3 + c * widthBytes;
                offsetC = a * 3 + b * widthBytes;

                // get image values for colors and normalize
                bA = ((float)imageA.imageData[offsetA + BLUE]) / 255;
                gA = ((float)imageA.imageData[offsetA + GREEN]) / 255;
                rA = ((float)imageA.imageData[offsetA + RED]) / 255;

                bB = ((float)imageB.imageData[offsetB + BLUE]) / 255;
                gB = ((float)imageB.imageData[offsetB + GREEN]) / 255;
                rB = ((float)imageB.imageData[offsetB + RED]) / 255;

                bC = 0.03 * (bA * bB) * 255;
                gC = 0.03 * (gA * gB) * 255;
                rC = 0.03 * (rA * rB) * 255;

                imageC.imageData[offsetC + BLUE] += (BYTE)(bC);
                imageC.imageData[offsetC + GREEN] += (BYTE)(gC);
                imageC.imageData[offsetC + RED] += (BYTE)(rC);
            }
}
//************************************************************************************************************************
void synch(int par_id, int par_count, int *ready, int ri)
{
    // ALL processes get stuck here until all ARE here
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
//************************************************************************************************************************
void readHeaders(BMFH &filehead, BMIH &infohead, FILE *readFile)
{
    // fread(void *dest, int numBytes, int times, FILE *file)
    // read in data one field at a time to avoid padding issues
    fread(&filehead.bfType, sizeof(WORD), 1, readFile);
    fread(&filehead.bfSize, sizeof(DWORD), 1, readFile);
    fread(&filehead.bfReserved1, sizeof(WORD), 1, readFile);
    fread(&filehead.bfReserved2, sizeof(WORD), 1, readFile);
    fread(&filehead.bfOffBits, sizeof(DWORD), 1, readFile);

    // no need to read in data one at a time because all fields take
    // up a complete memory cell. No padding to account for
    fread(&infohead, sizeof(infohead), 1, readFile);
};
//************************************************************************************************************************
void writeHeaders(BMFH &filehead, BMIH &infohead, FILE *writeFile)
{
    // fwrite(void *source, int numBytes, int times, FILE *file)
    fwrite(&filehead.bfType, sizeof(WORD), 1, writeFile);
    fwrite(&filehead.bfSize, sizeof(DWORD), 1, writeFile);
    fwrite(&filehead.bfReserved1, sizeof(WORD), 1, writeFile);
    fwrite(&filehead.bfReserved2, sizeof(WORD), 1, writeFile);
    fwrite(&filehead.bfOffBits, sizeof(DWORD), 1, writeFile);
    fwrite(&infohead, sizeof(infohead), 1, writeFile);
};
//************************************************************************************************************************
BMP readImageInfo(FILE *file)
{
    /**
     * reads data into a BMP struct and returns results
     */

    // declare image object to return
    BMP image;
    image.imageData = NULL;

    // check for proper open
    if (file == NULL)
    {
        printf("Could not open file, please try again\n");
        return image;
    }

    // read in data
    readHeaders(image.fileHeader, image.infoHeader, file);

    return image;
}
//************************************************************************************************************************
void writeImage(BMP image, char *output_image_filename)
{
    /**
     * writes data from a BMP struct into a file
     */

    // WRITE DATA
    // open file using output.bmp command line arg as write binary
    FILE *newFile = fopen(output_image_filename, "wb");

    // check for proper open
    if (newFile == NULL)
    {
        printf("Could not open file, please try again\n");
        return;
    }

    // write info and data to newfile
    writeHeaders(image.fileHeader, image.infoHeader, newFile);
    fwrite(image.imageData, image.infoHeader.biSizeImage, 1, newFile);

    // close file
    fclose(newFile);
}
//************************************************************************************************************************