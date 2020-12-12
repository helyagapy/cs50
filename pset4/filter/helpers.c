#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    int gray;
    // loop through image grid
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // calculate new gray value by averaging rgb value in each pixel and then reassign individual rgb value to the new gray.
            gray = round((image[i][j].rgbtRed + image[i][j].rgbtGreen + image[i][j].rgbtBlue) / 3.0);
            image[i][j].rgbtRed = gray;
            image[i][j].rgbtGreen = gray;
            image[i][j].rgbtBlue = gray;
        }
    }
    return;
}

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    //initiate sepia color containers
    int sepiaRed;
    int sepiaGreen;
    int sepiaBlue;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            //formula for new sepia values
            sepiaRed = round(.393 * image[i][j].rgbtRed + .769 * image[i][j].rgbtGreen + .189 * image[i][j].rgbtBlue);
            sepiaGreen = round(.349 * image[i][j].rgbtRed + .686 * image[i][j].rgbtGreen + .168 * image[i][j].rgbtBlue);
            sepiaBlue = round(.272 * image[i][j].rgbtRed + .534 * image[i][j].rgbtGreen + .131 * image[i][j].rgbtBlue);

            // if ALL sepia values are less than 255, then proceed to replace image values with sepia values
            if (sepiaRed <= 255 && sepiaGreen <= 255 && sepiaBlue <= 255)
            {
                image[i][j].rgbtRed = sepiaRed;
                image[i][j].rgbtGreen = sepiaGreen;
                image[i][j].rgbtBlue = sepiaBlue;
            }
            //if any sepia color is over 255, cap it at 255 and then replace image values with sepia values
            if (sepiaRed > 255)
            {
                sepiaRed = 255;
            }
            if (sepiaGreen > 255)
            {
                sepiaGreen = 255;
            }
            if (sepiaBlue > 255)
            {
                sepiaBlue = 255;
            }
            image[i][j].rgbtRed = sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // iterate through image grid and using container RGBTRIPLE tempArray to copy over original image values and process pairwise swap/reflection aka "third glass"
    RGBTRIPLE tempArray[height][width];
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < (width / 2); j++)
        {
            tempArray[i][j] = image[i][j];
            image[i][j] = image[i][(width - j - 1)];
            image[i][width - j - 1] = tempArray[i][j];
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    //declare temporary array to store averages without overwriting original
    RGBTRIPLE tempArray[height][width];
    // First loop to average surrounding rgb values and then to store new averaged values in temporary array
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int sumR = 0;
            int sumB = 0;
            int sumG = 0;
            float pixels = 0.0;

            //with [i][j] as origin aka (0,0) of 3x3 grid, set new grid with y for row iteration and x for column interation
            for (int y = -1; y < 2; y++)
            {
                for (int x = -1; x < 2; x++)
                {
                    int newY;
                    int newX;
                    
                    // calculate new coordinate indexes of surrounding pixels to calculate rbg values.
                    newY = y + i;
                    newX = x + j;

                    // check if observed pixel surrounding [i][j] is within confines of image height and width indexes. If any violations on edges, break from the single iteration on the pixel in question and continue on.
                    if (newY < 0 || newY > (height - 1) || newX< 0 || newX > (width - 1))
                    {
                        continue;
                    }
                   
                    // calculate sums
                    sumR = sumR + image[newY][newX].rgbtRed;
                    sumB = sumB + image[newY][newX].rgbtBlue;
                    sumG = sumG + image[newY][newX].rgbtGreen;
                    pixels++;
                }
            }
            // calculate averages and save in temporary array
            tempArray[i][j].rgbtRed = round(sumR / pixels);
            tempArray[i][j].rgbtBlue = round(sumB / pixels);
            tempArray[i][j].rgbtGreen = round(sumG / pixels);
        }
    }

    // Second loop to replace the image values with new values from the temporary array
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = tempArray[i][j];
        }
    }
    return;
}
