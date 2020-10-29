#include "exit.h"
#include "../Utils/intList.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//process tracking list
struct IntList* process_list;

/*********************************************************************
** Function: exit_processTracking_Init
** Description: allocates memory for tracking list
*********************************************************************/
void exit_processTracking_Init(){
    process_list = intlist_create();
}

/*********************************************************************
** Function: exit_processTracking_Kill
** Description: frees memory from tracking list
*********************************************************************/
void exit_processTracking_Kill(){
    intlist_delete(process_list);
}

/*********************************************************************
** Function: exit_addProcessID
** Description: adds process ID to tracking list
*********************************************************************/
void exit_addProcessID(int ID){
    intlist_push_back(process_list, ID);
    printf("Background pid is: %i\n", ID);
    fflush(stdout);
}

/*********************************************************************
** Function: exit_removeProcessID
** Description: removes process ID from tracking list
*********************************************************************/
void exit_removeProcessID(int ID){
    intlist_removeNode_search(process_list, ID);
}

/*********************************************************************
** Function: exit_print_terminated
** Description: prints terminated process and exit method
*********************************************************************/
void exit_print_terminated(int ID, int exitMethod){
    printf("Background pid %i is done: terminated by signal %i\n", ID, exitMethod);
    fflush(stdout);
}

/*********************************************************************
** Function: exit_killProcess
** Description: kills process with certain ID and removes from tracking
*********************************************************************/
int exit_killProcess(int ID){
    exit_removeProcessID(ID);
    int i = kill((pid_t) ID, SIGKILL);
    return i;
}

/*********************************************************************
** Function: exit_killProcess_noFree
** Description: kills process with certain ID but does not remove it
** from the tracking list
*********************************************************************/
int exit_killProcess_noFree(int ID){
    int i = kill((pid_t) ID, SIGKILL);
    return i;
}

/*********************************************************************
** Function: exit_killAllProcesses
** Description: kills all currently running background processes
*********************************************************************/
int exit_killAllProcesses(){
    struct NodeInt* node = process_list->head;
    while (node){
        int ID = node->val;
        exit_killProcess_noFree(ID);
        node = node->next;
    }
}

/*********************************************************************
** Function: exit_check_ended
** Description: Parses through the list and checks if a process has
** terminated.
*********************************************************************/
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
