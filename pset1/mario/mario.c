#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int h; //height
    int i; //row
    int j; //column

    //prompt user for height until integer value of [1,8] is provided
    do
    {
        h = get_int("Height: ");
    }
    while (h < 1 || h > 8);

    //Iterate until there are h number of rows
    for (i = 1; i <= h ; i++)
    {
        //Iterate until there are h number of columns within a row
        for (j = 1; j <= h; j++)
        {
            if (i + j <= h) //Print spaces if coordinate is less than height
            {
                printf(" ");
            }
            else //Print hashes if coordinate points is > height
            {
                printf("#");
            }
        }
        printf("\n");
    }
}
