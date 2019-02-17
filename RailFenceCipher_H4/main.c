//
//  main.c
//  RailFenceCipher_H4
//
//  Created by Antony Morales on 13/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Library to access POSIX functions
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

#define BUFFER_SIZE 511

/* Function declarations
 *********************************************************/
void parentMenu(FILE * fp_out, FILE * fp_in);
void childProcess(FILE * fp_out, FILE * fp_in);

/* Function tu close the unesessary channels
 *********************************************************/
void preparePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in){
    close(pipe_in[1]); // Close the unwanted channels
    close(pipe_out[0]);
    // Open the file pointers
    *fp_out = fdopen(pipe_out[1], "w");
    if (*fp_out == NULL)
    {
        perror("Unable to open pipe for writing\n");
        exit(EXIT_FAILURE);
    }
    *fp_in = fdopen(pipe_in[0], "r");
    if (*fp_in == NULL)
    {
        perror("Unable to open pipe for reading\n");
        exit(EXIT_FAILURE);
    }
}

/* Function tu close all the channels
 *********************************************************/
void closePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in){
    // Close the file pointers and pipes
    fclose(*fp_in);
    fclose(*fp_out);
    close(pipe_in[0]);
    close(pipe_out[1]);
}

/* Parent menu to select the file and number or rows
 *********************************************************/
void parentMenu(FILE * fp_out, FILE * fp_in){
    printf("Entra al padre\n");
    char buffer[BUFFER_SIZE];
    char fileName[BUFFER_SIZE]; //File name max length size 255
    bool openMenu = true;
    int selection, rails = 0;
    char * process = NULL;

    printf("================= MENU =================\n");
    while (openMenu) {
        printf("Select:\n1) Encode\n2) Decode\n3) Exit program: ");
        scanf("%i", &selection);
        switch (selection) {
            case 1:
            case 2:
                //Scan user data
                printf("insert file name: ");
                scanf("%s", fileName);
                printf("insert number of rails:  ");
                scanf("%d", &rails);
                //Write to child
                sprintf(buffer, "%s %d %d", fileName, rails, selection);
                fprintf(fp_out, "%s\n", buffer);
                fflush(fp_out);
                
                //Read from child
                fscanf(fp_in, "%d", &rails);
                printf("The result sent by child is %d\n", rails);
                break;
            case 3:
                printf("Bye :)\n");
                openMenu = false;
                break;
            default:
                printf("Invalid Option: %d! (Select 1,2 or 3)\n\n", selection);
                break;
        }
    }
}

void childProcess(FILE * fp_out, FILE * fp_in){
    printf("Entra al hijo\n");
    char buffer[BUFFER_SIZE];
    //bool openPipe = true;
    int rails = 0, encode;
    char * process = NULL;
    
    while (1) {
        fscanf(fp_in, "%s %d %d", buffer, &rails, &encode);
        //Read from parent
        printf("estoy en el hijo y recibí: %s %d encode: %d\n", buffer, rails, encode);
        if (encode == 1) {process = "encode";} else if(encode == 2){process = "decode";}else{break;} //write a string to personalize the output print
        //Write to parent
        fprintf(fp_out, "%d\n",(rails*5));
        fflush(fp_out);
    }
}

/* Function tu create the processes
 *********************************************************/
void createProces(){
    pid_t pid;
    //Variables for pipes, every variable indicates the direction of comunication, one read and other write
    int pipe_parent_child[2];
    int pipe_child_parent[2];
    FILE * fp_in = NULL;
    FILE * fp_out = NULL;

    // Open and test the pipes
    if (pipe(pipe_parent_child) == -1){
        perror("ERROR: error opening the pipe parent to child");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe_child_parent) == -1){
        perror("ERROR: error opening the pipe child to parent");
        exit(EXIT_FAILURE);
    }
    
    pid = fork();
    if (pid > 0) {
        preparePipes(pipe_parent_child, pipe_child_parent, &fp_out, &fp_in);
        parentMenu(fp_out, fp_in);
        closePipes(pipe_parent_child, pipe_child_parent, &fp_out, &fp_in);
        printf("Ya se va el padre\n");
    }else if(pid == 0){
        preparePipes(pipe_child_parent, pipe_parent_child, &fp_out, &fp_in);
        childProcess(fp_out, fp_in);
        closePipes(pipe_child_parent, pipe_parent_child, &fp_out, &fp_in);
        printf("Ya se va el hijo\n");
    }else{ // No process created
        printf("ERROR: unable to fork\n");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, const char * argv[]) {
    printf("******************************** Rail Fence Cipher Program ********************************\n");
    createProces();
    return 0;
}
