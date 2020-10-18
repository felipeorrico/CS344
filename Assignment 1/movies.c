
#include "movies.h"
#include "linkedlist.h"
#include "methods.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]){

    if (argc > 2){
        printf("ERROR - Too Many arguments!\n");
        return EXIT_FAILURE;
    } else if (argc < 2){
        printf("ERROR - You must provide the name of the file to process!\n");
        return EXIT_FAILURE;
    }
    runProgram(argv[1]);
    return 0;

}

/*********************************************************************
** Function: movie_create
** Description: Allocates memory for a Movie struct "object" and sets
** desired values for each.
*********************************************************************/
struct Movie* movie_create(char* name, int year, struct LinkedList* langs, double rating){
    
    struct Movie* movie = (struct Movie*) malloc(sizeof(struct Movie));

    movie->name = name;
    movie->year = year;
    movie->langs = langs;
    movie->rating = rating;

    return movie;
}

