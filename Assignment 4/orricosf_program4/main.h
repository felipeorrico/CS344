#ifndef MAIN_H
#define MAIN_H

void print(struct LinkedList* list);
char* get_user_input();
int put_buff_1(char* item, int done);
void *get_input(void *args);
struct LinkedList* get_buff_1();
int put_buff_2(struct LinkedList* item, int done);
char* line_separator_replacer(char* line, char replacement);
struct LinkedList* process_separators(struct LinkedList* item);
void *line_separator_thread(void *args);
struct LinkedList* get_buff_2();
int put_buff_3(struct LinkedList* item, int done);
struct LinkedList* process_plus_sign(struct LinkedList* item);
void* plus_sign_replacer_t(void *args);
int count_characters_in_list(struct LinkedList* list);
struct LinkedList* get_buff_3();
int check_last_node_STOP(struct LinkedList* list);
void *write_output(void *args);
struct LinkedList* util_extractTokens(char* command);
int util_checkStrings(char* first, char*second);
char* util_replaceSubstring_dynamic(char* str, char* toBeReplaced, char* repl);


#endif