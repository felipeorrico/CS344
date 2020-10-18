#include "movies.h"
#include "linkedlist.h"
#include "methods.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//this is just a temporary variable for the getFields function.
char out[256];

/*********************************************************************
** Function: getFields
** Description: Gets a specific field from a line string separated by commas.
*********************************************************************/
char* getFields(char* line, int field){
    char* cpy = (char*) calloc(strlen(line) + 1, sizeof(char));
    strcpy(cpy, line);
    char* token = strtok(cpy,",");
    int i = 0;

    while (token != 0) { 
        if (i == field)
            break;
        i++;
        token = strtok(0, ","); 
    } 
    if (field != 0){ //if the field in question is not the name of the movie
        strcpy(out, token);
        free(cpy);
        return out;
    }
    return token;
}

/*********************************************************************
** Function: processFile
** Description: Opens desired file and allocates each movie to a movie
** struct and pushes each of them into a linked list.
*********************************************************************/
struct LinkedList *processFile(char *filePath)
{
    FILE *File = fopen(filePath, "r");
    if (!File){
        printf("\nERROR - File not Found!\n\n");
        return NULL;
    }

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;
    char* ptr;
    struct LinkedList* list = linkedlist_create();

    int i = 0;

    //read each line and allocate movie for that line
    while ((nread = getline(&currLine, &len, File)) != -1)
    {
        if (i == 0){
            i++;
            continue;
        }
        if (!currLine){
            continue;
        }

        struct Movie* movie = (struct Movie*) movie_create(getFields(currLine, 0),atoi(getFields(currLine, 1)), getFieldLang(getFields(currLine,2)), strtod(getFields(currLine, 3), &ptr));
        linkedlist_push_back(list, movie);
    }
    free(currLine);
    fclose(File);
    return list;
}

/*********************************************************************
** Function: getLineCount
** Description: returns the number of lines a file has. (not used in 
** this program)
*********************************************************************/
int getLineCount(char* filename){
    FILE *file;
    file = fopen(filename,"r");

    int i = 0;
    char line[2048];
    while (fgets(line,2048,file)){

        i++;
    }
    fclose(file);
    return i;
}

/*********************************************************************
** Function: clear
** Description: Frees the memory allocated for movies, and the linked
** list.
*********************************************************************/
void clear(struct LinkedList* list){
    struct Node* next = list->head;
	struct Node* cur = NULL;
	while (next) {
		cur = next;
		next = next->next;
        struct LinkedList* langs = ((struct Movie*)(cur->val))->langs;
        clearLangs(langs);
        char* name = ((struct Movie*)(cur->val))->name;
		free(name);
        free(cur->val);
        free(cur);
	}
	list->length = 0;
    free(list);
}

/*********************************************************************
** Function: clearLangs
** Description: frees memory alocated for languages within a movie
** struct.
*********************************************************************/
void clearLangs(struct LinkedList* list){
    struct Node* next = list->head;
	struct Node* cur = NULL;
	while (next) {
		cur = next;
		next = next->next;
        char* name = (char*)cur->val;
		free(name);
        free(cur);
	}
	list->length = 0;
    free(list);
}

/*********************************************************************
** Function: sortYear
** Description: creates and returns a linked list of all movies from 
** that year.
*********************************************************************/
struct LinkedList* sortYear(struct LinkedList* list, int year){
    
    struct LinkedList* sorted = linkedlist_create();

    struct Node* current = list->head;
    while (current){
        struct Movie* movie = (struct Movie*) current->val;
        if (movie->year == year){
            linkedlist_push_back(sorted, movie);
        }
        current = current->next;
    }

    //printf("\n\nSize of Movie List: %i", sorted->length);
    return sorted;
}

/*********************************************************************
** Function: sortYear
** Description: creates and returns a linked list of all languages from 
** that a movie.
*********************************************************************/
struct LinkedList* getFieldLang(char* cpy){
    cpy[strlen(cpy)-1] = 0;
    cpy++;
    char* token = strtok(cpy,";");
    //printf("Memory of Lang: %x\n", cpy);
    
    struct LinkedList* listLang = linkedlist_create();
    
    while (token) { 
        char* lang = (char*) calloc(strlen(token) + 1, sizeof(char));
        strcpy(lang, token);
        linkedlist_push_back(listLang, lang);
        token = strtok(0, ";"); 
    }

    return listLang;
}

/*********************************************************************
** Function: checkStrings
** Description: check if two strings are equal(1) or not(0)
*********************************************************************/
int checkStrings(char* first, char*second){
    int returned = strcmp(first, second);
    if (returned == 0)
        return 1;
    else
        return 0;
}

/*********************************************************************
** Function: print_movies_year
** Description: Prints the movies that were released on the desired
** year.
*********************************************************************/
void print_movies_year(struct LinkedList* list, int year){
    
    struct LinkedList* years = sortYear(list, year);

    //printf("Now printing Movies released in year: %i\n");
    if (years->length == 0){
        linkedlist_delete(years);
        printf("No data about movies released in the year %i\n\n", year);
        return;
    }

    struct Node* head = years->head;
	while (head) {
        struct Movie* movie = (struct Movie*)head->val;
		printf("%s\n", movie->name);
		head = head->next;
	}

    printf("\n\n");
    linkedlist_delete(years);
}

/*********************************************************************
** Function: sortLang
** Description: Returns a LinkedList of movies that were released
** in the chosen language.
*********************************************************************/
struct LinkedList* sortLang(struct LinkedList* list, char* lang){
    
    struct LinkedList* sorted = linkedlist_create();

    struct Node* current = list->head;
    while (current){
        struct Movie* movie = (struct Movie*) current->val;
        struct Node* langs = movie->langs->head;
        while(langs){
            if (checkStrings(lang, (char*) langs->val) == 1){
                linkedlist_push_back(sorted, movie);
                break;
            }
            langs = langs->next;
        }
        current = current->next;
    }

    return sorted;
}

/*********************************************************************
** Function: print_movies_lang
** Description: Calls sort and prints all movies that were released
** in the chosen language.
*********************************************************************/
void print_movies_lang(struct LinkedList* list, char* lang){
    
    struct LinkedList* langs = sortLang(list, lang);

    if (langs->length == 0){
        linkedlist_delete(langs);
        printf("No data about movies released in language %s\n\n", lang);
        return;
    }

    struct Node* head = langs->head;
	while (head) {
        struct Movie* movie = (struct Movie*)head->val;
		printf("%i %s\n",movie->year ,movie->name);
		head = head->next;
	}
    
    printf("\n\n");
    linkedlist_delete(langs);
}

/*********************************************************************
** Function: print_movies_rating
** Description: Sorts and prints the highest rated movie for each year
*********************************************************************/
void print_movies_rating(struct LinkedList* list){

    struct LinkedList* rating = linkedlist_create();
    struct Node* list_node = list->head,* movie_node = NULL;

    //adding movies in linked list, and replacing if rating for year is greater.
    while (list_node){
        movie_node = containsYear(rating, ((struct Movie*)(list_node->val))->year);
        if (movie_node == NULL){
            linkedlist_push_back(rating, list_node->val);
        } else {
            if (((struct Movie*)(list_node->val))->rating > ((struct Movie*)movie_node->val)->rating){
                movie_node->val = list_node->val;
            }
        }
        list_node = list_node->next;
    }
    if (rating->length == 0){
        printf("\nERROR - List of Movies is Empty!");
        return;
    }

    //printing the list
    struct Node* printer = rating->head;

    while(printer){
        struct Movie* movie = (struct Movie*) printer->val;
        printf("%i %.1f %s\n", movie->year, movie->rating, movie->name);
        printer = printer->next;
    }

    list_node = NULL;
    printer = NULL;
    movie_node = NULL;

    printf("\n\n");
    linkedlist_delete(rating);
}

/*********************************************************************
** Function: containsYear
** Description: Checks if a LinkedList contains a movie released in
** the chosen year and returns it. if no movie found, returns NULL.
*********************************************************************/
struct Node* containsYear(struct LinkedList* list, int year){

    struct Node* head = list->head;
    while(head){
        if (((struct Movie*)(head->val))->year == year)
            return head;
        head = head->next;
    }

    return NULL;
}

/*********************************************************************
** Function: getValidchoice
** Description: Gets a valid input choice for the menu.
*********************************************************************/
int getValidChoice(){
    int choice = 0;
    char choice_char[1];
    int trigger = 0;
    while (choice < 1 || choice > 4){
        if (trigger == 1){
            choice = 0;
            trigger = 0;
            printf("\nERROR - This is not a valid choice! Please choose again.\n");
        }
        printf("\n\nEnter a choice from 1 to 4: ");
        scanf("%s", choice_char);
        printf("\n");
        choice = atoi(choice_char);
        trigger = 1;
    }
    
    return choice;
}

/*********************************************************************
** Function: runProgram
** Description: Runs the program calling other functions based on the
** given choice.
*********************************************************************/
void runProgram(char* filename){
    struct LinkedList* movies = processFile(filename);
    if (!movies)
        return;

    printf("\nProcessed file movie_sample_1.csv and parsed data for %i movies.\n\n", movies->length);

    int choice = 0;

    while (choice != 4) {
        printf("1. Show movies released in the specified year\n2. Show highest rated movie for each year\n3. Show the title and year of release of all movies in a specific language\n4. Exit from the program");
        choice = getValidChoice();
        if (choice == 1){ //movies for a year
            char years[1];
            printf("\nEnter the year for which you want to see movies: ");
            scanf("%s", years);
            int year = atoi(years);
            printf("\n");
            print_movies_year(movies, year);
        } else if (choice == 2){ //movies by rating
            print_movies_rating(movies);
        } else if (choice == 3){ //movies by language
            char language[64];
            printf("\nEnter the language for which you want to see movies: ");
            scanf("%s", language);
            printf("\n");
            print_movies_lang(movies, language);
        }
    }

    clear(movies);
}