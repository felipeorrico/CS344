#include "exit.h"
#include "../Utils/intList.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct IntList* process_list;

void exit_processTracking_Init(){
    process_list = intlist_create();
}

void exit_processTracking_Kill(){
    intlist_delete(process_list);
}

void exit_addProcessID(int ID){
    intlist_push_back(process_list, ID);
    printf("Background pid is: %i\n", ID);
    fflush(stdout);
}

void exit_removeProcessID(int ID){
    intlist_removeNode_search(process_list, ID);
}

void exit_print_terminated(int ID, int exitMethod){
    printf("Background pid %i is done: terminated by signal %i\n", ID, exitMethod);
    fflush(stdout);
}

int exit_killProcess(int ID){
    exit_removeProcessID(ID);
    int i = kill((pid_t) ID, SIGKILL);
    return i;
}

int exit_killProcess_noFree(int ID){
    int i = kill((pid_t) ID, SIGKILL);
    return i;
}

int exit_killAllProcesses(){
    struct NodeInt* node = process_list->head;
    while (node){
        int ID = node->val;
        exit_killProcess_noFree(ID);
        node = node->next;
    }
}

void exit_check_ended(){
    struct NodeInt* node = process_list->head;
    while (node){
        int ID = node->val;
        int childExitMethod = 0;
        //printf("Chencking if %i has terminated.\n", ID);
        //fflush(stdout);
        int childPID_actual = waitpid((pid_t) ID, &childExitMethod, WNOHANG);
        if (childPID_actual != 0){ //process terminated
            exit_removeProcessID(ID);
            exit_print_terminated(ID,childExitMethod);
            break;
        }
        node = node->next;
    }
}