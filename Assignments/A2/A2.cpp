/*******************************************************
PROGRAM NAME - Assignment 2 - Image Blending

PROGRAMMER - Nathan Jaggers

DATE - 05/10/22

DESCRIPTION - This program takes two bmp files from the 
              user and blends them together. The resulting
              image is stored in a newfile with the name
              also specified by the user. If the user does
              not enter the correct amount of arguments, a
              manual will be printed to show the user correct
              operation of the program. This program takes
              standard BMPs (24 bits per pixel).
*******************************************************/

#include <iostream>
#include <stdlib.h>
#include <unistd.h>

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

typedef struct BITMAPFILE
{
    BMFH fileHeader;    //holds image file header data
    BMIH infoHeader;    //holds image info header data
    BYTE imageData;     //holds image data
}BMP;

void welcome();
void manual();
void readHeaders(BMFH& , BMIH&, FILE*);
void writeHeaders(BMFH& , BMIH&, FILE*);
BMP  readImage(char*);
void writeImage(BMP, char*);
void blendImages(BMP&, BMP&, BMP&);
BYTE get_red(BYTE *imagedata,float x,float y, int imagewidth, int imageheight);

int main(int argc, char** argv)
{
//WELCOME
    welcome();

//CHECK FOR PROPER USE OF PROGRAM
    //if not, print out manual and exit
    if (argc != 5)
    {
        manual();
        return -1;
    }
    char *inputfile1 = argv[1];
    char *inputfile2 = argv[2];
    float ratio      = atof(argv[3]);
    char *outputfile = argv[4];

//READ IN IMAGES
    //read in first user defined image
    BMP image1 = readImage(inputfile1);

    //read in second user defined image
    BMP image2 = readImage(inputfile2);

//ALLOCATE SPACE FOR RESULTING IMAGE
    //create and allocate memory for new image
    BMP newImage;
    //newImage.imageData = 

//BLEND IMAGES
    //blend images and store results in new image
    //depending on which is bigger
    blendImages(image1, image2, newImage);

//WRITE NEW IMAGE TO NEW FILE
    //
    writeImage(newImage,outputfile);

//CLEAN UP
    //close files?
    //release any allocated memory

//EXIT
    return 0;

}

void welcome()
{
    /**
     * prints welcome message for user
     */
}

void manual()
{
    /**
     * prints manual on how to use program for user
     */

    /******SOMETHING LIKE THIS BUT IT NEED WORK*******/

    // ./yourprogram [IMAGEFILE] [COLOR GRADING] [OUTPUTFILE]

    // [IMAGEFILE] that’s your bitmap
    // [COLOR GRADING] three float number between 0 and 1 representing red, green and blue (RGB)
    // [OUTPUTFILE] the output file

    // ./lab3   [input.bmp] [R] [G] [B] [output.bmp] 
    // argv[0]  argv[1]     argv[2:4]   argv[5] 
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

BMP  readImage(char*)
{
    BMP test;
    return test;
}

void writeImage(BMP, char*)
{

}

void blendImages(BMP&, BMP&, BMP&)
{
    // Loop over the bigger one:
    // Loop in x
    // Loop in y
    // //Get the coords from the smaller image:
    // x_2 = … x …;
    // y_2 = … y …;
    // get the color from image 2:
    // get_red(imagedata2,x_2,y_2,…);
    // //and green, blue
    // Blend the colors
    // Assign them into the resultimage
}

BYTE get_red(BYTE *imagedata,float x,float y, int imagewidth, int imageheight)
{
    BYTE test;
    return test;
}