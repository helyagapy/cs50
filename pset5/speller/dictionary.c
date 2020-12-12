// Implements a dictionary's functionality

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include "dictionary.h"

//word counter for dictionary
int dic_word_count = 0;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table; selected from hash function below
const unsigned int N = 60000;

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false. Compare word from script to value in hash table. If the same, case-insensitive, then return true.
bool check(const char *word)
{
    int hash_ind_txt = hash(word);
    node *checker = table[hash_ind_txt];
    //traverse through nodes at hash index until it hits true or until the node hits NULL at which point it is known that the word is NOT in dict.
    while (checker != NULL)
    {
        if (strcasecmp(checker->word, word) == 0)
        {
            return true;
        }
        else
        {
            checker = checker->next;
        }
    }
    return false;
}

// Hashes word to a number. From djb2, Dan Bernstein, http://www.cse.yorku.ca/~oz/hash.html. Modified to save argument in lowercase copy and to return valid index number.
unsigned int hash(const char *word)
{
    int len = strlen(word) + 1;
    unsigned long hash = 5381;
    int c;
    char copy[len];
    for (int i = 0; i < len; i++)
    {
        copy[i] = tolower(word[i]);
    }
    while ((c = (*copy)++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % N;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *dic_file;
    char dic_word[LENGTH + 1];

    //open dictionary
    dic_file = fopen(dictionary, "r");

    if (dic_file == NULL)
    {
        return 1;
    }

    //from dic_file, scan for strings and store into word variable until end of file is hit
    while (fscanf(dic_file, "%s", dic_word) != EOF)
    {
        char copy[strlen(dic_word) + 1];
        strcpy(copy, dic_word);
        //new node to store dic_words and check for malloc errors
        // printf("this is dictionary word: %s\n", copy);
        node *new_node = malloc(sizeof(node));
        new_node->next = NULL;
        if (new_node == NULL)
        {
            return 1;
        }
        //copy lowered-case word from dictionary to head node
        strcpy(new_node->word, copy);

        //hash word to obtain hash value
        int hash_index = hash(copy);

        // if head node is empty, then new node with word takes the space
        if (table[hash_index] == NULL)
        {
            table[hash_index] = new_node;
            dic_word_count++;
        }
        //if head node already has value present, then store new word at the front of the list.
        else
        {
            new_node->next = table[hash_index];
            table[hash_index] = new_node;
            dic_word_count++;
        }
    }
    fclose(dic_file);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    if (dic_word_count > 0)
    {
        return dic_word_count;
    }
    else
    {
        return 0;
    }
}

// Unloads dictionary from memory, returning true if successful, else false. Traverse through entire hash table and free each node.
bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        node *cursor = table[i];
        //if array at hash index is actually present, then continuously free the head node and rove through the next node. If no array is pressent, then just free hash table at index.
        while (cursor != NULL)
        {
            node *unloader = cursor;
            cursor = cursor->next;
            free(unloader);
        }
        free(cursor);
    }
    return true;
}