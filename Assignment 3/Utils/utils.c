#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

int SIZE_ARGS;

/*********************************************************************
** Function: util_getArgSize
** Description: returns the amount of arguments of the input.
*********************************************************************/
int util_getArgSize(){
    return SIZE_ARGS;
}

/*********************************************************************
** Function: util_replaceSubstring
** Description: replaces substring for desired value (repl)
*********************************************************************/
char* util_replaceSubstring(char* str, char* toBeReplaced, int repl){
    char new_str[2050];
    memset(new_str, 0, sizeof(new_str));
    strcpy(new_str, str);
    for (int i = 0; i < (strlen(new_str) - 1); i++){ //search and replace algorithm
        if (new_str[i] == toBeReplaced[0] && new_str[i+1] == toBeReplaced[0]){
            new_str[i] = '%';
            new_str[i+1] = 'i';
            char str_temp[2050];
            sprintf(str_temp,new_str, repl);
            strcpy(new_str, str_temp);
        }
    }
    char* returned = malloc(sizeof(char) * (strlen(new_str) + 1));
    strcpy(returned, new_str);
    return returned;
}

/*********************************************************************
** Function: util_replaceSubstring_dynamic
** Description: Same as above, but it frees the input str.
*********************************************************************/
char* util_replaceSubstring_dynamic(char* str, char* toBeReplaced, int repl){
    char new_str[2050];
    memset(new_str, 0, sizeof(new_str));
    strcpy(new_str, str);
    free(str);
    for (int i = 0; i < (strlen(new_str) - 1); i++){
        if (new_str[i] == toBeReplaced[0] && new_str[i+1] == toBeReplaced[0]){
            new_str[i] = '%';
            new_str[i+1] = 'i';
            char str_temp[2050];
            sprintf(str_temp,new_str, repl);
            strcpy(new_str, str_temp);
        }
    }
    char* returned = malloc(sizeof(char) * (strlen(new_str) + 1));
    strcpy(returned, new_str);
    return returned;
}

/*********************************************************************
** Function: util_extractTokens
** Description: extracts the tokens from the command and returns
** a 2d array of strs.
*********************************************************************/
char **util_extractTokens(char* command){
    SIZE_ARGS = 0;
    struct LinkedList* tmp_list = linkedlist_create();
    char* cpy = (char*) calloc(strlen(command) + 1, sizeof(char));
    strcpy(cpy, command);

    //extract token from command
    char* token = strtok(cpy," ");

    while (token != 0) {
        //add token to linked list for buffering
        linkedlist_push_back(tmp_list,token);
        token = strtok(0, " "); 
    } 

    //create a 2d array of strings and sets the size to the lenght of the
    //linked list + 1
    char** args = malloc((tmp_list->length + 1) * sizeof(char*));
    struct Node* node = tmp_list->head;
    int i = 0;

    //parse the linked list and store each of the tokens in the 2d array
    while(node && i <= tmp_list->length){
        args[i] = (char*) malloc(sizeof(char) * (strlen(node->val) + 1));
        strcpy(args[i], (char*)node->val);
        node = node->next;
        i++;
    }
    args[tmp_list->length] = NULL; //sets last str to null
    args[tmp_list->length-1][strlen(args[tmp_list->length-1])-1] = 0; //removes newline from cmd
    SIZE_ARGS = tmp_list->length;

    linkedlist_delete(tmp_list);
    free(cpy);
    return args;
}

/*********************************************************************
** Function: util_freeTokens
** Description: Frees the 2d array created by the getTokens function.
*********************************************************************/
void util_freeTokens(char** args){
    for (int i = 0; i < (SIZE_ARGS); i++){
            free(args[i]);
        }
    free(args);
}

/*********************************************************************
** Function: util_checkStrings
** Description: Returns 1 if the strs are equal and 0 otherwise
*********************************************************************/
int util_checkStrings(char* first, char*second){ //returns 1 if they are equal
    int returned = strcmp(first, second);
    if (returned == 0)
        return 1;
    else
        return 0;
}

/*********************************************************************
** Function: util_isBuilt_in
** Description: Retuns different values depending the cmd input.
*********************************************************************/
int util_isBuilt_in(char* cmd){
    if (util_checkStrings("exit", cmd)){
        return 55;
    } else if (util_checkStrings("cd", cmd)){
        return 65;
    } else if (util_checkStrings("status", cmd)){
        return 75;
    } else {
        return 0;
    }
}

/*********************************************************************
** Function: util_hasRedirect
** Description: Returns different values for the type of redirection
** args has.
*********************************************************************/
int util_hasRedirect(char** args){
    int flag_in = 0; // <
    int flag_out = 0; // >
    //parse through args to see if there are < or >
    for (int i = 0; i < SIZE_ARGS; i++){
        if (util_checkStrings(">", args[i])){
            flag_out = 1;
        } else if (util_checkStrings("<", args[i])){
            flag_in = 1;
        }
    }

    if (flag_in && !flag_out){
        return 40;
    } else if (!flag_in && flag_out){
        return 41;
    } else if (flag_in && flag_out) {
        return 42;
    } else {
        return 0;
    }
}