#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include "cd.h"

char HOME[256];

/*********************************************************************
** Function: CD_extractHome
** Description: returns the HOME directory of user.
*********************************************************************/
char* CD_extractHome(char* pathname){
    char HOME_CPY[256];
    memset(HOME_CPY, 0, sizeof(HOME_CPY));
    strcpy(HOME_CPY, pathname);
    int slashes_counted = 0;
    int breakpoint = 0;
    
    //this assumes that the user is working on OS1, and therefore there are 5 slashes until the home directory
    //i.e. /nfs/stak/users/orricosf/ contains 5 slashes and is my $HOME directory.
    for (; slashes_counted < 5; breakpoint++){
        if (HOME_CPY[breakpoint] == '/') {
            slashes_counted++;
        }
    }

    char home_dir[breakpoint + 1];
    memset(home_dir, 0, sizeof(home_dir));

    for(int i = 0; i < (breakpoint); i++){
        home_dir[i] = HOME_CPY[i];
    }

    char* returned = (char*) malloc(sizeof(char) * (strlen(home_dir) + 1 ));
    strcpy(returned, home_dir);
    
    return returned;
}

/*********************************************************************
** Function:  CD_getHome
** Description: sets global HOME var to output of extractHome
*********************************************************************/
void CD_getHome(){ //run this function in the beginning of the program so home is always set.
    //get path of current working directory
    char pathname[256];
    memset(pathname, 0, sizeof(pathname)); 
    getcwd(pathname, 256);
    
    //extract the HOME path
    char* home = CD_extractHome(pathname);
    memset(HOME, 0, sizeof(HOME)); 
    strcpy(HOME, home);
    free(home);
    //printf("HOME IS: %s\n", HOME);
}

/*********************************************************************
** Function: CD_HOME
** Description: returns HOME global var
*********************************************************************/
char* CD_HOME(){
    return HOME;
}

/*********************************************************************
** Function: CD_changeDirectory
** Description: changes current working directory to input path
** if PATH == NULL it goes home, else it moves to that PATH.
*********************************************************************/
void CD_changeDirectory(char* path){
    if (!path){
        chdir(HOME);
        //printf("Succesfully HOME: %s\n", getcwd(NULL, 256));
    } else {
        int exit_stats = chdir(path);
        if (exit_stats){
            perror("Could not move into selected directory");
        } else {
            //printf("Succesfully CHDIR: %s\n", getcwd(NULL, 256));
        }
    }
}
