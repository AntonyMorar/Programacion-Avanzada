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
#include <sys/wait.h>

#define BUFFER_SIZE 255

void readFile(char *fileName){
    
}

void writeFile(char *filename){
    
}

void encode(){
    
}

void decode(){
    
}

int main(int argc, const char * argv[]) {
    printf("********************************************\n********* Rail Fence Cipher Program ********\n********************************************\n");
    char selection;
    char *fileName;
    pid_t pid;
    
    while (1) {
        printf("\n============================\nMENU\n============================\n");
        printf("1. Encode\n2. Decode\n3. Exit\n>>> ");
        scanf("%c", &selection);
        
        switch (selection) {
            case '1':
                break;
            case '2':
                break;
            case '3':
                return 0;
                break;
            default:
                break;
        }
        
        printf("File name:\n>>> ");
        scanf("%s", &fileName);
        printf("Number of rails:\n>>> ");
    }

    return 0;
}
