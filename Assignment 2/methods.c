#include "movies.h"
#include "linkedlist.h"
#include "methods.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define PREFIX "movies_"
#define SUFFIX ".csv"

//these are just buffer variables.
char out[256];
char entryName[256];

/*********************************************************************
** Function: getFields
** Description: Gets a specific field from a line string separated by commas.
*********************************************************************/
char* getFields(char* line, int field){
    char* cpy = (char*) calloc(strlen(line) + 1, sizeof(char));
    strcpy(cpy, line);
    char* token = strtok(cpy,",");
    int i = 0;

    while (token != 0) { //gets the desired collumn (fiels) for the given line
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
        printf("\nERROR - Could process specified file. Please try again.\n\n");
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
** Description: creates and returns a linked list of all languages from 
** that a movie.
*********************************************************************/
struct LinkedList* getFieldLang(char* cpy){
    //remove brackets from string
    cpy[strlen(cpy)-1] = 0;
    cpy++;

    //extract first token from string
    char* token = strtok(cpy,";");
    
    struct LinkedList* listLang = linkedlist_create();
    
    //put token into linked list and extract next token.
    while (token) { 
        char* lang = (char*) calloc(strlen(token) + 1, sizeof(char));
        strcpy(lang, token);
        linkedlist_push_back(listLang, lang);
        token = strtok(0, ";"); 
    }

    return listLang;
}

/*********************************************************************
** Function: getValidchoice
** Description: Gets a valid input choice for the menu.
*********************************************************************/
int getValidChoice(int max, int min){
    int choice = 0;
    char choice_char[1];
    int trigger = 0;
    while (choice < min || choice > max){
        if (trigger == 1){ //if the choice was not valid, loop again and print error message
            choice = 0;
            trigger = 0;
            printf("\nERROR - This is not a valid choice! Please choose again.\n");
        }
        printf("\n\nEnter a choice from %i to %i: ", min,max);
        scanf("%s", choice_char);
        printf("\n");
        choice = atoi(choice_char);
        trigger = 1;
    }
    
    return choice;
}

/*********************************************************************
** Function: getSizeableFile
** Description: gets the name of the largest or smallest file in the 
** current directory. The base for this function was found in
** Exploration Directories in Modules.
*********************************************************************/
char* getSizeableFile(int variant){ //variant == 1 largest, variant == 0 smallest.
    DIR* currDir = opendir(".");
    struct dirent *aDir;
    off_t last;
    struct stat dirStat;
    int i = 0;

    // Go through all the entries
    while((aDir = readdir(currDir)) != NULL){

        int k, p;
        if (strlen(aDir->d_name) < 4){
            continue;
        }

        //check if the file has the necessary prefix and if the last 4 characters are .csv
        if(strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0 && strncmp(SUFFIX, aDir->d_name + (strlen(aDir->d_name) - 4), strlen(SUFFIX)) == 0){
            
            stat(aDir->d_name, &dirStat);
            
            //Get entry
            if (variant){
                if(i == 0 || dirStat.st_size > last){
                    last = dirStat.st_size;
                    memset(entryName, '\0', sizeof(entryName));
                    strcpy(entryName, aDir->d_name);
                }
            } else {
                if(i == 0 || dirStat.st_size < last){
                    last = dirStat.st_size;
                    memset(entryName, '\0', sizeof(entryName));
                    strcpy(entryName, aDir->d_name);
                }
            }
            i++;
        }
    }
    // Close the directory
    closedir(currDir);
    return entryName;
}

/*********************************************************************
** Function: createDir
** Description: created a directory with a random number termination
** and returns the int of that random termination.
*********************************************************************/
int createDir(){
    srand(time(NULL)); 
    int randomNumber = rand() % 100000;
    
    char pathName[256];
    memset(pathName, '\0', sizeof(pathName));
    
    sprintf(pathName,"./orricosf.movies.%i", randomNumber);
    //create directory with permissions 0750
    mkdir(pathName, 0750); 
    return randomNumber;
}

/*********************************************************************
** Function: createFiles
** Description: Creates the output files for each year and populates
** those files with the movies for that year.
*********************************************************************/
void createFiles(struct LinkedList* movies, int dirPath){
    struct LinkedList* years = getYears(movies);
    struct Node* current_year = years->head;

    while (current_year){
        char stdPath[256];
        int currYear =  ((struct Movie*)current_year->val)->year;
        memset(stdPath, '\0', sizeof(stdPath));
        //set file path
        sprintf(stdPath,"orricosf.movies.%i/%i.txt", dirPath, currYear);
        //create the file
        FILE *file = fopen(stdPath, "w");

        struct Node* innerMovie = movies->head;
        //put movies into file
        while (innerMovie){
            struct Movie* currentMovie = (struct Movie*) innerMovie->val;

            if (currentMovie->year == currYear){
                fputs(currentMovie->name, file);
                fputs("\n", file);
            }

            innerMovie = innerMovie->next;
        }
        fclose(file);
        //change file permissions
        chmod(stdPath, 0640);
        current_year = current_year->next;
    }
    linkedlist_delete(years);
}

/*********************************************************************
** Function: containsYearList
** Description: Checks if a year is already in the year list.
*********************************************************************/
int containsYearList(struct LinkedList* years, int year){
    struct Node* current = years->head;
    while(current){
        
        if(((struct Movie*)current->val)->year == year)
            return 1;

        current = current->next;
    }
    return 0;
}

/*********************************************************************
** Function: getYears
** Description: Gets a list of the years that the movies were released
*********************************************************************/
struct LinkedList* getYears(struct LinkedList* movies){
    struct LinkedList* years = linkedlist_create();
    
    struct Node* current = movies->head;

    while(current){
        //if the years list does not contain the year, make a shallow copy of the movie to the years list
        if (!containsYearList(years, ((struct Movie*) current->val)->year)){
            linkedlist_push_back(years, current->val);
        }
        current = current->next;
    }

    return years;
}

/*********************************************************************
** Function: getFileName
** Description: Gets the file name to process. this will accept any
** input string. the error checking only happens in processFile.
*********************************************************************/
char* getFileName(){
    char buffer[256];
    memset(buffer, '\0', sizeof(buffer));
    printf("Enter the complete file name: ");
    scanf("%s", buffer);
    char* returned = (char*) malloc((strlen(buffer)+1) * sizeof(char));
    strcpy(returned, buffer);
    return returned;
}

/*********************************************************************
** Function: runProgram
** Description: Runs the program calling other functions based on the
** given choice.
*********************************************************************/
void runProgram(){

    int choice = 0;
    struct LinkedList* movies = NULL;

    while (choice != 2) {
        printf("1. Select file to process\n2. Exit the program");
        choice = getValidChoice(2,1);
        do{
            if (choice == 1){ //actually run the program
                printf("Which file you want to process?\nEnter 1 to pick the largest file\nEnter 2 to pick the smallest file\nEnter 3 to specify the name of a file");
                choice = getValidChoice(3,1);
                if (choice == 1){//largest
                    movies = processFile(getSizeableFile(1));
                } else if (choice == 2) {//smallest
                    movies = processFile(getSizeableFile(0));
                } else if (choice == 3) {//name the file
                    char* filename = getFileName();
                    movies =  processFile(filename);
                    strcpy(entryName, filename);
                    free(filename);
                }
                if (!movies){ //if the movie pointer is still NULL
                    choice = 1;
                    continue;
                }
                printf("\nNow processing file %s and parsing the data of %i movies.\n", entryName,movies->length);
                int dirPath = createDir();
                createFiles(movies, dirPath);
                printf("\nCreated directory with name orricosf.movies.%i\n\n", dirPath);
                clear(movies);
                choice = 0;
            }
        } while(choice == 1);
    }

}