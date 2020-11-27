/***********************************************************************
** File: dec_client.c
** Author: Felipe Orrico Scognamiglio
** Class: CS344 F2020
** Instructor: Justin Goins
** Assignment: One-Time Pads (Assignment 5)
** Final Version Date: 11/27/2020
***********************************************************************/

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <fcntl.h>

//from client.c example
void error(const char *msg) {
  fprintf(stderr, "%s", msg);
  exit(1);
} 

int util_checkStrings(char* first, char*second){ //returns 1 if they are equal
    int returned = strcmp(first, second);
    if (returned == 0)
        return 1;
    else
        return 0;
}
// from client.c example
void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "DEC_CLIENT: [ERROR] Host not found.\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char* argv[]){
    int FD_PlainText, SIZE_PlainText, FD_key, SIZE_key, PORT_number, FD_socket, charsWritten;
    struct sockaddr_in SERVER_address;
    char STR_PlainText[71000], STR_key[71000];
    memset(STR_PlainText, 0, sizeof(STR_PlainText));

    //open plain text file
    FD_PlainText = open(argv[1], O_RDONLY);
    //read plain text to string and count number of bytes read.
    SIZE_PlainText = read(FD_PlainText, STR_PlainText, sizeof(STR_PlainText));
    //close file
    close(FD_PlainText);

    //open key file, read it and count number of bytes
    FD_key = open(argv[2], O_RDONLY);
    SIZE_key = read(FD_key, STR_key, sizeof(STR_key));
    close(FD_key);

    //key is not long enough
    if (SIZE_key < SIZE_PlainText){
        error("DEC_CLIENT: [ERROR] Key size is smaller than Text size.\n");
    }

    //get port number to server
    PORT_number = atoi(argv[3]);

    //Create the message to be sent to the server:
    // 1) Use termination characters to identify end of message
    // 2) Use identifier "bodm" to identify this message comes from dec_client

    char MESSAGE_SEND[strlen(STR_key) + strlen(STR_PlainText) + 10];
    memset(MESSAGE_SEND, '\0', sizeof(MESSAGE_SEND));
    //copy identifier to beginning of message (add newline to tokenize later)
    strcpy(MESSAGE_SEND, "bodm\n"); //this reads beginning of decryption message
		//if a message with this is sent to enc_server it will return an error
    //copy plain text to message. Since key and plain text have newline,
    //we can tokenize them too using that
    strcat(MESSAGE_SEND, STR_key);
    strcat(MESSAGE_SEND, STR_PlainText);
    //then add termination character to identify end of message
    strcat(MESSAGE_SEND, "eom\n\0"); //this reads end of message ;)

    //setup socket
    FD_socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (FD_socket < 0){
        error("DEC_CLIENT: [ERROR] Could not open socket.\n");
    }
    //use function given in explorations to setup struct
    setupAddressStruct(&SERVER_address, PORT_number, "localhost");

    // Connect to server (from client.c in explorations)
    if (connect(FD_socket, (struct sockaddr*)&SERVER_address, sizeof(SERVER_address)) < 0){
        error("DEC_CLIENT: [ERROR] Could not connect to server.\n");
    }

    //loop while not all characters were sent
    int TOTAL_chars = strlen(MESSAGE_SEND);
    int TOTAL_sent = 0;
    int TOTAL_left = TOTAL_chars;
    while (TOTAL_sent < TOTAL_chars){
        int chars_sent_loop = 0;
        chars_sent_loop = send(FD_socket, MESSAGE_SEND, strlen(MESSAGE_SEND), 0); 
        if (chars_sent_loop < 0){
        error("DEC_CLIENT: [ERROR] Could not write to socket.\n");
        }
        TOTAL_sent += chars_sent_loop;
        TOTAL_left -= chars_sent_loop;
    }
    //in case somehow all characters were not sent
    if (TOTAL_sent < strlen(MESSAGE_SEND)){
        printf("DEC_CLIENT: [WARNING] Not all data written to socket!\n");
    }

    //receive encoded message
    char MESSAGE_RECEIVE[71000];
    memset(MESSAGE_RECEIVE, 0, sizeof(MESSAGE_RECEIVE));
    int TOTAL_received = 0;

    while(TOTAL_received < (strlen(STR_PlainText) + 10)) { //loop for eom
        int loop_read = 0;
        if (strstr(MESSAGE_RECEIVE,"eom") != NULL) //check if message contains eom
            break;
        char RECEIVE_BUFFER[71000];
        memset(RECEIVE_BUFFER, 0, sizeof(RECEIVE_BUFFER));
        loop_read = recv(FD_socket, RECEIVE_BUFFER, sizeof(RECEIVE_BUFFER), 0);
        if (loop_read < 0){
            error("DEC_CLIENT: [ERROR] Could not receive from socket.\n");
            break;
        }
        if (MESSAGE_RECEIVE[0] == '\0') //if the receive str is empty
            strcpy(MESSAGE_RECEIVE, RECEIVE_BUFFER);
        else
            strcat(MESSAGE_RECEIVE, RECEIVE_BUFFER);
    }
    close(FD_socket);

    //server will send message with beginning boem\n
    char* token = strtok(MESSAGE_RECEIVE, "\n");
    int end_of_message_point;
    //write(1, token, strlen(token));
    if (util_checkStrings(token, "bodm")){ //check message came back from dec_server
        //get next token (message)
        token = strtok(NULL, "\n");
        if (util_checkStrings(token, "eom"))
            return EXIT_SUCCESS;
        if (token[0] < ' ') //check if valid message
            return EXIT_SUCCESS;
        //end_of_message_point = strstr(token, "eom") - token;
    } else {
        error("DEC_CLIENT: [ERROR] Message Received is not from dec_server.\n");
    }
    for (int i = 0; i < strlen(token); ++i){
        char out[2];
        memset(out, 0, sizeof(out));
        out[0] = token[i];
        write(STDOUT_FILENO, out, 1);
    }
    write(STDOUT_FILENO, "\n", 1);

    return EXIT_SUCCESS;
}