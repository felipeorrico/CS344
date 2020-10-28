#ifndef CD_H
#define CD_H

#include <string.h>

void CD_getHome();
char* CD_extractHome(char* pathname);
void CD_changeDirectory(char* path);
char* CD_HOME();

#endif