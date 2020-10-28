#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

void Shell_setup();
void Shell_exit();
void Shell_callBuilt_in(int bi_index);
void Shell_redirectIO();
void Shell_redirectIO_bg();
void Shell_returnSTD();
void Shell_run();
char* Shell_getCommand();
pid_t Shell_getCurrentFG();
void Shell_SetExitMethod(int exmthd);
void Shell_SetFGONLY(int i);
int Shell_GetFGONLY();
int Shell_get_local_dup_o();
void Shell_callExternal(int option);

#endif