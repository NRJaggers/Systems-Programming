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

//function prototypes
void welcome();
void manual();
void readHeaders(BMFH& , BMIH&, FILE*);
void writeHeaders(BMFH& , BMIH&, FILE*);
BMP  readImage(char*);
void writeImage(BMP, char*);
BMP blendImages(BMP&, BMP&, float);
float interpolate_color(BMP, float, float, int);
float get_color(BMP, int, int, int);

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
        image3 = blendImages(image1, image2, ratio);

//WRITE NEW IMAGE TO NEW FILE
    //
    writeImage(image3,outputfile);

//CLEAN UP
    //release any allocated memory
    free(image1.imageData);
    free(image2.imageData);
    free(image3.imageData);

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

BMP blendImages(BMP& firstImage, BMP& secondImage, float blendRatio)
{
    /**
     * Takes two input images and blends them together.
     * Results are stored and returned in a BMP object
     */

    //determine which image has higher resolution
    //take inverse of blend ratio if needed
    BMP higher_resolution;
    BMP lower_resolution;

    if(firstImage.infoHeader.biXPelsPerMeter>=secondImage.infoHeader.biXPelsPerMeter)
    {
        higher_resolution = firstImage;
        lower_resolution  = secondImage;
    }
    else
    {
        higher_resolution = secondImage;
        lower_resolution  = firstImage;
        blendRatio = 1-blendRatio;
    }

    //create temp to hold new image data
    BMP newImage;

    //set new image to same size as high resolution image
    newImage.fileHeader = higher_resolution.fileHeader;
    newImage.infoHeader = higher_resolution.infoHeader;
    newImage.imageData = (BYTE*) malloc(higher_resolution.infoHeader.biSizeImage);
    
    //get more specific how you are gonna do this
    //can reference pdf and notes
    //also can use old lab work for padding
    
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

     //check for padding on width of pixels for high resolution image
    int widthBytesBig = higher_resolution.infoHeader.biWidth * 3; //pixels*(Byte/pixels)
    if(widthBytesBig %4 != 0)
    {
        widthBytesBig += (4-(widthBytesBig%4));
    }

    //determine equivalent increment for low res image
    float xInc = (float) lower_resolution.infoHeader.biWidth/higher_resolution.infoHeader.biWidth;
    float yInc = (float) lower_resolution.infoHeader.biHeight/higher_resolution.infoHeader.biHeight;
    float interpretPixel_x;
    float interpretPixel_y;

    //loop through rows and columns to change RGB values
    int offset = 0;
    float bBig, gBig, rBig, bSmall, gSmall, rSmall;
    
    for(int rows = 0; rows < higher_resolution.infoHeader.biHeight; rows++)
    {
        for(int cols = 0; cols < higher_resolution.infoHeader.biWidth; cols++)
        {
            //determine offset to access current pixel for high res image
            offset = (rows*widthBytesBig) + (cols*3);

            //get colors for high res image
            bBig = higher_resolution.imageData[offset+BLUE]; 
            gBig = higher_resolution.imageData[offset+GREEN]; 
            rBig = higher_resolution.imageData[offset+RED];

            //get coordinates between pixels for equivalent spot in low res image
            interpretPixel_x =  cols * xInc;
            interpretPixel_y = rows * yInc;
            
            //get colors for low res image
            bSmall = interpolate_color(lower_resolution, interpretPixel_x, interpretPixel_y, BLUE);
            gSmall = interpolate_color(lower_resolution, interpretPixel_x, interpretPixel_y, GREEN);
            rSmall = interpolate_color(lower_resolution, interpretPixel_x, interpretPixel_y, RED);

            //image of same size
            // bSmall = lower_resolution.imageData[offsetLow+BLUE]; 
            // gSmall = lower_resolution.imageData[offsetLow+GREEN]; 
            // rSmall = lower_resolution.imageData[offsetLow+RED];

            //store result into allocated memory
            newImage.imageData[offset + BLUE]  = (BYTE)(bBig)*(blendRatio) + (bSmall)*(1-blendRatio);
            newImage.imageData[offset + GREEN] = (BYTE)(gBig)*(blendRatio) + (gSmall)*(1-blendRatio);
            newImage.imageData[offset + RED]   = (BYTE)(rBig)*(blendRatio) + (rSmall)*(1-blendRatio);

        }
    }

    return newImage;
}

float interpolate_color(BMP image, float x, float y, int color)
{
    //define place for result and define coordinates
    float result;
    int x1 = x, x2 = x+1, y1 = y, y2 = y+1;
    float dx = x-x1, dy = y-y1;


    //get colors from surrounding pixels
    float leftUp = get_color(image, x, y, color);
    float leftDown = get_color(image, x, y, color);
    float rightUp = get_color(image, x, y, color);
    float rightDown = get_color(image, x, y, color);

    //calculate interpoated color
    //the closer it is the stronger the weight should be
    float left = leftUp * dy + leftDown * (1 - dy);
    float right = rightUp * dy + rightDown * (1 - dy);
    result = left * (1 - dx) + right * dx;

    //return result
    return result;
}

float get_color(BMP image, int x, int y, int color)
{
    //define variable to return value of color at desired location
    float colorValue;

    //check for padding on width of pixels for low resolution image
    int widthBytes = image.infoHeader.biWidth * 3; //pixels*(Byte/pixels)
    if(widthBytes %4 != 0)
    {
        widthBytes += (4-(widthBytes%4));
    }

    int offset = y*widthBytes + x*3 +color;

    //get color at desired location
    colorValue = image.imageData[offset];

    //return result
    return colorValue;
}