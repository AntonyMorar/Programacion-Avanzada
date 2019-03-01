//
//  main.c
//  BlackjackClient_H5
//
//  Created by Antony Morales on 27/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Sockets libraries
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// My sockets library
#include "sockets.h"

// Constant values
#define BUFFER_SIZE 300
#define DEFAULT_PORT 8989

// Function declarations
void usage(char * program);
void communicationLoop();

int main(int argc, char * argv[]){
    if (argc != 3){
        usage(argv[0]);
    }
    printf("*************************************\n********* Blackjack 21 Game *********\n*************************************\n\n");
    
    int connection_fd;
    
    connection_fd = connectToServer(argv[1], argv[2]);
    
    communicationLoop(connection_fd);
    
    return 0;
}

void usage(char * program){
    printf("Usage: %s {server_address} {port_number}\n", program);
    exit(EXIT_SUCCESS);
}

void communicationLoop(int connection_fd){
    char buffer[BUFFER_SIZE];
    int chars_read = 0;
    int bufferSize = 0;
    // First Handshake
    chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
    printf("%s", buffer);
    printf("Insert deal amount: ");
    scanf("%s", buffer);
    send(connection_fd, buffer, strlen(buffer)+1, 0);
    
    while(1){
        chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
        if (chars_read <= 0){
            break;
        }
        printf("%s",buffer);
        bufferSize = (int)strlen(buffer);
        //Check if the round in the game ends
        if('~' == buffer[bufferSize-1]){
            break;
        }
        printf("Select an option: 1) Hit, 2) Stand 3) Double Down: ");
        scanf("%s", buffer);
        send(connection_fd, buffer, strlen(buffer)+1, 0);
    }
    
    // Close the socket to the client
    close(connection_fd);
}
