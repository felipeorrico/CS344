/***********************************************************************
** File: keygen.c
** Author: Felipe Orrico Scognamiglio
** Class: CS344 F2020
** Instructor: Justin Goins
** Assignment: One-Time Pads (Assignment 5)
** Final Version Date: 11/27/2020
***********************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h> //for rand()


int main(int argc, char** argv) {
    srand(time(0));

		//list of all possible letters to the key. They are strings so that they can be cat
		//together to for the key within the loop.
		const char* ListLetters[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", " "};

    if (argc < 2 || argc > 2){
        fprintf(stderr,"%s: Not enough arguments\nUsage: ./keygen length\n", argv[0] + 2);
        exit(EXIT_FAILURE);
    }
    //as per assignment instructions, argv[1] is the size of the key
    char* key_size_arg = argv[1];
    int key_size = atoi(key_size_arg);
    //key size acquired! Now, generate random key!

    char key[key_size];
    memset(key, 0, sizeof(key));
    
    //loop for the size of the array and cat values to key array
    for (int i = 0; i < key_size; i++)
    {
        //for when the key string is still all null we copy the first value to it
        if (i == 0)
        {
					//copy a random str from the list into the key
          strcpy(key, ListLetters[rand() % 27]);
          continue;
        }
				//cat a rancom str from the list into the key
        strcat(key, ListLetters[rand() % 27]);
    }

    write(1, key, strlen(key));
    write(1, "\n", 1);

  return EXIT_SUCCESS;
}