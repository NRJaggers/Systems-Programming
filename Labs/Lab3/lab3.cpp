/*******************************************************
PROGRAM NAME - Lab 3 - Bitmap file color grading

PROGRAMMER - Nathan Jaggers

DATE - 04/16/22

DESCRIPTION - This program takes in command line arguments
              for an input bmp image, color grading values (RGB)
              and an output bmp image. It then works with the 
              input bmp and changes color values of pixels based
              on the input. The time to complete this process is
              measured with and without fork. The data is written
              to an output file specified by the user.
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <wait.h>

using namespace std;

#define BLUE  0
#define GREEN 1
#define RED   2

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

typedef struct BITMAPFILEHEADER
{
    WORD bfType;  //specifies the file type
    DWORD bfSize;  //specifies the size in bytes of the bitmap file
    WORD bfReserved1;  //reserved; must be 0
    WORD bfReserved2;  //reserved; must be 0
    DWORD bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BMFH;


typedef struct BITMAPINFOHEADER
{
    DWORD biSize;  //specifies the number of bytes required by the struct
    LONG biWidth;  //specifies width in pixels
    LONG biHeight;  //species heightin pixels
    WORD biPlanes;  //specifies the number of color planes, must be 1
    WORD biBitCount;  //specifies the number of bit per pixel
    DWORD biCompression;  //spcifies the type of compression
    DWORD biSizeImage;  //size of image in bytes
    LONG biXPelsPerMeter;  //number of pixels per meter in x axis
    LONG biYPelsPerMeter;  //number of pixels per meter in y axis
    DWORD biClrUsed;  //number of colors used by the bitmap
    DWORD biClrImportant;  //number of colors that are important
}BMIH;

void readHeaders(BMFH& , BMIH&, FILE*);
void writeHeaders(BMFH& , BMIH&, FILE*);
void changeColorGrading(BMIH&, BYTE*, BYTE*, float, float, float, int, int);



// ./yourprogram [IMAGEFILE] [COLOR GRADING] [OUTPUTFILE]

// [IMAGEFILE] that’s your bitmap
// [COLOR GRADING] three float number between 0 and 1 representing red, green and blue (RGB)
// [OUTPUTFILE] the output file

// ./lab3   [input.bmp] [R] [G] [B] [output.bmp] 
// argv[0]  argv[1]     argv[2:4]   argv[5] 


int main(int argc, char** argv)
{
    //organize command line inputs
    char *inputfile  = argv[1];
    float redGrade   = atof(argv[2]);
    float greenGrade = atof(argv[3]);
    float blueGrade  = atof(argv[4]);
    char *outputfile = argv[5];

//READ DATA
    //open file using input.bmp command line arg as read binary
    FILE *file = fopen(inputfile, "rb");

    //check for proper open
    if (file == NULL)
    {
        printf("Could not open file, please try again\n");
        return -1; 
    }

    //read in data
    //create instance of BITMAPFILEHEADER and read in info from bmp
    BMFH fileHeader;
    //create instance of BITMAPINFOHEADER and read in info from bmp
    BMIH infoHeader;
    readHeaders(fileHeader, infoHeader, file);

//ALLOCATE MEMORY TO STORE DATA
    //allocate memory to store read in data
    BYTE *inputData = (BYTE*) malloc(infoHeader.biSizeImage);

    //read in image data
    fread(inputData, infoHeader.biSizeImage, 1, file);

    //close input file
    fclose(file);

    //allocate memory to write to for no fork process - outputData1
    //void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    BYTE* outputData1 = (BYTE*) mmap(NULL, infoHeader.biSizeImage, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);

    //allocate memory to write to for fork process - outputData2
    //void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    BYTE* outputData2 = (BYTE*) mmap(NULL, infoHeader.biSizeImage, PROT_READ | PROT_WRITE, MAP_ANON | MAP_SHARED, -1, 0);


//MANIPULATE DATA W/O FORK, MEASURE TIME
    //define and start clock
    timeval start, end;
    gettimeofday(&start,NULL);

    //change pixels
    changeColorGrading(infoHeader, inputData, outputData1, redGrade, greenGrade, blueGrade, 0, infoHeader.biHeight);

    //stop clock
    gettimeofday(&end, NULL);

    //print results in usec
    long time = (1000000)*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
    printf("No Fork Process took %ld us to complete.\n",time);

//MANIPULATE DATA W/ FORK, MEASURE TIME    
    //create split point for parent/child
    int rowHalf = infoHeader.biHeight/2;
    
    //restart clock
    gettimeofday(&start, NULL);

    //fork and check pid
    if(fork() == 0)
    {
        //child fork  - work on lower half of bmp
        changeColorGrading(infoHeader, inputData, outputData2, redGrade, greenGrade, blueGrade, rowHalf, infoHeader.biHeight);

        //once done, return
        return 0;
    }
    else
    {
        //parent fork - work on upper half of bmp
        changeColorGrading(infoHeader, inputData, outputData2, redGrade, greenGrade, blueGrade, 0, rowHalf);
        
        //once done, wait for child
        wait(0);
    }

    //stop clock
    gettimeofday(&end, NULL);

    //print results in usec
    time = (1000000)*(end.tv_sec-start.tv_sec) + (end.tv_usec-start.tv_usec);
    printf("Fork Process took %ld us to complete.\n",time);

//WRITE DATA
    //open file using output.bmp command line arg as write binary
     FILE *newFile = fopen(outputfile, "wb");

    //check for proper open
     if (newFile == NULL)
     {
         printf("Could not open file, please try again\n");
         return -1; 
     }

    //write info and data to newfile
    writeHeaders(fileHeader, infoHeader, newFile);
    fwrite(outputData2, infoHeader.biSizeImage, 1, newFile);

    //close file
     fclose(newFile);

//CLEAN UP
    //close any open files
    //release any allocated memory
    free(inputData); //input data array
    munmap(outputData1,infoHeader.biSizeImage); //output data for no fork process
    munmap(outputData2,infoHeader.biSizeImage); //output data for fork process


    return 0;
}

void readHeaders(BMFH& filehead, BMIH& infohead, FILE *readFile)
{
     //fread(void *dest, int numBytes, int times, FILE *file)
     //read in data one field at a time to avoid padding issues 
     fread(&filehead.bfType, sizeof(WORD), 1, readFile);
     fread(&filehead.bfSize, sizeof(DWORD), 1, readFile);
     fread(&filehead.bfReserved1, sizeof(WORD), 1, readFile);
     fread(&filehead.bfReserved2, sizeof(WORD), 1, readFile);
     fread(&filehead.bfOffBits, sizeof(DWORD), 1, readFile);

     //no need to read in data one at a time because all fields take
     //up a complete memory cell. No padding to account for
     fread(&infohead, sizeof(infohead), 1, readFile);

};

void writeHeaders(BMFH& filehead, BMIH& infohead, FILE *writeFile)
{
     //fwrite(void *source, int numBytes, int times, FILE *file)
     fwrite(&filehead.bfType, sizeof(WORD), 1, writeFile);
     fwrite(&filehead.bfSize, sizeof(DWORD), 1, writeFile);
     fwrite(&filehead.bfReserved1, sizeof(WORD), 1, writeFile);
     fwrite(&filehead.bfReserved2, sizeof(WORD), 1, writeFile);
     fwrite(&filehead.bfOffBits, sizeof(DWORD), 1, writeFile);
     fwrite(&infohead, sizeof(infohead), 1, writeFile);
};

void changeColorGrading(BMIH& infohead, BYTE* input, BYTE* output, float rGrade, float gGrade, float bGrade, int rowStart, int rowStop)
{
    //check for padding on width of pixels
    int widthBytes = infohead.biWidth * 3; //pixels*(Byte/pixels)
    if(widthBytes %4 != 0)
    {
        widthBytes += (4-(widthBytes%4));
    }

    //loop through rows and columns to change RGB values
    int offset = 0;
    float r, g, b;
    
    for(int rows = rowStart; rows < rowStop; rows++)
    {
        for(int cols = 0; cols < infohead.biWidth; cols++)
        {
            //determine offset to access current pixel
            offset = (rows*widthBytes) + (cols*3);

            //normalize color and apply grading
            b = ((float)input[offset + BLUE])/255; //normalize
            b = (b*bGrade)*255; //apply grading and return to 0-255

            g = ((float)input[offset + GREEN])/255; //normalize
            g = (g*gGrade)*255; //apply grading and return to 0-255

            r = ((float)input[offset + RED])/255; //normalize
            r = (r*rGrade)*255; //apply grading and return to 0-255

            //store result into allocated memory
            output[offset + BLUE] = (BYTE)b;
            output[offset + GREEN] = (BYTE)g;
            output[offset + RED] = (BYTE)r;

        }
    }
}