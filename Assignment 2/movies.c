
#include "movies.h"
#include "linkedlist.h"
#include "methods.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]){

    if (argc > 1){
        printf("ERROR - Too Many arguments!\n");
        return EXIT_FAILURE;
    }
    runProgram();
    return EXIT_SUCCESS;

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

