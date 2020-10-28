#ifndef EXIT_H
#define EXIT_H

#include "../Utils/intList.h"

void exit_processTracking_Init();
void exit_addProcessID(int ID);
void exit_removeProcessID(int ID);
void exit_print_terminated(int ID, int exitMethod);
void exit_check_ended();
int exit_killAllProcesses();
int exit_killProcess(int ID);
int exit_killProcess_noFree(int ID);
void exit_processTracking_Kill();

#endif