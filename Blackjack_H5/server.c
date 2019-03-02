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
#define BUFFER_SIZE 300
#define DEFAULT_PORT 8989

// Function declarations
void usage(char * program);
void waitForConnections(int server_fd);
void communicationLoop();
//Blakcjack functions
int getDeckCard();
void giveCard(Hand *player_hand);
void initPlayerHand(Hand *player_hand);
void reviewHand(Hand *player_hand);

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

void communicationLoop(int connection_fd){
    srand(time(NULL)); // Initialization time for change random cards
    char buffer[BUFFER_SIZE];
    char tempText[BUFFER_SIZE];
    int chars_read = 0;
    int userSelection = 0;
    bool roundOpen = true, dealerClose = true;
    Hand dealer_hand;
    Hand player_hand;
    
    while (1) {
        //Resete the values
        bzero(&buffer, BUFFER_SIZE); //Clear the buffer
        bzero(&tempText, BUFFER_SIZE); //Clear the temporal text
        chars_read = 0; userSelection=0; roundOpen=true, dealerClose=true;
        // First Handshake
        send(connection_fd, "\n\n******************** The table is open ********************\n", strlen("\n\n******************** The table is open ********************\n")+1, 0);
        chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
        if(chars_read == 0){
            printf("[INFO] [%i] Client disconnected\n", getpid());
            return;
        }
        
        printf("Game started, the table is closed\n");
        initPlayerHand(&dealer_hand);
        initPlayerHand(&player_hand);
        sscanf(buffer, "%d", &player_hand.bet);
        printf("Player bet $%d\n", player_hand.bet);
        if(player_hand.bet < 0){
            printf("-- The player leave the table\n");
            break;
        }
        
        giveCard(&dealer_hand);
        giveCard(&player_hand);
        giveCard(&dealer_hand);
        giveCard(&player_hand);
        reviewHand(&player_hand);
        reviewHand(&dealer_hand);
        
        printf("-- Dealer cards: %i, %i -- Total:%i -- Hand type: %i\n", dealer_hand.hand[0], dealer_hand.hand[1], dealer_hand.handValue, dealer_hand.handType);
        printf("-- Player cards: %i, %i -- Total:%i --Hand type: %i\n", player_hand.hand[0], player_hand.hand[1], player_hand.handValue, player_hand.handType);
        sprintf(buffer, "****** Dealer ******\ncards: X(hidden), %i\ntotal: %i\n****** Player ******\ncards: %i, %i\ntotal: %i\n\n", dealer_hand.hand[1],dealer_hand.handValue - dealer_hand.hand[0], player_hand.hand[0], player_hand.hand[1],player_hand.handValue);
        
        if(player_hand.handValue == 21 && dealer_hand.handValue != 21){//Check if is blackjack
            printf("-- Player get Blackjack\n");
            sprintf(tempText, "BLACKJACK! You win $%d\n~", (int)(player_hand.bet * 1.5));
            strcat(buffer, tempText);
            roundOpen = false;
        }
        send(connection_fd, buffer, strlen(buffer)+1, 0);
        
        while(roundOpen){
            // Get the acction from the user
            chars_read = recvMessage(connection_fd, buffer, BUFFER_SIZE);
            if (chars_read <= 0){
                printf("[INFO] [%i] Client disconnected\n", getpid());
                break;
            }
            //Player selects the turn option
            sscanf(buffer, "%d", &userSelection);
            printf("The player select: %i\n", userSelection);
            
            sprintf(buffer, "\n\n------------ Table movements ------------\n");
            
            switch (userSelection) {
                case 1: //User selection equals to hit
                    giveCard(&player_hand);
                    reviewHand(&player_hand);
                    printf("-- Player receive the card %d, total: %d\n",player_hand.hand[player_hand.cardCounter-1], player_hand.handValue);
                    sprintf(tempText, "-- You receive the card: %d\n",player_hand.hand[player_hand.cardCounter-1]);
                    strcat(buffer, tempText);
                    if(player_hand.handType == 1){//If the player overcome the 21
                        printf("The player lose with hand value: %d\n", player_hand.handValue);
                        sprintf(tempText, "-- Total hand of the dealer: %d\n-- Your hand: %d\n",dealer_hand.handValue, player_hand.handValue);
                        strcat(buffer, tempText);
                        sprintf(tempText, "LOSE! -$%d\n~", player_hand.bet);
                        strcat(buffer, tempText);
                        send(connection_fd, buffer, strlen(buffer)+1, 0);
                        roundOpen = false;
                    }
                    break;
                case 2: //User selection equals to stand
                    dealerClose = false;
                    sprintf(tempText, "-- Dealer open the first card with value: %d\n",dealer_hand.hand[0]);
                    strcat(buffer, tempText);
                    if (player_hand.handValue < dealer_hand.handValue) {
                        printf("The player lose with hand value: %d\n", player_hand.handValue);
                        sprintf(tempText, "-- Total hand of the dealer: %d\n-- Your hand: %d\n",dealer_hand.handValue, player_hand.handValue);
                        strcat(buffer, tempText);
                        sprintf(tempText, "You LOSE! -$%d\n~",player_hand.bet);
                        strcat(buffer, tempText);
                        send(connection_fd, buffer, strlen(buffer)+1, 0);
                        roundOpen = false;
                    }else{
                        while (dealer_hand.handValue <= 16) {
                            giveCard(&dealer_hand);
                            reviewHand(&player_hand);
                            printf("-- Dealer take the card: %d, total: %d\n",dealer_hand.hand[dealer_hand.cardCounter-1], dealer_hand.handValue);
                            sprintf(tempText, "-- Dealer takes the card: %d\n",dealer_hand.hand[dealer_hand.cardCounter-1]);
                            strcat(buffer, tempText);
                        }
                        if(dealer_hand.handType == 1 || dealer_hand.handValue < player_hand.handValue){
                            printf("The player win with hand value: %d\n", player_hand.handValue);
                            sprintf(tempText, "-- Total hand of the dealer: %d\n-- Your hand: %d\n",dealer_hand.handValue, player_hand.handValue);
                            strcat(buffer, tempText);
                            sprintf(tempText, "You WIN! -$%d\n~",player_hand.bet);
                            strcat(buffer, tempText);
                            send(connection_fd, buffer, strlen(buffer)+1, 0);
                            roundOpen = false;
                        }else if(dealer_hand.handValue > player_hand.handValue){
                            printf("The player lose with hand value: %d\n", player_hand.handValue);
                            sprintf(tempText, "-- Total hand of the dealer: %d\n-- Your hand: %d\n",dealer_hand.handValue, player_hand.handValue);
                            strcat(buffer, tempText);
                            sprintf(tempText, "You LOSE! -$%d\n~",player_hand.bet);
                            strcat(buffer, tempText);
                            send(connection_fd, buffer, strlen(buffer)+1, 0);
                            roundOpen = false;
                        }else{
                            printf("TIE: player: %d, dealer %d\n", player_hand.handValue, dealer_hand.handValue);
                            sprintf(tempText, "-- Total hand of the dealer: %d\n-- Your hand: %d\n",dealer_hand.handValue, player_hand.handValue);
                            strcat(buffer, tempText);
                            sprintf(tempText, "You TIE! -$%d\n~",player_hand.bet);
                            strcat(buffer, tempText);
                            send(connection_fd, buffer, strlen(buffer)+1, 0);
                            roundOpen = false;
                        }
                    }
                    break;
                default:
                    printf("Player selects an invalid option\n");
                    sprintf(buffer, "INVALID option\n");
                    send(connection_fd, buffer, strlen(buffer)+1, 0);
                    break;
            }
            if (roundOpen) { //Check if the round is over
                //Save in the buffer the actual state of the cards in the table
                sprintf(tempText, "****** Dealer ******\ncards: ");
                strcat(buffer, tempText);
                for (int i = 0; i<dealer_hand.cardCounter; i++) {
                    if (dealerClose && i==0) {
                        sprintf(tempText,"%s ", "X(hidden)");
                        strcat(buffer, tempText);
                    }else if(dealer_hand.hand[i] == 1){//Detects if the card is an Ace
                        sprintf(tempText,"%c ", 'A');
                        strcat(buffer, tempText);
                    }else{
                        sprintf(tempText,"%d ", dealer_hand.hand[i]);
                        strcat(buffer, tempText);
                    }
                }
                if (dealerClose) {
                    sprintf(tempText, "\ntotal: %i\n****** Player ******\ncards:", dealer_hand.handValue - dealer_hand.hand[0]);
                }else{
                    sprintf(tempText, "\ntotal: %i\n****** Player ******\ncards:", dealer_hand.handValue);
                }
                strcat(buffer, tempText);
                for (int i = 0; i<player_hand.cardCounter; i++) {
                    if(player_hand.hand[i] == 1){//Detects if the card is an Ace
                        sprintf(tempText,"%c ", 'A');
                        strcat(buffer, tempText);
                    }else{
                        sprintf(tempText,"%d ", player_hand.hand[i]);
                        strcat(buffer, tempText);
                    }
                }
                sprintf(tempText, "\ntotal: %i\n\n", player_hand.handValue);
                strcat(buffer, tempText);
                send(connection_fd, buffer, strlen(buffer)+1, 0);
            }
        }
    }
    close(connection_fd);// Close the socket to the client
}

int getDeckCard(){
    int card = rand() % 13 + 1;
    if(card == 11 || card == 12 || card == 13){//Increase the chances of a ten (Jack, Queen, King)
        card = 10;
    }
    return card;
}

//Puts the initial values of the hand
void initPlayerHand(Hand *player_hand){
    player_hand->bet = 0;
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
            player_hand->handType = 1; //The player lose the hand
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

//Changes the value of an Ace depending on the hand
void reviewHand(Hand *player_hand){
    int aceInHand = 0; //0) Not exist, 1)As value 1, 2)As value 11
    for (int i = 0; i<player_hand->cardCounter; i++) {//Check if exist an ace in the hand
        if (player_hand->hand[i] == 1) {
            aceInHand = 1;
        }else if(player_hand->hand[i] == 11){
            aceInHand = 2;
        }
    }
    if (aceInHand == 1 && (player_hand->handValue+10) <= 21) { //Check if the Ace can be 11
        for (int i = 0; i<player_hand->cardCounter; i++) {
            if(player_hand->hand[i] == 1){
                player_hand->hand[i] = 11;
                player_hand->handValue += 10;
                return;
            }
        }
    }else if(aceInHand == 2 && player_hand->handValue > 21){ //Chack if the Ace can be 1
        for (int i = 0; i<player_hand->cardCounter; i++) {
            if(player_hand->hand[i] == 11){
                player_hand->hand[i] = 1;
                player_hand->handValue -= 10;
                return;
            }
        }
    }else{
        return;
    }
}
