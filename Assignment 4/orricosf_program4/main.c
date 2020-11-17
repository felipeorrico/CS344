#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "linkedlist.h"
#include "main.h"


// Buffer 1, shared resource between input thread and Newline Replacer thread
struct LinkedList* buffer_1 = NULL;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;



// Buffer 2, shared resource between Newline Replacer Thread and Plus Sign thread
struct LinkedList* buffer_2 = NULL;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3, shared resource between Plus Sign thread and Output Thread
struct LinkedList* buffer_3 = NULL;
// Initialize the mutex for buffer 3
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

/*
Debug function. 
Prints all contents of input linkedlist. 
*/
void print(struct LinkedList* list){
    if (!list){
        write(1, "\nLIST DOES NOT EXIST\n", strlen("\nLIST DOES NOT EXIST\n"));
        return;
    }
    struct Node* node = list->head;
    while(node){
        write(1, node->val, strlen(node->val));
        write(1, " ", 1);
        node = node->next;
    }
    write(1, "\n", 1);
}

  ////////////////////////////////////////////////////////////////////////////////
 //                                 INPUT                                      //
////////////////////////////////////////////////////////////////////////////////

/*
Get input from the user.
This function doesn't perform any error checking.
*/
char* get_user_input(){
    char *in = NULL;
    size_t size;
    getline(&in, &size, stdin);
    return in;
}

/*
 Put an item in buff_1
*/
int put_buff_1(char* item, int done){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_1);

    //if the buffer does not exist, allocate memory for it.
    if (!buffer_1) 
        buffer_1 = linkedlist_create();
    
    //extract tokens from input
    struct LinkedList* tmp = util_extractTokens(item);

    //loop through tokens and add them to buffer
    struct Node* node = tmp->head;
    while(node){
        if (!done){
            if (util_checkStrings(node->val, "STOP\n")){
                done = 1;
            }
            linkedlist_push_back(buffer_1, node->val);
        }
        else
            free(node->val);
        node = node->next;
    }
    linkedlist_delete(tmp);

    //signal buffer has content
    pthread_cond_signal(&full_1);
    //unlock mutex
    pthread_mutex_unlock(&mutex_1);
    return done;
}

/*
Main function for the input thread.
Loops getting input until receives STOP\n.
*/
void *get_input(void *args)
{
    //variable that defines how long the thread should run.
    int input_done = 0;
    while (!input_done)
    {
        char* item = get_user_input();

        input_done = put_buff_1(item, input_done);
    }
    return NULL;
}


  ////////////////////////////////////////////////////////////////////////////////
 //                             REPLACE NEWLINE                                //
////////////////////////////////////////////////////////////////////////////////

/*
Get the next item from buffer 1.
creates a copy of everuthing in buffer_1 and returns it.
*/
struct LinkedList* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  
    pthread_mutex_lock(&mutex_1);

    while (!buffer_1)
        pthread_cond_wait(&full_1, &mutex_1);

    if (!buffer_1){
        pthread_mutex_unlock(&mutex_1);
        return NULL;
    }

    struct LinkedList* out = linkedlist_copy_str(buffer_1);

    linkedlist_free_dynamic_val(buffer_1);
    buffer_1 = NULL;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);
     // Return the item
    return out;
}

/*
 Put an item in buff_2
*/
int put_buff_2(struct LinkedList* item, int done){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_2);

    //checks if input list is not NULL
    if(!item){
        pthread_cond_signal(&full_2);
        pthread_mutex_unlock(&mutex_2);
        return done;
    }

    //if the buffer is NULL, allocate memory for it
    if (!buffer_2)
        buffer_2 = linkedlist_create();
    
    //make a deep copy of the input item
    struct LinkedList* tmp = linkedlist_copy_str(item);

    //loop through the copy until it finds "STOP" and then set variable done to 1
    //also add items in buffer_2 at this step
    struct Node* node = tmp->head;
    while(node){
        if (util_checkStrings(node->val, "STOP"))
            done = 1;
        linkedlist_push_back(buffer_2, node->val);
        node = node->next;
    }

    // Signal to the consumer that the buffer is no longer empty
    pthread_cond_signal(&full_2);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);
    //return new value for done.
    return done;
}
/*
change line separators for nul replacement
*/
char* line_separator_replacer(char* line, char replacement){
    //replace all line separators for a null terminator
    //this is only due to the use of linked lists and
    //tokenization of input. Spaces will be added in buffer_3.
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == '\n'){
            line[i] = replacement;
        }
    }
    //return updated input char array
    return line;
}

/*
iterate through the item list and individually call
line_separator_replacer for each node->val in it.
*/
struct LinkedList* process_separators(struct LinkedList* item){
    if (!item)
        return NULL;
    struct Node* node = item->head;
    while (node){
        node->val = line_separator_replacer(node->val, '\0');
        node = node->next;
    }
    return item;
}

/*
Main function for the line separator thread.
*/
void *line_separator_thread(void *args)
{
    struct LinkedList* item = NULL;
    int sep_done = 0;
    while(!sep_done){

        item = get_buff_1();

        item = process_separators(item);

        sep_done = put_buff_2(item, sep_done);

        item  = NULL;

    }
    return NULL;
}

  ////////////////////////////////////////////////////////////////////////////////
 //                             PLUS SIGN THREAD                               //
////////////////////////////////////////////////////////////////////////////////

/*
Get the next item from buffer 2
*/
struct LinkedList* get_buff_2(){
    
    //lock mutex
    pthread_mutex_lock(&mutex_2);
    
    //wait until buffer_2 has data.
    while(!buffer_2){
        pthread_cond_wait(&full_2, &mutex_2);
    }

    //if the buffer is empty return NULL
    if (!buffer_2)
        return NULL;

    //make deep copy of buffer
    struct LinkedList* item = linkedlist_copy_str(buffer_2);

    //free buffer_2
    linkedlist_free_dynamic_val(buffer_2);
    buffer_2 = NULL;

    //unlock the mutex
    pthread_mutex_unlock(&mutex_2);

    //return copy of buffer
    return item;
}

int put_buff_3(struct LinkedList* item, int done){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_3);

    //if item is null, unlock mutex, signal full and return value of
    //done
    if (!item){
        pthread_cond_signal(&full_3);
        pthread_mutex_unlock(&mutex_3);
        return done;
    }
    
    //if the buffer is null. allocate memory
    if (!buffer_3)
        buffer_3 = linkedlist_create();

    //loop through the item list and push items into buffer_3
    struct Node* node = item->head;

    while (node) {
        //check if the node->val is a known termination string.
        if (util_checkStrings(node->val, "STOP"))
            done = 1;
        char* value = malloc(sizeof(char) * (strlen(node->val) + 2));
        sprintf(value, "%s%c",(char*) node->val, ' ');
        linkedlist_push_back(buffer_3, value);
        node = node->next;
    }
    
    // Signal to the consumer that the buffer is no longer empty
    pthread_cond_signal(&full_3);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
    return done;
}

/*
Replace double occurence of plus signs for ^
*/
struct LinkedList* process_plus_sign(struct LinkedList* item){
    //check if item exist
    if (!item)
        return NULL;

    //loop through item list
    struct Node* node = item->head;
    while(node){
        char* curr = node->val;
        //replace each of the nodes individually.
        curr = util_replaceSubstring_dynamic(curr, "+", "^");
        node->val = curr;
        node = node->next;
    }
    return item;
}

/*
main function for the plus sign replacer thread.
*/
void* plus_sign_replacer_t(void *args){
    struct LinkedList* item;
    int sep_done = 0;
    while(!sep_done)
    {
        //get item from buffer 2
        item = get_buff_2();
        
        //process plus signs
        item = process_plus_sign(item);

        //put into buffer 3
        sep_done = put_buff_3(item, sep_done);
        item = NULL;
    }
    return NULL;
}

  ////////////////////////////////////////////////////////////////////////////////
 //                                OUTPUT THREAD                               //
////////////////////////////////////////////////////////////////////////////////

/*
count how many characters are in the words within the linked list.
*/
int count_characters_in_list(struct LinkedList* list){
    int count = 0;
    struct Node* node = list->head;
    while (node){
        count += strlen(node->val);
        node = node->next;
    }

    return count;
}

/*
get items from buffer 3
*/
struct LinkedList* get_buff_3(){
    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_3);

    //buffer is empty. wait for signal that it is full
    while(!buffer_3)
        pthread_cond_wait(&full_3, &mutex_3);

    //make deep copy of buffer_3 into item.
    struct LinkedList* item = linkedlist_copy_str(buffer_3);

    linkedlist_free_dynamic_val(buffer_3);
    buffer_3 = NULL;

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
    // Return the item
    return item;
}

/*
check if the last node of a linked list is "STOP "
*/
int check_last_node_STOP(struct LinkedList* list){
    struct Node* node = list->head;
    while(node){
        if (!node->next)
            return util_checkStrings(node->val, "STOP ");
        node = node->next;
    }
    return 0;
}

/*
main function for the output thread.
*/
void *write_output(void *args)
{
    struct LinkedList* item = linkedlist_create();
    int output_done = 0;
    while(!output_done)
    {

        if (check_last_node_STOP(item)){
            output_done = 1;
            break;
        }
        
        //get buffer_3 into temporary buffer
        struct LinkedList* item_buffer = get_buff_3();

        //add buffer 3 to local buffer
        struct Node* new_node = item_buffer->head;
        while(new_node){
            linkedlist_push_back(item, new_node->val);
            new_node = new_node->next;
        }

        item_buffer = NULL;

        //loop while there are 80 or more characters in list
        while(count_characters_in_list(item) >= 80){

            //break loop if the output is done
            if (output_done)
                break;

            //counts printed characters
            int count = 0;

            //loop through list
            struct Node* node = item->head;

            while (node){
                struct Node* nextNode = node->next;
                //if the string is the stop terminator
                if (util_checkStrings(node->val, "STOP ")){
                    output_done = 1;
                    break;
                }
                //if the already printed characters + the current words characters
                //pass the 80 character per line limit
                if (count + strlen(node->val) > 80){ 
                    int i = 0;
                    while(count < 80){
                        char* val = (char*) node->val;
                        char out_val[10];
                        sprintf(out_val, "%c", val[i]);
                    
                        write (1, out_val, strlen(out_val));
                        count++;
                        i++;
                    }
                    //shift pointer
                    char* val = (char*) node->val;
                    //--i;
                    val = val + i;
                    node->val = val;
                    //print newline
                    write(1, "\n", 1);
                    count = 0;
                    break;
                } else { //print the word and a space (remove word from list)
                    write(1, node->val, strlen(node->val));
                    count = count + strlen(node->val);
                    item->length--;
                    item->head = nextNode;
                    node = item->head;
                }
            }
            fflush(stdout);
        }

        if (output_done)
            break;
    }

    return NULL;
}


  ////////////////////////////////////////////////////////////////////////////////
 //                                   MAIN                                     //
////////////////////////////////////////////////////////////////////////////////

int main(int argn, char** argv)
{

    pthread_t input_t, line_sep_t, plus_sig_t, output_t;
    // Create the threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&line_sep_t, NULL, line_separator_thread, NULL);
    pthread_create(&plus_sig_t, NULL, plus_sign_replacer_t, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(line_sep_t, NULL);
    pthread_join(plus_sig_t, NULL);
    pthread_join(output_t, NULL);


    return EXIT_SUCCESS;
}

  ////////////////////////////////////////////////////////////////////////////////
 //                                 EXTRAS                                     //
////////////////////////////////////////////////////////////////////////////////

/*********************************************************************
** Function: util_extractTokens
** Description: extracts the tokens from the command and returns
** a linked list of strings.
*********************************************************************/
struct LinkedList* util_extractTokens(char* command){
    struct LinkedList* tmp_list = linkedlist_create();
    char* cpy = (char*) calloc(strlen(command) + 1, sizeof(char));
    strcpy(cpy, command);

    //extract token from command
    char* token = strtok(cpy," ");

    while (token != 0) {
        //add token to linked list for buffering
        char* pushback = malloc(sizeof(char) * (strlen(token) + 1));
        strcpy(pushback, token);
        linkedlist_push_back(tmp_list,pushback);
        token = strtok(0, " "); 
    }

    free(command);
    free(cpy);
    return tmp_list;
}

/*********************************************************************
** Function: util_checkStrings
** Description: Returns 1 if the strs are equal and 0 otherwise
*********************************************************************/
int util_checkStrings(char* first, char*second){ //returns 1 if they are equal
    int returned = strcmp(first, second);
    if (returned == 0)
        return 1;
    else
        return 0;
}

/*********************************************************************
** Function: util_replaceSubstring_dynamic
** Description: Same as above, but it frees the input str.
*********************************************************************/
char* util_replaceSubstring_dynamic(char* str, char* toBeReplaced, char* repl){
    char new_str[2050];
    memset(new_str, 0, sizeof(new_str));
    strcpy(new_str, str);
    free(str);
    for (int i = 0; i < (strlen(new_str) - 1); i++){
        if (new_str[i] == toBeReplaced[0] && new_str[i+1] == toBeReplaced[0]){
            new_str[i] = '%';
            new_str[i+1] = 's';
            char str_temp[2050];
            sprintf(str_temp,new_str, repl);
            strcpy(new_str, str_temp);
        }
    }
    char* returned = malloc(sizeof(char) * (strlen(new_str) + 1));
    strcpy(returned, new_str);
    return returned;
}
