
#ifndef MoviesH
#define MoviesH

#include "linkedlist.h"
#include <stdio.h>


struct Movie {
    char* name;
    int year;
    struct LinkedList* langs;
    double rating;
};

struct Movie* movie_create(char* name, int year, struct LinkedList* langs, double rating);


#endif