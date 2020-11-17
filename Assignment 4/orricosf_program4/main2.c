#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "linkedlist.h"
#include "main.h"

/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.

*/

// Size of the buffers
#define SIZE 10

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 6

// Buffer 1, shared resource between input thread and Newline Replacer thread
//int buffer_1[SIZE];
struct LinkedList* buffer_1 = NULL;
// Number of items in the buffer
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between Newline Replacer Thread and Plus Sign thread
//double buffer_2[SIZE];
struct LinkedList* buffer_2 = NULL;
// Number of items in the buffer
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3, shared resource between Plus Sign thread and Output Thread
//double buffer_2[SIZE];
struct LinkedList* buffer_3 = NULL;
// Number of items in the buffer
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;

//Mutex for input sync
pthread_mutex_t mutex_sync = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_sync = PTHREAD_COND_INITIALIZER;


int input_done = 0;

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
    //pthread_mutex_lock(&mutex_sync);
    char *in = NULL;
    size_t size;
    getline(&in, &size, stdin);
    return in;
    //pthread_mutex_unlock(&mutex_sync);
}

/*
 Put an item in buff_1
*/
void put_buff_1(char* item){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_1);

    //if (input_done){
    //    pthread_cond_signal(&full_1);
    //    pthread_mutex_unlock(&mutex_1);
    //}

    if (item[0] >= ' '){
        if (!buffer_1) 
            buffer_1 = linkedlist_create();
        struct LinkedList* tmp = util_extractTokens(item);
        struct Node* node = tmp->head;
        //print(tmp);
        while(node){
            if (util_checkStrings("STOP\n",(char*) node->val)){
                input_done = 1;
                fflush(stdout);
                break;
            }
            if (!input_done)
                linkedlist_push_back(buffer_1, node->val);
            else
                free(node->val);
            node = node->next;
        }
        linkedlist_delete(tmp);
    }

   // write(1, "\nBuffer 1 after input: ", strlen("\nBuffer 1 after input: "));
    //print(buffer_1);
    //write(1, "\n", 1);

    pthread_cond_signal(&full_1);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);
    //pthread_cond_signal(&full_1);
}

void *get_input(void *args)
{
    while (!input_done)
    {
        pthread_mutex_lock(&mutex_sync);
        char* item = get_user_input();
        //char a[1000];
        //sprintf(a, "\n\nINPUT: %s\n", item);
        //write(1, a, strlen(a));
        //write(1, "\n\n", 2);
        //write(1, item, strlen(item));
        if (util_checkStrings("STOP\n", item)){
            input_done = 1;
            //pthread_cond_signal(&full_1);
            //break;
        }
        put_buff_1(item);
        pthread_mutex_unlock(&mutex_sync);
    }
    return NULL;
}


  ////////////////////////////////////////////////////////////////////////////////
 //                             REPLACE NEWLINE                                //
////////////////////////////////////////////////////////////////////////////////

/*
Get the next item from buffer 1
*/
struct LinkedList* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  
    pthread_mutex_lock(&mutex_1);

    while(!buffer_1){
        pthread_cond_wait(&full_1, &mutex_1);
    }

    //write(1, "\nBuffer 1 before Copy: ", strlen("\nBuffer 1 before Copy: "));
    //print(buffer_1);
    //write(1, "\n", 1);

    if (!buffer_1){
        pthread_mutex_unlock(&mutex_1);
        return NULL;
    }

    struct LinkedList* out = linkedlist_copy_str(buffer_1);

    linkedlist_free_dynamic_val(buffer_1);
    buffer_1 = NULL;

    //write(1, "\nBuffer 1 after NULL: ", strlen("\nBuffer 1 after NULL: "));
    //print(buffer_1);
    //write(1, "\n", 1);

    //write(1, "\nCopy of Buffer 1: ", strlen("\nCopy of Buffer 1: "));
    //print(out);
    //write(1, "\n", 1);

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);
     // Return the item
    return out;
}

/*
 Put an item in buff_2
*/
void put_buff_2(struct LinkedList* item){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_2);
    // Put the item in the buffer

    if(!item){
        // Signal to the consumer that the buffer is no longer empty
        pthread_cond_signal(&full_2);
        // Unlock the mutex
        pthread_mutex_unlock(&mutex_2);
        return;
    }

    if (!buffer_2)
        buffer_2 = linkedlist_copy_str(item);
    else {
        struct LinkedList* tmp = linkedlist_copy_str(item);
        struct Node* node = tmp->head;
        while(node){
            linkedlist_push_back(buffer_2, node->val);
            node = node->next;
        }
    }

    //write(1, "\n\nPut Buffer 2:\n\n", strlen("\n\nPut Buffer 2:\n\n"));
    //print(buffer_2);
    //write(1, "\n\n", strlen("\n\n"));

    // Signal to the consumer that the buffer is no longer empty
    pthread_cond_signal(&full_2);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);
}

char* line_separator_replacer(char* line, char replacement){
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == '\n'){
            line[i] = replacement;
        }
    }
    return line;
}

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

void *line_separator_thread(void *args)
{
    struct LinkedList* item = 0;
    //for (int i = 0; buffer_1/*i < global_count || buffer_1*/; i++)
    while(1){

        item = get_buff_1();
        //process linked list here and put to buffer 2
        item = process_separators(item);

        //write(1, "\nAfter processed Separators: ", strlen("\nAfter processed Separators: "));
        //print(buffer_1);
        //write(1, "\n", 1);

        put_buff_2(item);
        //linkedlist_free_dynamic_val(item);
        item  = NULL;

        pthread_mutex_lock(&mutex_sync);
        if (input_done){
            pthread_mutex_unlock(&mutex_sync);
            break;
        }
        pthread_mutex_unlock(&mutex_sync);
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
    // Lock the mutex before checking if the buffer has data
    //pthread_mutex_lock(&mutex_2);
    //while (count_2 == 0)
        // Buffer is empty. Wait for the producer to signal that the buffer has data
        //pthread_cond_wait(&full_2, &mutex_2);
    pthread_mutex_lock(&mutex_2);
    while(!buffer_2){
        pthread_cond_wait(&full_2, &mutex_2);
    }

    if (!buffer_2)
        return NULL;

    struct LinkedList* item = linkedlist_copy_str(buffer_2);

    //write(1, "\n\nBuffer 2 copy:\n\n", strlen("\n\nBuffer 2 copy:\n\n"));
    //print(item);
    //write(1, "\n\n", strlen("\n\n"));

    linkedlist_free_dynamic_val(buffer_2);
    buffer_2 = NULL;

    //count_2--;
    // Unlock the mutex
    //pthread_mutex_unlock(&mutex_2);
    // Return the item
    pthread_mutex_unlock(&mutex_2);
    return item;
}

void put_buff_3(struct LinkedList* item){
    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_3);
    // Put the item in the buffer

    if (!item){
        pthread_cond_signal(&full_3);
        pthread_mutex_unlock(&mutex_3);
        return;
    }
    
    if (!buffer_3)
        buffer_3 = linkedlist_create();

    struct Node* node = item->head;

    while (node) {
        char* value = malloc(sizeof(char) * (strlen(node->val) + 2));
        sprintf(value, "%s%c",(char*) node->val, ' ');
        linkedlist_push_back(buffer_3, value);

        node = node->next;
    }
    
    //write(1, "\n\nPut Buffer 3:\n\n", strlen("\n\nPut Buffer 3:\n\n"));
    //print(buffer_3);
    //write(1, "\n\n", strlen("\n\n"));

    //count_3++;
    // Signal to the consumer that the buffer is no longer empty
    pthread_cond_signal(&full_3);
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
}

struct LinkedList* process_plus_sign(struct LinkedList* item){
    if (!item)
        return NULL;
    struct Node* node = item->head;
    while(node){
        char* curr = node->val;
        curr = util_replaceSubstring_dynamic(curr, "+", "^");
        node->val = curr;
        node = node->next;
    }
    return item;
}

void* plus_sign_replacer_t(void *args){
    struct LinkedList* item;
    while(1)
    {

        item = get_buff_2();
        
        item = process_plus_sign(item);

        put_buff_3(item);
        //linkedlist_free_dynamic_val(item);

        pthread_mutex_lock(&mutex_sync);
        if (input_done){
            pthread_mutex_unlock(&mutex_sync);
            break;
        }
        pthread_mutex_unlock(&mutex_sync);

    }
    return NULL;
}

  ////////////////////////////////////////////////////////////////////////////////
 //                                OUTPUT THREAD                               //
////////////////////////////////////////////////////////////////////////////////

int count_characters_in_list(struct LinkedList* list){
    int count = 0;
    struct Node* node = list->head;
    while (node){
        count += strlen(node->val);
        node = node->next;
    }

    return count;
}

struct LinkedList* get_buff_3(){
    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_3);
    //while (count_3 == 0)
        // Buffer is empty. Wait for the producer to signal that the buffer has data
        pthread_cond_wait(&full_3, &mutex_3);

    struct LinkedList* item = linkedlist_copy_str(buffer_3);

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
    // Return the item
    return item;
}

void *write_output(void *args)
{
    struct LinkedList* item;
    while(1)//for (int i = 0; i < global_count && !input_done; i++)
    {
        //item = get_buff_3();
        pthread_mutex_lock(&mutex_3);
        //pthread_mutex_lock(&mutex_sync);
        //while (count_3 == 0)
            // Buffer is empty. Wait for the producer to signal that the buffer has data
        while(!buffer_3)
            pthread_cond_wait(&full_3, &mutex_3);

        //while(buffer_3->length < 80)
            //pthread_cond_wait(&full_3, &mutex_3);


        item = buffer_3;

        while(count_characters_in_list(item) >= 80){
            int count = 0;
            struct Node* node = item->head;

            while (node){
                struct Node* nextNode = node->next;
                if (count + strlen(node->val) > 80){ //shift val pointer until count + strlen == 80 and print newline
                    //printf("Switched to within 80\n");
                    int i = 0;
                    while(count < 80){
                        char* val = (char*) node->val;
                        char out_val[10];
                        sprintf(out_val, "%c", val[i]);
                        //printf("Outputting character: %s\n", out_val);
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
                    //node = node->next;
                    count = 0;
                    break;
                } else { //print the word and a space (remove word from list)
                    write(1, node->val, strlen(node->val));
                    //printf("Count: %i\n", count);
                    count = count + strlen(node->val);
                    item->length--;
                    item->head = nextNode;
                    node = item->head;
                }
            }
            fflush(stdout);
        }

        pthread_mutex_lock(&mutex_sync);
        if (input_done && buffer_3->length < 80){
            pthread_mutex_unlock(&mutex_sync);
            break;
        }
        pthread_mutex_unlock(&mutex_sync);

        pthread_mutex_unlock(&mutex_3);
        //processing_done = 1;
        //pthread_cond_signal(&full_sync);
        //pthread_mutex_unlock(&mutex_sync);
    }

    //write(1, "\n\nLEFTOVER:\n\n", strlen("\n\nLEFTOVER\n\n:"));
    //print(buffer_3);
    

    return NULL;
}

void signal(){
    pthread_cond_signal(&full_1);
    pthread_cond_signal(&full_2);
    pthread_cond_signal(&full_3);
}


  ////////////////////////////////////////////////////////////////////////////////
 //                                   MAIN                                     //
////////////////////////////////////////////////////////////////////////////////

int main(int argn, char** argv)
{
    srand(time(0));

    pthread_t input_t, line_sep_t, plus_sig_t, output_t;
    // Create the threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&line_sep_t, NULL, line_separator_thread, NULL);
    
    pthread_create(&plus_sig_t, NULL, plus_sign_replacer_t, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    //write(1, "\nINPUT JOINED\n", strlen("\nINPUT JOINED\n"));
    pthread_join(line_sep_t, NULL);
    //write(1, "\nLINE JOINED\n", strlen("\nLINE JOINED\n"));
    pthread_join(plus_sig_t, NULL);
    //(1, "\nPLUS JOINED\n", strlen("\nPLUS JOINED\n"));
    pthread_join(output_t, NULL);
    //write(1, "\nOUT JOINED\n", strlen("\nOUT JOINED\n"));

    //linkedlist_free_dynamic_val(buffer_3);

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
