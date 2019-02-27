//
//  main.c
//  Blackjack_H5
//
//  Created by Antony Morales on 26/02/19.
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

// Constant values
#define MAX_QUEUE 5
#define BUFFER_SIZE 200
#define DEFAULT_PORT 8989

// Function declarations
void usage(char * program);
void prepareServer(char * port, int max_queue);
void waitForConnections(int server_fd);
void communicationLoop();

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        usage(argv[0]);
    }
    
    prepareServer(argv[1], MAX_QUEUE);
    
    return 0;
}

void usage(char * program)
{
    printf("Usage: %s {port_number}\n", program);
    exit(EXIT_SUCCESS);
}

void prepareServer(char * port, int max_queue)
{
    struct addrinfo hints;
    struct addrinfo * address_info = NULL;
    int server_fd;
    
    // Clear the structure before filling the data
    bzero(&hints, sizeof hints);
    
    // Configure the structure to search for addresses
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    // Get the list of addresses for the server machine
    if (getaddrinfo(NULL, port, &hints, &address_info) == -1)
    {
        perror("ERROR: getaddrinfo");
        exit(EXIT_FAILURE);
    }
    
    // Open the socket
    server_fd = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (server_fd == -1)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }
    
    int yes = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == -1)
    {
        close(server_fd);
        freeaddrinfo(address_info);
        perror("ERROR: setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Bind the socket to an address and port
    if (bind(server_fd, address_info->ai_addr, address_info->ai_addrlen) == -1)
    {
        close(server_fd);
        freeaddrinfo(address_info);
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
    
    // Free the list of addresses
    freeaddrinfo(address_info);
    
    // Configure the socket to listen for incoming connections
    if (listen(server_fd, max_queue) == -1)
    {
        close(server_fd);
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }
    
    // Start waiting for incoming connections
    waitForConnections(server_fd);
}

void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_addrlen = sizeof client_address;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid;
    
    while(1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_addrlen);
        if (client_fd == -1)
        {
            close(server_fd);
            close(client_fd);
            perror("ERROR: listen");
        }
        
        new_pid = fork();
        
        if (new_pid == 0)   // Child process
        {
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
        else if (new_pid > 0)   // Parent process
        {
            close(client_fd);
        }
        else
        {
            perror("ERROR: fork");
            exit(EXIT_FAILURE);
        }
    }
    
    // Close the server port
    close(server_fd);
}

void communicationLoop(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    int chars_read = 0;
    int num_messages = 0;
    
    while(1)
    {
        // Read the request
        bzero(buffer, BUFFER_SIZE);
        
        chars_read = recv(connection_fd, buffer, BUFFER_SIZE, 0);
        // An error in the communication
        if (chars_read == -1)
        {
            perror("ERROR: recv");
            break;
        }
        // Connection closed by the client
        if (chars_read == 0)
        {
            printf("Client has disconnected\n");
            break;
        }
        
        num_messages++;
        printf("The client sent me: '%s'\n", buffer);
        
        // Prepare the response
        sprintf(buffer, "Reply to message #%d\n", num_messages);
        send(connection_fd, buffer, strlen(buffer)+1, 0);
    }
    
    // Close the socket to the client
    close(connection_fd);
}
