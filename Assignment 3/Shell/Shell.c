#include "Shell.h"
#include "../Utils/intList.h"
#include "../Utils/linkedlist.h"
#include "../Utils/utils.h"
#include "../Built-in/cd.h"
#include "../Built-in/exit.h"
#include "../Utils/handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int local_dup_o;
int local_dup_i;
int local_dup_e;
int fg_only;
pid_t current_fg;
int lastExitMethod;
char* cmd;
char** args;


/*********************************************************************
** Function: Shell_setup
** Description: Initialized all variables required to start the shell
*********************************************************************/
void Shell_setup(){
    //INIT variables
    local_dup_o = dup(1); //save stdout of shell
    local_dup_i = dup(0); //save stdin of shell
    local_dup_e = dup(2); //save stderr of shell
    fg_only = 0; //Output mode (FG only)
    current_fg = -5; //current FG process
    cmd = NULL; //Input shell line
    args = NULL; //array of arguments taken from cmd
    lastExitMethod = 0;

    //INIT "classes"
    exit_processTracking_Init(); //initializing bg process tracking
    CD_getHome(); //get user home
}

/*********************************************************************
** Function: Shell_exit
** Description: Clears all allocated memory by the shell and kills all
** background processess that are being executed by the shell.
*********************************************************************/
void Shell_exit(){
    exit_killAllProcesses();
    util_freeTokens(args);
    exit_processTracking_Kill();
    exit(0);
}

/*********************************************************************
** Function: Shell_get_local_dup_o
** Description: returns shell stdout
*********************************************************************/
int Shell_get_local_dup_o(){
    return local_dup_o;
}

/*********************************************************************
** Function: Shell_getCurrentFG()
** Description: returns PID of current FG process
*********************************************************************/
pid_t Shell_getCurrentFG(){
    return current_fg;
}

/*********************************************************************
** Function: Shell_SetExitMethod
** Description: Sets latest exit method.
*********************************************************************/
void Shell_SetExitMethod(int exmthd){
    lastExitMethod = exmthd;
}

/*********************************************************************
** Function:  Shell_SetFGONLY
** Description: sets foreground only mode of shell
*********************************************************************/
void Shell_SetFGONLY(int i){
    fg_only = i;
}

/*********************************************************************
** Function: Shell_GetFGONLY
** Description:  returns foreground only mode of shell
*********************************************************************/
int Shell_GetFGONLY(){
    return fg_only;
}

/*********************************************************************
** Function: Shell_callBuilt_in
** Description: calls built-in shell commands based on index
*********************************************************************/
void Shell_callBuilt_in(int bi_index){
    switch (bi_index)
    {
    case 55:{ //exit
        Shell_exit();
        break;
    }
    case 65:{ //cd
        if (util_getArgSize() == 1){ //go home
            CD_changeDirectory(NULL);
        } else if (util_getArgSize() == 2) {//go to dir
            CD_changeDirectory(args[1]);
        }
        break;
    }
    case 75:{ //status
        if (WIFSIGNALED(lastExitMethod)){ //checking for signal
            char out[256];
            sprintf(out, "terminated by signal %i\n", WTERMSIG(lastExitMethod));
            write(Shell_get_local_dup_o(), out, strlen(out));
        }
        else
        {
            printf("exit value %i\n", WEXITSTATUS(lastExitMethod));
        }
        
        fflush(stdout);

        break;
    }
    }
}

/*********************************************************************
** Function: Shell_redirectIO
** Description: redirects stdin and stdout to desired files
*********************************************************************/
void Shell_redirectIO(){
    for (int i = 0; i < util_getArgSize(); i++){
        if (util_checkStrings("<", args[i])){ //has input redirection
            if (!args[i+1]) { //if there is no input file for redirection
                int file = open("/dev/null", O_RDONLY);
                if (file == -1) { 
                    printf("Cannot open /dev/null for input\n"); 
                    fflush(stdout);
                    exit(1); 
                }
                int result = dup2(file, 0);
                if (result == -1) { 
                    printf("Cannot open /dev/null for input*\n");   
                    fflush(stdout);
                    exit(1); 
                }
            } else {
                int file = open(args[i+1], O_RDONLY);
                if (file == -1) { 
                    printf("Cannot open %s for input\n", args[i+1]); 
                    fflush(stdout);
                    exit(1); 
                }
                int result = dup2(file, 0);
                if (result == -1) { 
                    printf("Cannot open %s for input*\n", args[i+1]);  
                    fflush(stdout);
                    exit(1); 
                }
            }

        } else if (util_checkStrings(">", args[i])) { //has output redirection
            if (!args[i+1]) { //if there is no input file for redirection
                int file = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (file == -1) { 
                    printf("Cannot open /dev/null for output\n"); 
                    fflush(stdout);
                    exit(1); 
                }
                int result = dup2(file, 1);
                if (result == -1) { 
                    printf("Cannot open /dev/null for output*\n");  
                    fflush(stdout); 
                    exit(1); 
                }
            } else {
                int file = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (file == -1) { 
                    printf("Cannot open %s for output\n", args[i+1]);
                    Shell_returnSTD();
                    fflush(stdout); 
                    exit(1); 
                }
                int result = dup2(file, 1);
                if (result == -1) { 
                    printf("Cannot open %s for output2\n", args[i+1]);
                    Shell_returnSTD(); 
                    fflush(stdout);
                    exit(1); 
                }
            }
        }
    }

}

/*********************************************************************
** Function: Shell_redirectIO_bg
** Description: Redirects stdin and stdout to /dev/null if there is no
** io redirection in the background.
*********************************************************************/
void Shell_redirectIO_bg(){
    int file = open("/dev/null", O_RDONLY);
    if (file == -1) { 
        printf("Cannot open /dev/null for input\n"); 
        fflush(stdout);
        exit(1); 
    }
    int result = dup2(file, 0);
    if (result == -1) { 
        printf("Cannot open /dev/null for input*\n");   
        fflush(stdout);
        exit(1); 
    }
     int file2 = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file2 == -1) { 
        printf("Cannot open /dev/null for output\n"); 
        fflush(stdout);
        exit(1); 
    }
    int result2 = dup2(file2, 1);
    if (result2 == -1) { 
        printf("Cannot open /dev/null for output*\n");  
        fflush(stdout); 
        exit(1); 
    }
}

/*********************************************************************
** Function: Shell_returnSTD
** Description: Error prevention. Prevents that shell doesn't get
** redirected back to standard stdin and stdout
*********************************************************************/
void Shell_returnSTD(){
    int resi = dup2(local_dup_i, 0);
    if (resi == -1) { 
        perror("Could not return to STD in");  
        fflush(stdout); 
        exit(33); 
    }
    int reso = dup2(local_dup_o, 1);
    if (reso == -1) { 
        perror("Could not return to STD out");  
        fflush(stdout); 
        exit(33); 
    }
}

/*********************************************************************
** Function: Shell_callExternal
** Description: Forks and executes external commands to the shell.
*********************************************************************/
void Shell_callExternal(int option){ //option 1 for fg, 0 for bg
    pid_t spawnpid = -5;
    spawnpid = fork();
    int exitmethod = -23;
    if (util_hasRedirect(args) != 0){ //has redirection
        if (dup(0) == local_dup_i){
            exit(1);
        }

        if (spawnpid == 0){ //executed by child
            //redirect here
            Shell_redirectIO();
            char *newarg[] = {args[0], NULL};
            
            if (option){ //fg
                signal(SIGTSTP, SIG_IGN);
            } else { //bg
                //Shell_redirectIO_bg(); //redirect IO to dev/null
                signal(SIGTSTP, SIG_IGN);
                signal(SIGINT, SIG_IGN);
            }

            if (execvp(args[0], newarg) < 0){
                int result = dup2(local_dup_o, 2);
                perror(args[0]);
                exit(1);
            }
        } else if (spawnpid == -1){ //executed by error (output to stdout)
            int result = dup2(local_dup_o, 2);
            perror(args[0]);
            exit(1);
        } else { //executed by parent
            if (option){
                current_fg = spawnpid; //setting current fg process ID
                waitpid(spawnpid, &exitmethod, 0); //wait for FG process
                if (WIFSIGNALED(exitmethod)){ //check for received signal
                    char out[256];
                    sprintf(out, "terminated by signal %i\n", WTERMSIG(exitmethod));
                    write(Shell_get_local_dup_o(), out, strlen(out));
                }
                lastExitMethod = exitmethod; //sets latest exit method
                current_fg = 0; //sets current fg process ID to 0
            } else {
                exit_addProcessID(spawnpid);
            }
        }
    } else { //no redirection
        if (spawnpid == 0){ //executed by child
            if (option){ //fg
                signal(SIGTSTP, SIG_IGN);
            } else { //bg
                Shell_redirectIO_bg(); //redirect IO to /dev/null/
                signal(SIGTSTP, SIG_IGN);
                signal(SIGINT, SIG_IGN);
            }
            if (util_checkStrings("&", args[util_getArgSize() - 1])) //removes & from arguments of command
                args[util_getArgSize() - 1] = NULL;
            if (execvp(args[0], args) < 0){ 
                int result = dup2(local_dup_o, 2);
                perror(args[0]);
                exit(1);
            }
        } else if (spawnpid == -1){ //executed by error (output to stdout)
            int result = dup2(local_dup_o, 2);
            perror(args[0]);
            exit(1);
        } else { //executed by parent
            if (option){
                current_fg = spawnpid;
                waitpid(spawnpid, &exitmethod, 0);
                current_fg = 0;
                if (WIFSIGNALED(exitmethod)){
                    char out[256];
                    sprintf(out, "terminated by signal %i\n", WTERMSIG(exitmethod));
                    write(Shell_get_local_dup_o(), out, strlen(out));
                }
                lastExitMethod = exitmethod;
            } else {
                exit_addProcessID(spawnpid);
            }
        }
    }
}

/*********************************************************************
** Function: Shell_Run
** Description: "main" function of this shell. It handles everything
** related to the shell.
*********************************************************************/
void Shell_run(){
    //catch signals
    signal(SIGINT, Handler_KillFG);
    signal(SIGTSTP, Handler_SetFGOnly);
    //setup shell variables
    Shell_setup();

    //this is where the shell loops. The reason I did not use an atual loop was
    //nothng more than a matter preference
    shellstart:;
    Shell_returnSTD();
    exit_check_ended();
    //get command and replace $$ when found by Parent PID;
    cmd = util_replaceSubstring_dynamic(Shell_getCommand(), "$", getpid());
    //extract all from cmd to separate tokens
    args = util_extractTokens(cmd);
    //free cmd memory
    free(cmd);

    //if the input is a comment...
    if (args[0][0] == '#'){
        util_freeTokens(args);
        goto shellstart;
    }

    int bi = util_isBuilt_in(args[0]);
    if (bi != 0){ //if it is internal command
        Shell_callBuilt_in(bi);
    } else { //external command
        if (fg_only){ //only fg command
            Shell_callExternal(1);
        } else {
            if (util_getArgSize() > 1){ //check if command is being sent with other args
                if (util_checkStrings("&", args[util_getArgSize() - 1])){ //run in bg
                    //printf("Calling Background!\n");
                    //Shell_callExternal_bg();
                    Shell_callExternal(0);
                } else {//run fg
                    //Shell_callExternal_fg();
                    Shell_callExternal(1);
                }
            } else { //if args legth is 1
                //Shell_callExternal_fg();
                Shell_callExternal(1);
            }
        }
    }
    //free arguments to prevent leaks
    util_freeTokens(args);
    goto shellstart;
}

/*********************************************************************
** Function: Shell_getCommand
** Description: Gets user input to the shell.
*********************************************************************/
char* Shell_getCommand() {
    char *command = NULL;
    printf(": ");
    fflush(stdout);
    size_t size;
    getline(&command, &size, stdin);
    fflush(stdin);
    return command;
}

