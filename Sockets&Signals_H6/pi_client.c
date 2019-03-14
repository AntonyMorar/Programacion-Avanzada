/*
    Client program to get the value of PI
    This program connects to the server using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    21/10/2018
 
    Edited by Antony Adrian Morales Rosas on 13/03/19.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
// Sockets & Signals libraries
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/poll.h>
// Custom libraries
#include "sockets.h"
#include "fatal_error.h"


#define BUFFER_SIZE 1024
#define TIMEOUT 10

///// FUNCTION DECLARATIONS
void usage(char * program);
void requestPI(int connection_fd);
void setupHandlers();
void onInterrupt(int signal);

///// Global flag variable
int interrupted = 0;

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int connection_fd;

    printf("\n=== CLIENT FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 3)
    {
        usage(argv[0]);
    }

    // Start the server
    connection_fd = connectSocket(argv[1], argv[2]);
    setupHandlers();
	// Listen for connections from the clients
    requestPI(connection_fd);
    // Close the socket
    close(connection_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {server_address} {port_number}\n", program);
    exit(EXIT_FAILURE);
}


void requestPI(int connection_fd)
{
    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    //Polling variables and declaration
    int poll_response;
    struct pollfd client_poll;

    printf("Enter the number of iterations for PI: ");
    scanf("%lu", &iterations);

    // Prepare the response to the client
    sprintf(buffer, "%lu", iterations);
    // SEND
    // Send the response
    sendString(connection_fd, buffer);
    
    client_poll.fd = connection_fd;
    client_poll.events = POLLIN;
    
    while (!interrupted) {
        poll_response = poll(&client_poll, 1, TIMEOUT);
        if (poll_response > 0){
            if (client_poll.revents  & POLLIN)
            {
                break;
            }
        }else if(poll_response < 0){
            if (errno == EINTR && send(connection_fd, "q", 1 + 1, 0) == -1){
                fatalError("send");
                break;
            }
        }
    }
    // RECV
    // Receive the request
    recvString(connection_fd, buffer, BUFFER_SIZE);
    // Print the result
    printf("%s", buffer);
}

void setupHandlers(){
    struct sigaction new_action;
    struct sigaction old_action;
    
    // Prepare the structure to handle a signal
    new_action.sa_handler = onInterrupt;
    new_action.sa_flags = 0;
    sigfillset(&new_action.sa_mask);
    
    // Catch the signal for Ctrl-C
    sigaction(SIGINT, &new_action, &old_action);
}

void onInterrupt(int signal){
    
    interrupted = 1;
}
