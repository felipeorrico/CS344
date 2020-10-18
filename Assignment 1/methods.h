#ifndef MethodsH
#define MethodsH

#include "linkedlist.h"
#include <stdio.h>

char* getFields(char* line, int field);
int checkStrings(char* first, char*second);
struct LinkedList *processFile(char *filePath);
int getLineCount(char* filename);
struct LinkedList* sortYear(struct LinkedList* list, int year);
char* replaceChar(char* str);
struct LinkedList* getFieldLang(char* line);
struct LinkedList* readFile (char *filePath);
void clear(struct LinkedList* list);
void clearLangs(struct LinkedList* list);
void print_movies_year(struct LinkedList* list, int year);
struct LinkedList* sortLang(struct LinkedList* list, char* lang);
void print_movies_lang(struct LinkedList* list, char* lang);
struct Node* containsYear(struct LinkedList* list, int year);
void print_movies_rating(struct LinkedList* list);
void runProgram(char* filename);
int getValidChoice();

#endif