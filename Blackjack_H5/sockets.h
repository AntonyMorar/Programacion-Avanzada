//
//  main.h
//  BlackjackSockets_H5
//
//  Created by Antony Morales on 27/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//
#ifndef SOCKETS_H
#define SOCKETS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Sockets libraries
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_HAND 11 //Max cards that you can have before overcome 21

// Function declarations
int prepareServer(char * port, int max_queue);
int connectToServer(char * address, char * port);
int recvMessage(int connection_fd, char * buffer, int buffer_size);

typedef struct hand{
    int hand[MAX_HAND];
    int handValue;
    int cardCounter;
    int bet; //Bet in hand because the player can split the hand
    int handType; //Use to know the type of cards that have the player, 0) Not defined(only one card in hand)  1) Overcome 21(Losser Hand), 2) Below or equal 21 (not blackjack), 3) Blackjack, 4)Card match(Below 21 and 2 cards in hand)
}Hand;

#endif /* SOCKETS_H */
