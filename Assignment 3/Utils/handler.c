#include "handler.h"
#include "../Utils/intList.h"
#include "../Utils/linkedlist.h"
#include "../Utils/utils.h"
#include "../Built-in/cd.h"
#include "../Built-in/exit.h"
#include "../Shell/Shell.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


/*********************************************************************
** Function: Handler_KillFG
** Description: Just a placeholder function, Nothing to see here...
*********************************************************************/
void Handler_KillFG(int signum) {
    //I'm a lazy function...
    //I'm here just to ignore everything.
}

/*********************************************************************
** Function: Handler_SetFGOnly
** Description: Intercept ^Z signal and change output mode
*********************************************************************/
void Handler_SetFGOnly(int signum){
    if (Shell_GetFGONLY()){
        Shell_SetFGONLY(0);
        char out[] = "\nExiting foreground-only mode\n";
        write(Shell_get_local_dup_o(), out, strlen(out));
    } else {
        Shell_SetFGONLY(1);
        char out[] = "\nEntering foreground-only mode (& is now ignored)\n";
        write(Shell_get_local_dup_o(), out, strlen(out));
    }
}