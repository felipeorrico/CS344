/***********************************************************************
** File: ServerAPI.c
** Author: Felipe Orrico Scognamiglio
** Class: CS344 F2020
** Instructor: Justin Goins
** Assignment: One-Time Pads (Assignment 5)
** Final Version Date: 11/27/2020
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "intlist.h"

//from client.c example
void error(const char *msg, int exit_code) {
  fprintf(stderr, "%s",msg);
  exit(exit_code);
} 

int util_checkStrings(char* first, char*second){ //returns 1 if they are equal
    int returned = strcmp(first, second);
    if (returned == 0)
        return 1;
    else
        return 0;
}

char* encryptMessage(char* plain, char* key){
  //A = 0, 'space' = 27;
  char alphaNum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  char* out = calloc(strlen(plain) + 1, sizeof(char));
  //malloc(sizeof(char) * (strlen(plain) + 1));
  
  for (int i = 0; i < strlen(plain); i++){
    int key_value, plain_value;
    if (plain[i] == ' ') //space is 27th so index is 26
      plain_value = 26;
    else { //take 65 from character so A == 0
      plain_value = plain[i] - 65;
    }

    if (key[i] == ' ')
      key_value = 26;
    else {
      key_value = key[i] - 65;
    }

    int alphaNum_value;
    if (key_value + plain_value > 26){
      alphaNum_value = (key_value + plain_value) % 27;
    } else {
      alphaNum_value = key_value + plain_value;
    }
    out[i] = alphaNum[alphaNum_value];
  }
  out[strlen(plain)] = '\0';
  return out;
}

char* decryptMessage(char* crypt, char* key){
  //A = 0, 'space' = 27;
  char alphaNum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  char* out = malloc(sizeof(char) * (strlen(crypt) + 1));
  for (int i = 0; i < strlen(crypt); i++){
    int key_value, crypt_value;
    if (crypt[i] == ' ') //space is 27th so index is 26
      crypt_value = 26;
    else { //take 65 from character so A == 0
      crypt_value = crypt[i] - 65;
    }

    if (key[i] == ' ')
      key_value = 26;
    else {
      key_value = key[i] - 65;
    }

    int alphaNum_value;
    if (crypt_value - key_value < 0){
      alphaNum_value = (crypt_value - key_value) + 27;
    } else {
      alphaNum_value = crypt_value - key_value;
    }
    out[i] = alphaNum[alphaNum_value];
  }
  out[strlen(crypt)] = '\0';
  return out;
}

// Set up the address struct for the server socket (from server.c in explorations)
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

/*********************************************************************
** Function: exit_killProcess
** Description: kills process with certain ID and removes from tracking
*********************************************************************/
int exit_killProcess(int ID, struct IntList *process_list){
    intlist_removeNode_search(process_list, ID);
    int i = kill((pid_t) ID, SIGKILL);
    return i;
}

/*********************************************************************
** Function: exit_check_ended
** Description: Parses through the list and checks if a process has
** terminated.
*********************************************************************/
void exit_check_ended(struct IntList *process_list){
    struct NodeInt* node = process_list->head;
    while (node){
        int ID = node->val;
				node = node->next;
        int childExitMethod = 0;
        int childPID_actual = waitpid((pid_t) ID, &childExitMethod, WNOHANG);
        if (childPID_actual != 0){ //process terminated
            intlist_removeNode_search(process_list, ID);
        } else {
					exit_killProcess(ID, process_list);
					waitpid((pid_t) ID, &childExitMethod, WNOHANG);
				}
    }
		int childExitMethod = 0;
		waitpid(-1, &childExitMethod, WNOHANG);
		waitpid(-1, &childExitMethod, WNOHANG);
}

struct IntList *ChildPIDlist;

void signal_handler(int signum){
	if (ChildPIDlist)
		exit_check_ended(ChildPIDlist);
	exit(signum);
}

/* 
** This Function runs the server, binding ports and receving connections
** This Function is the same for ENC_Server and DEC_Server, they only
** differ based on the option input where 1 is ENC_server and 0 is DEC_server 
*/
int runServer(int argc, char*argv[], int option){

  if (argc < 2) { //from server.c
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 

  struct sockaddr_in SERVER_address;
  int FD_socket = 555;
  int PORT_number = atoi(argv[1]);

  FD_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (FD_socket < 0){
		if (option)
    	error("ENC_SERVER: [ERROR] Could not open Socket.\n", 1);
		else
			error("DEC_SERVER: [ERROR] Could not open Socket.\n", 1); 
  }
  
  setupAddressStruct(&SERVER_address, PORT_number);

  int socket_bind = bind(FD_socket, (struct sockaddr *) &SERVER_address, sizeof(SERVER_address));
  if (socket_bind < 0){
		if (option)
    	error("ENC_SERVER: [ERROR] Could not bind Socket!\n", 1);
		else 
			error("DEC_SERVER: [ERROR] Could not bind Socket!\n", 1);
  }

  listen(FD_socket, 5);

  ChildPIDlist = intlist_create();

	signal(SIGTERM, signal_handler);

  while(1){ //loop getting connections
    int FD_connection = 556;
    struct sockaddr_in CLIENT_address;
    socklen_t CLIENT_address_size = sizeof(CLIENT_address);
    FD_connection = accept(FD_socket, (struct sockaddr *)&CLIENT_address, &CLIENT_address_size); 
    if (FD_connection < 0){ //cannot accept request
			if (option)
      	write(STDERR_FILENO, "ENC_SERVER: [ERROR] Could not accept connection.\n", strlen("ENC_SERVER: [ERROR] Could not accept connection.\n"));
			else
				write(STDERR_FILENO, "DEC_SERVER: [ERROR] Could not accept connection.\n", strlen("DEC_SERVER: [ERROR] Could not accept connection.\n"));
    }
    else { //request accepted
      pid_t spawnPID = -55;
      int childExitMethod = -55;
      int childPID_finished = 0;
      spawnPID = fork();

      switch(spawnPID){
        case -1:{ //error
					if (option)
          	write(STDERR_FILENO, "ENC_SERVER: [ERROR] Could not fork process.\n", strlen("ENC_SERVER: [ERROR] Could not fork process.\n"));
					else
						write(STDERR_FILENO, "DEC_SERVER: [ERROR] Could not fork process.\n", strlen("DEC_SERVER: [ERROR] Could not fork process.\n"));
          close(FD_connection);
          break;
        }
        case 0:{ //child
          //receive and encrypt/decrypt information here
          char MESSAGE_received[142000];
          memset(MESSAGE_received, 0, sizeof(MESSAGE_received));

          //read from socket
          while(strstr(MESSAGE_received, "eom") == NULL){
            int loop_read = 0;
            //empty buffer at every loop
            char MESSAGE_buffer[142000];
            memset(MESSAGE_buffer, 0, sizeof(MESSAGE_buffer));
            loop_read = recv(FD_connection, MESSAGE_buffer, sizeof(MESSAGE_buffer), 0);
            if (loop_read < 0){
							if (option)
              	write(STDERR_FILENO, "ENC_SERVER: [ERROR] Could not read from socket!\n", strlen("ENC_SERVER: [ERROR] Could not read from socket!\n"));
							else
								write(STDERR_FILENO, "DEC_SERVER: [ERROR] Could not read from socket!\n", strlen("DEC_SERVER: [ERROR] Could not read from socket!\n"));
              break;
            }
            if (MESSAGE_received[0] == '\0')
              strcpy(MESSAGE_received, MESSAGE_buffer);
            else
              strcat(MESSAGE_received, MESSAGE_buffer);
          }

          //remove termination (and newline)
          int termination_locale = strstr(MESSAGE_received, "eom") - MESSAGE_received;
          MESSAGE_received[termination_locale] = '\0';

          //tokenize with \n
          char* token = strtok(MESSAGE_received, "\n");
          if (!util_checkStrings(token, "boem") && option) { //wrong client sent to server

            char message[] = "boem\nDISCONNECT\neom";
            int sent_chars = 0;
            int remaining_chars = strlen(message);
            while(sent_chars < strlen(message)){
              int loop_send = send(FD_connection, message + sent_chars, remaining_chars, 0);
              if (loop_send < 0){
                write(STDERR_FILENO, "ENC_SERVER: [ERROR] Could not write to socket.\n", strlen("ENC_SERVER: [ERROR] Could not write to socket.\n"));
                break;
              }
              sent_chars += loop_send;
              remaining_chars -= loop_send;
            }
            close(FD_connection);
            exit(1);

          } 
					if (!util_checkStrings(token, "bodm") && !option) { //wrong client sent to server

						//send disconnect message
            char message[] = "bodm\nDISCONNECT\neom";
            int sent_chars = 0;
            int remaining_chars = strlen(message);
            while(sent_chars < strlen(message)){
              int loop_send = send(FD_connection, message + sent_chars, remaining_chars, 0);
              if (loop_send < 0){
                write(STDERR_FILENO, "DEC_SERVER: [ERROR] Could not write to socket.\n", strlen("DEC_SERVER: [ERROR] Could not write to socket.\n"));
                break;
              }
              sent_chars += loop_send;
              remaining_chars -= loop_send;
            }
            close(FD_connection);
            exit(1);

          } else { //Correct client connected to server

            //get key
            token = strtok(NULL, "\n");
            char MESSAGE_key[strlen(token) + 1];
            memset(MESSAGE_key, 0, sizeof(MESSAGE_key));
            strcpy(MESSAGE_key, token);

            //get message
            token = strtok(NULL, "\n");
            char MESSAGE_plain[strlen(token) + 1];
            memset(MESSAGE_plain, 0, sizeof(MESSAGE_plain));
            strcpy(MESSAGE_plain, token);
						char* tmp_msg;

						//encrypt/decrypt message
						if (option)
            	tmp_msg = encryptMessage(MESSAGE_plain, MESSAGE_key);
						else
							tmp_msg = decryptMessage(MESSAGE_plain, MESSAGE_key);
            
						//cat all parts of the message together
						char MESSAGE_ENCRYPTED[142000];
            memset(MESSAGE_ENCRYPTED, 0, sizeof(MESSAGE_ENCRYPTED));
						if (option)
            	strcpy(MESSAGE_ENCRYPTED, "boem\n");
						else
							strcpy(MESSAGE_ENCRYPTED, "bodm\n");
            strcat(MESSAGE_ENCRYPTED, tmp_msg);
            strcat(MESSAGE_ENCRYPTED, "\neom\0");

            //completed encryption/decryption process, send message to client

            int TOTAL_chars = strlen(MESSAGE_ENCRYPTED);
            int TOTAL_sent = 0;
            int TOTAL_left = TOTAL_chars;
            while (TOTAL_sent < TOTAL_chars){
                int chars_sent_loop = 0;
                chars_sent_loop = send(FD_connection, MESSAGE_ENCRYPTED, TOTAL_left, 0); 
                if (chars_sent_loop < 0){
									if (option)
                  	error("ENC_SERVER: [ERROR] Could not write to socket.\n", 1);
									else
										error("DEC_SERVER: [ERROR] Could not write to socket.\n", 1);
                }
                TOTAL_sent += chars_sent_loop;
                TOTAL_left -= chars_sent_loop;
            }
            //in case somehow all characters were not sent
            if (TOTAL_sent < strlen(MESSAGE_ENCRYPTED)){
							if(option)
                printf("ENC_SERVER: [WARNING] Not all data written to socket.\n");
							else
								printf("DEC_SERVER: [WARNING] Not all data written to socket.\n");
            }

						//close connection and exit
            close(FD_connection);
            exit(0);
          }
        }
        default: { //parent
          close(FD_connection);
          intlist_push_back(ChildPIDlist, spawnPID);
          waitpid(-1, &childExitMethod, WNOHANG);
          break;
        }
      }
    }
  }

  close(FD_socket);

}