#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// NOTE: HELP WAS ACQUIRED TO COMPLETE PROBLEM. CONSULTED WITH MIT PHD STUDENT, WHOM IS NOT STUDENT IN CLASS, FOR LINES 58-61.

// Open and read forensic image for jpg files and copy any jpg data to new .jpg files.
#define BLOCK_SIZE 512
typedef uint8_t BYTE;
BYTE buffer[BLOCK_SIZE];

int main(int argc, char *argv[])
{
    // print error message if incorrect number of command line arguments are entered
    if (argc != 2)
    {
        printf("Usage: ./recover image");
        return 1;
    }

    // open source file and print error if it can't be opened
    FILE *source = fopen(argv[1], "r");
    if (source == NULL)
    {
        printf("nothing present");
        return 1;
    }

    // declare counter for image file name
    int imgNum = 0;
    FILE *img;

    // read through memory card
    while (fread(buffer, BLOCK_SIZE, 1, source))
    {
        // check first 4 bytes for jpg values and if present, continue through loop and create ascending image files
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && ((buffer[3] & 0xf0) == 0xe0))
        {
            // close prior opened img file, if any
            if (imgNum > 0)
            {
                fclose(img);
            }
            // allocate space for file name and include null char
            char filename[8];

            // create file name and increase imgNum counter for subsequent iterations
            sprintf(filename, "%03i.jpg", imgNum);

            // open and copy contents from buffer to img file
            img = fopen(filename, "w");

            // increase img file counter by one
            imgNum++;
        }

        // Check for if image is not present within first byte in which case imgNum remains 0. As such, skip and move on until jpg is detected and imgNum is > 0
        if (imgNum == 0)
        {
            continue;
        }

        // write/copy contents from buffer into new img file
        fwrite(buffer, BLOCK_SIZE, 1, img);
    }
    //close all files
    fclose(source);
    fclose(img);

    return 0;
}