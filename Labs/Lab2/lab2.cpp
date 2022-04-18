/*******************************************************
PROGRAM NAME - Lab 2 - Contrast Bitmap file

PROGRAMMER - Nathan Jaggers

DATE - 04/09/22

DESCRIPTION - This program takes in command line arguments
              for bmp images and a contrast value. It then
              contrasts the image and creates it as a new 
              bmp with a user specified name.
*******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

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
void contrastImage(BMIH&, BYTE*, float);

// argc holds the amount of arguments passed in from the command line
// argv is an array of char pointers holding all other args passed in
// arguments should be passed in as follows:

// ./lab2 [input.bmp] [output.bmp] [contrast]
// argv[0]   argv[1]     argv[2]     argv[3] 

// input.bmp is the bitmap image to contrast
// output.bmp is the name of the new bit map image
// contrast is the value to determine how much to contrast

int main(int argc, char* argv[])
{
    
// Reading the bmp info
     //open the file using input.bmp command line arg as read binary
     FILE *file = fopen(argv[1], "rb");

    //test that opening file was successful
     if (file == NULL)
     {
         printf("Could not open file, please try again\n");
         return -1; 
     }

     //create instance of BITMAPFILEHEADER and read in info from bmp
     BMFH fileHeader;
     //create instance of BITMAPINFOHEADER and read in info from bmp
     BMIH infoHeader;
     readHeaders(fileHeader, infoHeader, file);
     
//allocate memory for data and read image data
    //save return address to free memory later
     void *retAdd = sbrk(0);
    //allocate space for image data to be read in
     BYTE *data = (BYTE*) sbrk(infoHeader.biSizeImage);
    //read in image data
     fread(data, infoHeader.biSizeImage, 1, file);

    //contrast image pixels
     contrastImage(infoHeader, data, atof(argv[3]));

// Writing to the bmp
    //open the file using output.bmp command line arg as write binary
     FILE *newFile = fopen(argv[2], "wb");
     
    //test that opening file was successful
     if (newFile == NULL)
     {
         printf("Could not open file, please try again\n");
         return -1; 
     }

    //write info and data to newfile
    writeHeaders(fileHeader, infoHeader, newFile);
    fwrite(data, infoHeader.biSizeImage, 1, newFile);
    
    //close files
     fclose(newFile);
     fclose(file);

    //release memory
     brk(retAdd);

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

void contrastImage(BMIH& infohead, BYTE *imageData, float contrast)
{
    float normalizeColor;
    float color;
    for(unsigned int i = 0; i < infohead.biSizeImage; i++)
        {
            //adjust color value
            //store current color in float and normalize
            color = *(imageData+i);
            normalizeColor = (color/255);

            //contrast color: Keep brights bright, make darks darker
            normalizeColor = pow(normalizeColor, contrast);

            //return to 0-255 scale and cast as BYTE
            color = normalizeColor*255;
            *(imageData+i) = (BYTE)color;

        }
};