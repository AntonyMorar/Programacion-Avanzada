/*
 Antony Morales
 Program for a simple bank server
 It uses sockets and threads
 The server will process simple transactions on a limited number of accounts
 
 Gilberto Echeverria
 gilecheverria@yahoo.com
 29/03/2018
 08/04/2018  Added initialization of account balances
 27/10/2018  Simplify the function 'processOperation' by validating the account before the switch
 14/03/2019  Added the new operation for transfer from one account to another
 25/03/19    Edited by Antony Morales.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Signals library
#include <errno.h>
#include <signal.h>
// Sockets libraries
#include <netdb.h>
#include <sys/poll.h>
// Posix threads library
#include <pthread.h>

// Custom libraries
#include "bank_codes.h"
#include "sockets.h"
#include "fatal_error.h"

#define MAX_ACCOUNTS 5
#define BUFFER_SIZE 1024
#define MAX_QUEUE 5

///// Structure definitions

// Data for a single bank account
typedef struct account_struct {
    int id;
    int pin;
    float balance;
} account_t;

// Data for the bank operations
typedef struct bank_struct {
    // Store the total number of operations performed
    int total_transactions;
    // An array of the accounts
    account_t * account_array;
} bank_t;

// Structure for the mutexes to keep the data consistent
typedef struct locks_struct {
    // Mutex for the number of transactions variable
    pthread_mutex_t transactions_mutex;
    // Mutex array for the operations on the accounts
    pthread_mutex_t * account_mutex;
} locks_t;

// Data that will be sent to each structure
typedef struct data_struct {
    // The file descriptor for the socket
    int connection_fd;
    // A pointer to a bank data structure
    bank_t * bank_data;
    // A pointer to a locks structure
    locks_t * data_locks;
} thread_data_t;

//Messages to that the client will pass messages to the server
typedef struct message_client_struct {
    operation_t operation;
    int account_from;
    int account_to;
    float amount;
} client_msg;


// Global variables for signal handlers
int interrupt_exit = 0;


///// FUNCTION DECLARATIONS
void usage(char * program);
void setupHandlers();
void initBank(bank_t * bank_data, locks_t * data_locks);
void readBankFile(bank_t * bank_data);
void waitForConnections(int server_fd, bank_t * bank_data, locks_t * data_locks);
void * attentionThread(void * arg);
void closeBank(bank_t * bank_data, locks_t * data_locks);
int checkValidAccount(int account);
// My FUNCTION DECLARATION
float getBalance(thread_data_t* thread_data, int account_num);
float depositToAccount(thread_data_t* thread_data, int account, float amount);
float withdrawFromAccount(thread_data_t* thread_data, int account, float amount);
float transfer(thread_data_t* thread_data, int sourceAccount, int destAccount, float amount);
void interruption(int signal);
int getTotalTransactions();

///// MAIN FUNCTION
int main(int argc, char * argv[])
{
    int server_fd;
    bank_t bank_data;
    locks_t data_locks;
    
    printf("\n=== SIMPLE BANK SERVER ===\n");
    
    // Check the correct arguments
    if (argc != 2)
    {
        usage(argv[0]);
    }
    
    // Configure the handler to catch SIGINT
    setupHandlers();
    
    // Initialize the data structures
    initBank(&bank_data, &data_locks);
    
    // Show the IPs assigned to this computer
    printLocalIPs();
    // Start the server
    server_fd = initServer(argv[1], MAX_QUEUE);
    // Listen for connections from the clients
    waitForConnections(server_fd, &bank_data, &data_locks);
    // Close the socket
    close(server_fd);
    
    // Clean the memory used
    closeBank(&bank_data, &data_locks);
    
    // Finish the main thread
    pthread_exit(NULL);
    
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
 Modify the signal handlers for specific events
 */
void setupHandlers(){
    struct sigaction new_action;
    
    // Configure the action to take
    // Block all signals during the time the handler funciton is running
    sigfillset(&new_action.sa_mask);
    new_action.sa_handler = interruption;
    // Clear the flags
    new_action.sa_flags = 0;
    
    // Set the handler
    sigaction(SIGINT, &new_action, NULL);
}

/*Send signal if interruption occurs and change the global variable*/
void interruption(int signal){
    printf("\nShutting down\n");
    interrupt_exit = 1;
}

/*
 Function to initialize all the information necessary
 This will allocate memory for the accounts, and for the mutexes
 */
void initBank(bank_t * bank_data, locks_t * data_locks)
{
    // Set the number of transactions
    bank_data->total_transactions = 0;
    
    // Allocate the arrays in the structures
    bank_data->account_array = malloc(MAX_ACCOUNTS * sizeof (account_t));
    // Allocate the arrays for the mutexes
    data_locks->account_mutex = malloc(MAX_ACCOUNTS * sizeof (pthread_mutex_t));
    
    // Initialize the mutexes, using a different method for dynamically created ones
    //data_locks->transactions_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&data_locks->transactions_mutex, NULL);
    for (int i=0; i<MAX_ACCOUNTS; i++)
    {
        //data_locks->account_mutex[i] = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_init(&data_locks->account_mutex[i], NULL);
        // Initialize the account balances too
        bank_data->account_array[i].balance = 0.0;
    }
    
    // Read the data from the file
    readBankFile(bank_data);
}


/*
 Get the data from the file to initialize the accounts
 */
void readBankFile(bank_t * bank_data)
{
    FILE * file_ptr = NULL;
    char buffer[BUFFER_SIZE];
    int account = 0;
    char * filename = "accounts.txt";
    
    file_ptr = fopen(filename, "r");
    if (!file_ptr)
    {
        fatalError("ERROR: fopen");
    }
    
    // Ignore the first line with the headers
    fgets(buffer, BUFFER_SIZE, file_ptr);
    // Read the rest of the account data
    while( fgets(buffer, BUFFER_SIZE, file_ptr) )
    {
        sscanf(buffer, "%d %d %f", &bank_data->account_array[account].id, &bank_data->account_array[account].pin, &bank_data->account_array[account].balance);
        account++;
    }
    
    fclose(file_ptr);
}


/*
 Main loop to wait for incomming connections
 */
void waitForConnections(int server_fd, bank_t * bank_data, locks_t * data_locks)
{
    struct sockaddr_in client_address;
    socklen_t client_address_size;
    char client_presentation[INET_ADDRSTRLEN];
    int client_fd;
    pthread_t new_tid;
    thread_data_t * connection_data = malloc(sizeof(thread_data_t));
    int poll_response;
    int timeout = 500;        // Time in milliseconds (0.5 seconds)
    // Get the size of the structure to store client information
    client_address_size = sizeof client_address;
    
    while (1)
    {
        // Create a structure array to hold the file descriptors to poll
        struct pollfd test_fds[1];
        // Fill in the structure
        test_fds[0].fd = server_fd;
        test_fds[0].events = POLLIN;    // Check for incomming data
        poll_response = poll(test_fds, 1, timeout);
        
        if (client_fd == -1){
            fatalError("ERROR: accept");
        }
        
        if (poll_response == -1){
            // If poll detected an error due to an interruption, exit the loop
            if (errno == EINTR  && interrupt_exit)
            {
                printf("PROGRAM INTERRUPTED");
            }
            else
            {
                fatalError("ERROR: POLL");
            }
        }else if(poll_response == 0 && interrupt_exit){
            printf("WARNING: SERVER DOWN\n");
            break;
        }else if(test_fds[0].revents && POLLIN){
            // ACCEPT
            // Wait for a client connection
            client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_size);
            if (client_fd == -1)
            {
                fatalError("ERROR: CLIENT ACCEPT");
            }
            // Get the data from the client
            inet_ntop(client_address.sin_family, &client_address.sin_addr, client_presentation, sizeof client_presentation);
            printf("Received incomming connection from %s on port %d\n", client_presentation, client_address.sin_port);
            // Prepare the structure to send to the thread
            connection_data->connection_fd = client_fd;
            connection_data->bank_data = bank_data;
            connection_data->data_locks=data_locks;
            // CREATE A THREAD
            pthread_create(&new_tid, NULL, attentionThread, (void*) connection_data);
        }
    }
    
    // Show the number of total transactions (print the total number of transactions performed in the session)
    printf("Number of transactions in the session: %i", getTotalTransactions());
    // Store any changes in the file
}

/*
 Hear the request from the client and send an answer
 */
void * attentionThread(void * arg){
    printf("[%d] Attending request!\n", (int)pthread_self());
    // Receive the data for the bank, mutexes and socket file descriptor
    thread_data_t* thread_data = (thread_data_t*) arg;
    struct pollfd test_fds[1];
    char buffer[BUFFER_SIZE];
    int timeout = 100;
    int poll_response;
    float tempBalance;
    
    // Loop to listen for messages from the client
    while (!interrupt_exit) {
        test_fds[0].fd = thread_data->connection_fd;
        test_fds[0].events = POLLIN;
        poll_response = poll(test_fds, 1, timeout);
        
        if (poll_response == -1){
            // If poll detected an error due to an interruption, exit the loop
            if (errno == EINTR  && interrupt_exit)
            {
                printf("PROGRAM INTERRUPTED");
            }
            else
            {
                fatalError("ERROR: POLL");
            }
        }else if(poll_response > 0){
            if(recvString(thread_data->connection_fd, buffer, BUFFER_SIZE) == 0){
                printf("[%d] Client disconnected!\n", (int)pthread_self());//if the client disconnected
                break;
            }
            client_msg clientMsg; //Creates the struct for the client message
            // Receive the request
            sscanf(buffer, "%d %d, %d, %f", (int*)&(clientMsg.operation),&(clientMsg.account_from),&(clientMsg.account_to), &(clientMsg.amount));
            // Process the request being careful of data consistency
            if(clientMsg.operation == EXIT){
                printf("[%d] Client exit the ATM (Conection closed)\n", (int)pthread_self());
                break;
            }
            
            switch (clientMsg.operation) {
                case CHECK:
                    tempBalance = getBalance(thread_data, clientMsg.account_from);
                    break;
                case DEPOSIT:
                    tempBalance = depositToAccount(thread_data, clientMsg.account_from, clientMsg.amount);
                    break;
                case WITHDRAW:
                    tempBalance = withdrawFromAccount(thread_data, clientMsg.account_from, clientMsg.amount);
                    break;
                case TRANSFER:
                    tempBalance = transfer(thread_data, clientMsg.account_from, clientMsg.account_to, clientMsg.amount);
                    break;
                default:
                    fatalError("ERROR: NOT VALID OPERATION");
            }
            // Send a reply
            sprintf(buffer, "%i %f",  OK, tempBalance);
            sendString(thread_data->connection_fd, buffer, BUFFER_SIZE);
            printf("[%d] Transaction complete", (int)pthread_self());
        }
    }
    
    // Update the number of transactions
    
    // Send a end reply
    sprintf(buffer, "%i %d",  BYE, 0);
    sendString(thread_data->connection_fd, buffer, BUFFER_SIZE);
    free(thread_data);
    pthread_exit(NULL);
}

/*
 Free all the memory used for the bank data
 */
void closeBank(bank_t * bank_data, locks_t * data_locks)
{
    printf("DEBUG: Clearing the memory for the thread\n");
    free(bank_data->account_array);
    free(data_locks->account_mutex);
}


/*
 Return true if the account provided is within the valid range,
 return false otherwise
 */
int checkValidAccount(int account)
{
    return (account >= 0 && account < MAX_ACCOUNTS);
}

// Return the total transaction in te session
int getTotalTransactions(){
    return 6;
}


float getBalance(thread_data_t* thread_data, int accountNumber){
    float clientBalance = thread_data->bank_data->account_array[accountNumber].balance;
    thread_data->bank_data->total_transactions++;
    return clientBalance;
}

float depositToAccount(thread_data_t* thread_data, int account, float amount){
    float clientBalance = thread_data->bank_data->account_array[account].balance += amount;
    thread_data->bank_data->total_transactions++;
    return clientBalance;
}

float withdrawFromAccount(thread_data_t* thread_data, int account, float amount){
    float clientBalance = thread_data->bank_data->account_array[account].balance -= amount;
    thread_data->bank_data->total_transactions++;
    return clientBalance;
}

float transfer(thread_data_t* thread_data, int sourceAccount, int destAccount, float amount){
    float clientBalance = thread_data->bank_data->account_array[sourceAccount].balance -= amount;
    thread_data->bank_data->account_array[destAccount].balance += amount;
    thread_data->bank_data->total_transactions++;
    return clientBalance;
}
