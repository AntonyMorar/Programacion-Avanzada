/*
    Server program to compute the value of PI
    This program calls the library function 'get_pi'
    It will create child processes to attend requests
    It receives connections using sockets

    Gilberto Echeverria
    gilecheverria@yahoo.com
    21/10/2018
 
    Edited by Antony Adrian Morales Rosas on 13/03/19.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Sockets & Signals libraries
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include <sys/poll.h>
// Custom libraries
#include "get_pi.h"
#include "sockets.h"
#include "fatal_error.h"

#define BUFFER_SIZE 1024
#define MAX_QUEUE 5
#define TIMEOUT 10

///// FUNCTION DECLARATIONS
void usage(char * program);
void waitForConnections(int server_fd);
void attendRequest(int client_fd);
void setupHandlers();
void onInterrupt(int signal);

///// Global flag variable
int interrupted = 0;

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;

    printf("\n=== SERVER FOR COMPUTING THE VALUE OF pi ===\n");

    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }

	// Show the IPs assigned to this computer
	printLocalIPs();

    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
    setupHandlers();
	// Listen for connections from the clients
    waitForConnections(server_fd);
    // Close the socket
    close(server_fd);

    return 0;
}

///// FUNCTION DEFINITIONS

/*
    Explanation to the user of the parameters required to run the program
*/
void usage(char * program)
{
    printf("Usage:\n");
    printf("\t%s {port_number}\n", program);
    exit(EXIT_FAILURE);
}


/*
    Main loop to wait for incomming connections
*/
void waitForConnections(int server_fd)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pid_t new_pid;

    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;

    while (1)
    {
        // ACCEPT
        // Wait for a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
        if (client_fd == -1)
        {
            fatalError("accept");
        }
         
        // Get the data from the client
        inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
        printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);

        // FORK
        // Create a new child process to deal with the client
        new_pid = fork();
        // Parent
        if (new_pid > 0)
        {
            // Close the new socket
            close(client_fd);
        }
        else if (new_pid == 0)
        {
            // Close the main server socket to avoid interfering with the parent
            close(server_fd);
            printf("Child process %d dealing with client\n", getpid());
            // Deal with the client
            attendRequest(client_fd);
            // Close the new socket
            close(client_fd);
            // Terminate the child process
            exit(EXIT_SUCCESS);
        }
        else
        {
            fatalError("fork");
        }

    }
}

/*
    Hear the request from the client and send an answer
*/
void attendRequest(int client_fd)
{
    char buffer[BUFFER_SIZE];
    unsigned long int iterations;
    //Compute PI variables
    double result = 4;
    int sign = -1;
    unsigned long int divisor = 3;
    unsigned long int counter = 0;
    //Polling variables and declaration
    int poll_response;
    struct pollfd server_poll;
    server_poll.fd = client_fd;
    server_poll.events = POLLIN;
    
    // RECV
    // Receive the request
    recvString(client_fd, buffer, BUFFER_SIZE);
    sscanf(buffer, "%lu", &iterations);

    printf(" > Got request from client with iterations=%lu\n", iterations);
    //Set the poll with timeout
    
    while (counter < iterations && !interrupted) {
        poll_response = poll(&server_poll, 1, TIMEOUT);
        
        if(poll_response > 0){// There is something to read at the file descriptor
            break;
        }else if(poll_response == 0){ // The timeout transpired without receiving a message
            // Compute the value of PI
            result += sign * (4.0/divisor);
            sign *= -1;
            divisor += 2;
            counter++;
            
        }
    }
    printf(" > Sending PI=%.20lf with %lu iterations\n", result, counter);
    // Prepare the response to the client
    sprintf(buffer, "The value for PI is: %.20lf with %lu iterations\n", result, counter);
    // SEND
    // Send the response
    sendString(client_fd, buffer);
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
