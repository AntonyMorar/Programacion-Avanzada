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
#define BUFFER_SIZE 200
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
        if (!strncmp(buffer, "BUST", 5)){
            printf("You Lose\n");
            break;
        }else if(!strncmp(buffer, "WIN", 4)){
            printf("You Win!\n");
            break;
        }else if(!strncmp(buffer, "TIE", 4)){
            printf("It´s a Tie\n");
            break;
        }
        printf("%s",buffer);
        printf("Select an option: 1) Hit, 2) Stand 3) Double Down: ");
        scanf("%s", buffer);
        send(connection_fd, buffer, strlen(buffer)+1, 0);
        
        
        /*
        // Get the reply from the server
        chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
        if (chars_read <= 0){
            break;
        }
        
        printf("Select an option: 1) Stand, 2) Hit 3) Double Down");
        // Prepare the message
        scanf("%s", buffer);
        send(connection_fd, buffer, strlen(buffer)+1, 0);
        
        // Give feedback to the user
        printf("%s\n", buffer);
        // Finish when the game is won
        if (!strncmp(buffer, "Right", 6))
        {
            printf("Congratulations!\n");
            break;
        }
        */
    }
    
    // Close the socket to the client
    close(connection_fd);
}
