//
//  main.c
//  BlackjackServer_H5
//
//  Created by Antony Morales on 27/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
// Sockets libraries
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// My sockets library
#include "sockets.h"

// Constant values
#define MAX_QUEUE 5
#define BUFFER_SIZE 200
#define DEFAULT_PORT 8989

// Function declarations
void usage(char * program);
void waitForConnections(int server_fd);
void communicationLoop();
//Blakcjack functions

int main(int argc, char * argv[]){
    if (argc != 2){
        usage(argv[0]);
    }
    printf("*****************************************\n********* Blackjack 21 (Server) *********\n*****************************************\n\n");
    
    int server_fd;
    
    server_fd = prepareServer(argv[1], MAX_QUEUE);
    
    // Start waiting for incoming connections
    waitForConnections(server_fd);
    
    return 0;
}

void usage(char * program){
    printf("Usage: %s {port_number}\n", program);
    exit(EXIT_SUCCESS);
}

void waitForConnections(int server_fd){
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof client_address;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid;
    
    while(1){
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
        if (client_fd == -1){
            close(server_fd);
            close(client_fd);
            perror("ERROR: accept");
            break;
        }
        
        new_pid = fork();
        
        if (new_pid == 0){  // Child process
            // Close the main port
            close(server_fd);
            inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, INET_ADDRSTRLEN);
            printf("Connection from: %s, port %i\n", client_presentation, client_address.sin_port);
            
            // Start the communication loop
            communicationLoop(client_fd);
            
            // Terminate the child process
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        else if (new_pid > 0){// Parent process
            close(client_fd);
        }
        else{
            perror("ERROR: fork");
            exit(EXIT_FAILURE);
        }
    }
    
    // Close the server port
    close(server_fd);
}

int getDeckCard(){
    int card = rand() % 11 + 1;
    return card;
}

//Puts the initial valies of the hand
void initPlayerHand(Hand *player_hand){
    player_hand->bet = 50;
    player_hand->cardCounter = 0;
    player_hand->handValue = 0;
    player_hand->handType = 0;
}

//Manage the hand when grant a card
void giveCard(Hand *player_hand){
    player_hand->hand[player_hand->cardCounter] = getDeckCard();
    player_hand->handValue += player_hand->hand[player_hand->cardCounter];
    player_hand->cardCounter++;
    
    if(player_hand->cardCounter >= 2){
        if(player_hand->handValue > 21){
            player_hand->handType = 1; //The player lost the hand
        }else if(player_hand->handValue <= 21){
            player_hand->handType = 2; //The player is still in the game
        }
        if(player_hand->cardCounter == 2){ //Blackjack or card match
            if(player_hand->hand[0] == player_hand->hand[1]){
                player_hand->handType = 4; // The player is still in the game and have a pair
            }else if((player_hand->hand[0] == 11 && player_hand->hand[1] == 10)||(player_hand->hand[1] == 11 && player_hand->hand[0] == 10)){
                player_hand->handType = 3; //The player win the game
            }
        }
    }
}

void communicationLoop(int connection_fd){
    srand(time(NULL)); // Initialization time for change random cards
    char buffer[BUFFER_SIZE];
    char temp[BUFFER_SIZE];
    int chars_read = 0;
    int userSelection = 0;
    bool roundOpen = true;
    Player dealer;
    Player player;
    Hand dealer_hand;
    Hand player_hand;
    
    // First Handshake
    send(connection_fd, "You join to the table\n", strlen("You join to the table\n")+1, 0);
    chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
    
    printf("Game started, the table is closed\n");
    initPlayerHand(&dealer_hand);
    initPlayerHand(&player_hand);
    giveCard(&dealer_hand);
    giveCard(&player_hand);
    giveCard(&dealer_hand);
    giveCard(&player_hand);
    sscanf(buffer, "%d", &player_hand.bet);
    printf("Player bet $%d\n", player_hand.bet);
    
    printf("--- Dealer cards: %i, %i Hand type: %i\n", dealer_hand.hand[0], dealer_hand.hand[1], dealer_hand.handType);
    printf("--- Player cards: %i, %i Hand type: %i\n\n", player_hand.hand[0], player_hand.hand[1], player_hand.handType);
    
    while(roundOpen){
        sprintf(buffer, "***** Dealer *****\n>> cards: X(hidden), %i\n>> total: %i\n***** Player *****\n>> cards: %i, %i\n>> total: %i\n\n", dealer_hand.hand[1],dealer_hand.handValue - dealer_hand.hand[0], player_hand.hand[0], player_hand.hand[1],player_hand.handValue);
        send(connection_fd, buffer, strlen(buffer)+1, 0);
        
        // Get the acction from the user
        chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
        if (chars_read <= 0)
            break;
        sscanf(buffer, "%d", &userSelection);
        printf("The player select %i\n", userSelection);
        
        if(player_hand.handType == 3 && dealer_hand.handType != 3){
            printf("Blackjack: %d\n", player_hand.handType);
            sprintf(buffer, "BLACKJACK");
            send(connection_fd, buffer, strlen(buffer)+1, 0);
            roundOpen = false;
        }else{
            switch (userSelection) {
                case 1: //User selection equals to hit
                    giveCard(&player_hand);
                    printf("***Player recieve %d, total: %d\n",player_hand.hand[player_hand.cardCounter-1], player_hand.handValue);
                    if(player_hand.handType == 1){//If the player overcome the 21
                        printf("The player lost: %d\n", player_hand.handValue);
                        sprintf(buffer, "BUST");
                        send(connection_fd, buffer, strlen(buffer)+1, 0);
                        roundOpen = false;
                    }
                    break;
                case 2: //User selection equals to stand
                    if (player_hand.handValue < dealer_hand.handValue) {
                        printf("The player lost: %d\n", player_hand.handValue);
                        sprintf(buffer, "BUST");
                        send(connection_fd, buffer, strlen(buffer)+1, 0);
                        roundOpen = false;
                    }else{
                        while (dealer_hand.handValue <= 16) {
                            giveCard(&dealer_hand);
                            printf("****Entra a loop de give cartas dealer: %d\n", dealer_hand.handValue);
                        }
                        if(dealer_hand.handType == 1 || dealer_hand.handValue < player_hand.handValue){
                            printf("The player win: %d\n", player_hand.handValue);
                            sprintf(buffer, "WIN");
                            send(connection_fd, buffer, strlen(buffer)+1, 0);
                            roundOpen = false;
                        }else if(dealer_hand.handValue > player_hand.handValue){
                            printf("The player lost: %d\n", player_hand.handValue);
                            sprintf(buffer, "BUST");
                            send(connection_fd, buffer, strlen(buffer)+1, 0);
                            roundOpen = false;
                        }else{
                            printf("TIE: player: %d, dealer %d\n", player_hand.handValue, dealer_hand.handValue);
                            sprintf(buffer, "TIE");
                            send(connection_fd, buffer, strlen(buffer)+1, 0);
                            roundOpen = false;
                        }
                    }
                    break;
                default:
                    printf("Select one of the options\n");
                    break;
            }
        }
    }
    close(connection_fd);// Close the socket to the client
}
