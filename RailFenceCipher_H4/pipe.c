//
//  pipe.c
//  RailFenceCipher_H4
//
//  Created by Antony Morales on 17/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//
#include "pipe.h"

/* Parent menu to select the file and number or rows
 *********************************************************/
void parentMenu(FILE * fp_out, FILE * fp_in){
    char buffer[BUFFER_SIZE];
    char fileName[BUFFER_SIZE]; //File name max length size 255
    bool openMenu = true;
    int selection = 0, rails = 0, error = 0;
    
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
                printf("insert number of rails: ");
                scanf("%d", &rails);
                //Write to child
                sprintf(buffer, "%s %d %d", fileName, rails, selection);
                fprintf(fp_out, "%s\n", buffer);
                fflush(fp_out);
                printf(">> Data sended to child...\n");
                
                //Read from child
                fscanf(fp_in, "%d", &error);
                if(error == 0){
                    printf("\n******* Data recieved from child, generate the new file *******\n\n");
                }else{
                     printf("\n******* Error generating the file *******\n\n");
                }
                
                break;
            case 3:
                sprintf(buffer, "%s %d %d", fileName, rails, selection);
                fprintf(fp_out, "%s\n", buffer);
                fflush(fp_out); //Send empty data to close the child process
                printf(">> Bye :)\n");
                openMenu = false;
                break;
            default:
                printf(">> Invalid Option: %d! (Select 1,2 or 3)\n\n", selection);
                break;
        }
    }
}

/* Child process read and write data from the parent
 *********************************************************/
void childProcess(FILE * fp_out, FILE * fp_in){
    char fileName[BUFFER_SIZE];
    int rails = 0, encode, error = 0;
    
    while (1) {
        //Read from parent
        fscanf(fp_in, "%s %d %d", fileName, &rails, &encode);
        //Child sen data Rail Fence Cipher process managger
        if (encode == 1 || encode == 2) {
            error = railFenceF(fileName, rails, encode);
        }else{
            break;
        }
        //Write to parent
        fprintf(fp_out, "%d\n",error);
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
        perror(">> ERROR: error opening the pipe parent to child");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipe_child_parent) == -1){
        perror(">> ERROR: error opening the pipe child to parent");
        exit(EXIT_FAILURE);
    }
    
    pid = fork();
    if (pid > 0) {
        preparePipes(pipe_parent_child, pipe_child_parent, &fp_out, &fp_in);
        parentMenu(fp_out, fp_in);
        closePipes(pipe_parent_child, pipe_child_parent, &fp_out, &fp_in);
        printf(">> Parent pipe closed\n");
    }else if(pid == 0){
        preparePipes(pipe_child_parent, pipe_parent_child, &fp_out, &fp_in);
        childProcess(fp_out, fp_in);
        closePipes(pipe_child_parent, pipe_parent_child, &fp_out, &fp_in);
        printf(">> Child pipe closed\n");
    }else{ // No process created
        printf(">> ERROR: unable to fork\n");
        exit(EXIT_FAILURE);
    }
}

/* Function tu close the unesessary channels and open the pipe files
 *********************************************************/
void preparePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in){
    close(pipe_in[1]); // Close the unwanted channels
    close(pipe_out[0]);
    // Open the file pointers
    *fp_out = fdopen(pipe_out[1], "w");
    if (*fp_out == NULL)
    {
        perror(">> ERROR: Unable to open pipe for writing\n");
        exit(EXIT_FAILURE);
    }
    *fp_in = fdopen(pipe_in[0], "r");
    if (*fp_in == NULL)
    {
        perror(">> ERROR: Unable to open pipe for reading\n");
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
