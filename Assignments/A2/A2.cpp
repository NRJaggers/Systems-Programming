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
    BYTE *imageData;     //holds image data
}BMP;

void welcome();
void manual();
void readHeaders(BMFH& , BMIH&, FILE*);
void writeHeaders(BMFH& , BMIH&, FILE*);
BMP  readImage(char*);
void writeImage(BMP, char*);
BMP blendImages(BMP&, BMP&);
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

    // if either image is null, exit the program
    if((image1.imageData == NULL) || (image2.imageData == NULL))
    {
        printf(
                "At least one of the images was invalid.\n"
                "Exiting...\n"
              );
        return -1;
    }

//ALLOCATE SPACE FOR RESULTING IMAGE
    //create space for new image
    BMP image3;

//BLEND IMAGES
    //blend images and store results in new image
    //depending on which is bigger
    if(image1.infoHeader.biXPelsPerMeter>image2.infoHeader.biXPelsPerMeter)
        image3 = blendImages(image1, image2);
    else
        image3 = blendImages(image2, image1);

//WRITE NEW IMAGE TO NEW FILE
    //
    writeImage(image3,outputfile);

//CLEAN UP
    //release any allocated memory
    free(image1.imageData);
    free(image2.imageData);
    //free(image3.imageData);

//EXIT
    return 0;

}

void welcome()
{
    /**
     * prints welcome message for user
     */
    printf( 
            "\n------------------------------------------------------------\n"
            "Hello! Welcome to Assignment 2 where we blend two images\n"
            "specified by the user and display our results.\n"
            "------------------------------------------------------------\n\n"    
        );
}

void manual()
{
    /**
     * prints manual on how to use program for user
     */

    printf(
           "To use this program, you need to provide 5 arguments to the\n"
           "command line. You need to provide the name of this program to\n"
           "run it, followed by the two images you would like to blend,\n"
           "the blend ratio, and the name you would like to give the\n"
           "output file. The command should look the the following:\n\n"
           
           "./[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n"

           "[imagefile1] - first 24 bit per pixel BMP\n"
           "[imagefile2] - second 24 bit per pixel BMP\n"
           "     [ratio] - blend ratio ratio of images with respect to first image\n"
           "               ex. 1 is all first image, 0.5 is half and half, 0 is all of second image\n"
           "[outputfile] - desired name of resulting image file.\n\n"

           );
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

BMP  readImage(char* input_image_filename)
{
    /**
     * reads data into a BMP struct and returns results
     */

    //declare image object to return
    BMP image;

//READ DATA
    //open file using input.bmp command line arg as read binary
    FILE *file = fopen(input_image_filename, "rb");

    //check for proper open
    if (file == NULL)
    {
        printf("Could not open file, please try again\n");
        image.imageData = NULL;
        return image; 
    }

    //read in data
    readHeaders(image.fileHeader, image.infoHeader, file);

//ALLOCATE MEMORY TO STORE DATA
    //allocate memory to store read in data
    image.imageData = (BYTE*) malloc(image.infoHeader.biSizeImage);

    //read in image data
    fread(image.imageData, image.infoHeader.biSizeImage, 1, file);

    //close input file
    fclose(file);

    return image;
}

void writeImage(BMP image, char* output_image_filename)
{
    /**
     * writes data from a BMP struct into a file
     */

    //WRITE DATA
    //open file using output.bmp command line arg as write binary
     FILE *newFile = fopen(output_image_filename, "wb");

    //check for proper open
     if (newFile == NULL)
     {
         printf("Could not open file, please try again\n");
         return; 
     }

    //write info and data to newfile
    writeHeaders(image.fileHeader, image.infoHeader, newFile);
    fwrite(image.imageData, image.infoHeader.biSizeImage, 1, newFile);

    //close file
     fclose(newFile);
}

BMP blendImages(BMP& higher_resolution, BMP& lower_resolution)
{
    /**
     * Takes two input images and blends them together.
     * Results are stored and returned in a BMP object
     */

    //create temp to hold new image data
    BMP newImage;

    //set new image to same size as high resolution image
    newImage.fileHeader = higher_resolution.fileHeader;
    newImage.infoHeader = higher_resolution.infoHeader;
    newImage.imageData = (BYTE*) malloc(higher_resolution.infoHeader.biSizeImage);
    
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

    return newImage;
}

BYTE get_red(BYTE *imagedata,float x,float y, int imagewidth, int imageheight)
{
    BYTE test;
    return test;
}