#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//prompt user for key and text and present cipher text to user while keeping all white-spaces, punctuation, and numeric characters as-is

int main(int argc, string argv[])
{
    if (argc == 2) //only 2 inputs allowed: 1st is the command execution, 2nd is the key as string argv[]
    {
        //convert the string argv input to integer
        int k = atoi(argv[1]);

        //continue with cipher only if positive integer is available as key
        if (k > 0)
        {
            //prompt user for plain text
            string plain_text = get_string("plaintext: ");

            //store length of user's input
            int length = strlen(plain_text);

            printf("ciphertext: ");
            int i;

            //iterate through user input
            for (i = 0; i < length; i++)
            {
                //print character as-is if character is a punctuation, numeral, or white-space
                if (ispunct(plain_text[i]) || isdigit(plain_text[i]) || isspace(plain_text[i]))
                {
                    printf("%c", plain_text[i]);
                }
                //convert upper case letter ascii to alphabet index, and find new cipher character with provided formula to loop around alphabet in the same case.
                else if (isupper(plain_text[i]))
                {
                    int pi = plain_text[i] - 65;
                    int ci = (pi + k) % 26;
                    char c = ci + 65;
                    printf("%c", c); //print cipher character
                }
                //convert lower case letter ascii to alphabet index, and find new cipher character with provided formula to loop around alphabet in the same case.
                else if (islower(plain_text[i]))
                {
                    int pi = plain_text[i] - 97;
                    int ci = (pi + k) % 26;
                    char c = ci + 97;
                    printf("%c", c); //print cipher character
                }
            }
            printf("\n"); //print new line after all full iteration of user input
        }
        else //print error message to reprompt execution if non-positive integer is provided
        {
            printf("Usage: ./caesar\n");
        }
        return 0;

    }
    else //print error message to reprompt execution if less than or more than 2 command-line inputs are entered
    {
        printf("Usage: ./caesar\n");
    }
    return 1;
}