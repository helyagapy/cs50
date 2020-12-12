#include <cs50.h>
#include <stdio.h>
#include <math.h>

//Greedy algorithm for change return: For any positive number of amount owed, calculate the fewest number of coins required to produce amount owed
int main(void)
{
    float owed;
    int quart = 25;
    int dime = 10;
    int nickel = 5;
    int penny = 1;

    //Prompt user for numeric positive amount owed. Allow for float input.
    do
    {
        owed = get_float("Change owed: ");
    }
    while (owed < 0);

    //Round input and convert float input to integer number of cents.
    int cents = round(owed * 100);

    int coins;
    //While there remains an amount owed > 0, eject one coin's value from the amount owed starting with the largest coin.
    do
    {
        //For total cents remaining, count every loop when one coin is subtracted from the amount owed. Subtract coins in descending value, from largest to smallest denomination.
        for (coins = 0; cents > 0; coins++)
        {
            if (cents >= quart)
            {
                cents = cents - quart;
            }
            else if (cents < quart && cents >= dime)
            {
                cents = cents - dime;
            }
            else if (cents < dime && cents >= nickel)
            {
                cents = cents - nickel;
            }
            else
            {
                cents = cents - penny;
            }
        }
    }
    while (cents > 0);

    //Print total number of coins (or loops it took for cents owed to reach
    printf("%i\n", coins);
}