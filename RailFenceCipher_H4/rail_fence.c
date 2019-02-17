//
//  rail_fence.c
//  RailFenceCipher_H4
//
//  Created by Antony Morales on 17/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include "rail_fence.h"

void railFence(char* fileName, int rails, int encode){
    char buffer[BUFFER_SIZE];
    char **railMatrix = NULL;
    
    FILE *readFile = fopen(fileName, "r");
    if(readFile==NULL){ //Check if the file exist
        printf("No such read file.\n");
        return;
    }
    
    //Name the outputfile
    char *outputFileName = NULL;
     outputFileName = malloc((strlen(fileName) + 8) * sizeof(char));
    if(encode == 1){
        outputFileName = strcpy(outputFileName, "encoded_");
    }else{
        outputFileName = strcpy(outputFileName, "decoded_");
    }
    strcat(outputFileName, fileName);
    
    //Open and write in output file
    FILE *writeFile = fopen(outputFileName, "w");
    if(writeFile==NULL){ //Check if the file exist
        printf("No such write file.\n");
        return;
    }
    
    while(fgets(buffer, BUFFER_SIZE, readFile)){
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
        railMatrix = malloc(strlen(buffer) * sizeof(char*));
        for (int i = 0; i< strlen(buffer); i++) {
            railMatrix[i] = calloc(strlen(buffer),sizeof(char));
        }
        encodeFile(buffer, rails, &railMatrix);
    }
    
    fclose(readFile);
    fclose(writeFile);
    free(outputFileName);
}

void encodeFile(char* message, int rails, char ***railMatrixP){
    int i,j,count;
    unsigned long messageLength = strlen(message);
    printf("\nENCODING text: %s\n", message);
    
    char **railMatrix = NULL;
    railMatrix = malloc(messageLength * sizeof(char*));
    for (i = 0; i< messageLength; i++) {
        railMatrix[i] = calloc(messageLength,sizeof(char));
    }
    
    for (i = 0; i<rails; i++) {
        for (int j = 0; j<messageLength; j++) {
            railMatrix[i][j]=0;
        }
    }
    
    count=0;
    j=0;
    while(j<messageLength){
        if(count%2==0){
            for(i=0;i<rails;i++){
                railMatrix[i][j]=(int)message[j];
                j++;
            }
        }
        else{
            for(i=rails-2;i>0;i--){
                railMatrix[i][j]=(int)message[j];
                j++;
            }
        }
        count++;
    }
    
    printf("Final text: ");
    for(i=0;i<rails;i++){
        for(j=0;j<messageLength;j++){
            if(railMatrix[i][j]!=0)
                printf("%c",railMatrix[i][j]);
        }
    }
    
}

void decodeFile(char* message, int rails){
    printf("\nENCODING FILE\n");
}
