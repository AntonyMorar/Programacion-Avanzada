# Simple_Bank

This program simulates a network consisting on the central server of a bank, and client programs that would be running on the ATM machines, connecting to the central server to validate operations.

Complete the server program to create a bank system that can receive connections via sockets, and that creates threads to serve each client that connects.

## Data structures
- `bank_t` stores the data of the bank. It has an int to count the total number of transactions performed, and an array of floats that store the balance of the accounts. Each account is identified by the index in that array.
- `locks_t` has the mutex locks for the variables in the previous structure. One for the total number of transactions, and one for each of the accounts.
-   `thread_data_t ` has pointers to the previous two, plus the file descriptor of the socket to communicate with the client. This is the one that is passed to each thread as a parameter.

## Communication protocol
The client will pass messages to the server consisting of three ints and a float. The first int is a code indicating the type of operation to make (available operations are listed in bank_codes.h as an enum operation_t), the second is the source account number (when taking money out), the third parameter is the destination account (when depositing money), and the fourth is the amount to use for the operation. If the operation selected does not need an amount (for example check balance), and mount of 0 is sent.

The server will reply with a code also listed in bank_codes.h as response_t, and the new balance of the account. If the response does not use a balance, send 0.

## Requirements
Your program must use sockets, polling, signal handlers and threads. Make sure to use the corresponding locks to avoid data corruption. When the server is closed with Ctrl-C, print the total number of transactions performed in the session and write the account balances in the file "accounts.txt" to save any operations done. Make sure to use the exact same format for the file, so that it can be used again by the program.

Please read all the existing code and make sure you understand it. Try to adapt to the existing code, although you are allowed to do any modifications that you consider necessary.

Upload a single .zip file with all the source files and the Makefile. Include also the files I provided, even if you didn't modify them. Remember to write your name in any of the files you modify.
