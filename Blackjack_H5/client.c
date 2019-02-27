#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Sockets libraries
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Constant values
#define BUFFER_SIZE 200
#define DEFAULT_PORT 8989

// Function declarations
void usage(char * program);
void connectToServer(char * address, char * port);
void communicationLoop();

int main(int argc, char * argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
    }
    
    connectToServer(argv[1], argv[2]);
    
    return 0;
}

void usage(char * program)
{
    printf("Usage: %s {server_address} {port_number}\n", program);
    exit(EXIT_SUCCESS);
}

void connectToServer(char * address, char * port)
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
    if (getaddrinfo(address, port, &hints, &address_info) == -1)
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
    
    // Connect to the server
    if (connect(server_fd, address_info->ai_addr, address_info->ai_addrlen) == -1)
    {
        close(server_fd);
        freeaddrinfo(address_info);
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }
    
    // Free the list of addresses
    freeaddrinfo(address_info);
    
    communicationLoop(server_fd);
}

void communicationLoop(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    int chars_read = 0;
    int num_messages = 0;
    
    while(1)
    {
        
        printf("Enter a message to send: ");
        // Prepare the message
        fgets(buffer, BUFFER_SIZE, stdin);
        send(connection_fd, buffer, strlen(buffer)+1, 0);
        
        // Read the reply
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
            printf("Server has disconnected\n");
            break;
        }
        num_messages++;
        printf("The server sent me: '%s'\n", buffer);
    }
    
    // Close the socket to the client
    close(connection_fd);
}