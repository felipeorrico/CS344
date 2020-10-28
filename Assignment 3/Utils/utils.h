#ifndef UTILS_H
#define UTILS_H

int util_getArgSize();
char* util_replaceSubstring(char*, char*, int);
char* util_replaceSubstring_dynamic(char*, char*, int);
char **util_extractTokens(char* command);
void util_freeTokens(char** args);
int util_checkStrings(char* first, char*second);
int util_isBuilt_in(char* cmd);
int util_hasRedirect(char** args);

#endif