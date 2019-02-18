//
//  rail_fence.c
//  RailFenceCipher_H4
//
//  Created by Antony Morales on 17/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include "rail_fence.h"

/* Manages the rail fence system, writes files and sends a call to encrypt and decrypt
 *********************************************************/
int railFenceF(char* fileName, int rails, int encode){
    char buffer[BUFFER_SIZE];
    MatrixRails matrixRails;
    matrixRails.rails = rails;
    
    FILE *readFile = fopen(fileName, "r");
    if(readFile==NULL){ //Check if the file exist
        printf(">> ERROR: No such read file.\n");
        return 1;
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
        printf(">> ERROR: No such write file.\n");
        return 1;
    }
    
    while(fgets(buffer, BUFFER_SIZE, readFile)){
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
        allocMatrixRail(&matrixRails, strlen(buffer)); //Alloc memory for matrix struct
        if(encode == 1){ //Decode or encode the message and print in file
            encodeFile(buffer, &matrixRails);
            for(int i=0;i<matrixRails.rails;i++){//Save in the file
                for(int j=0;j<strlen(buffer);j++){
                    if(matrixRails.matrix[i][j]!=0){
                        fprintf(writeFile,"%c", matrixRails.matrix[i][j]);
                    }
                }
            }
        }else{
            decodeFile(buffer, &matrixRails);
            int row=0,col=0, temp1=-1;
            for(int i = 0; i < strlen(buffer); ++i){

                fprintf(writeFile,"%c", matrixRails.matrix[row][col]);
                col++;
                if(row == 0 || row == (matrixRails.rails)-1){
                    temp1*= -1;
                }
                row = row + temp1;
            }
        }
        fprintf(writeFile,"\n");
        freeMatrixRail(&matrixRails, strlen(buffer)); //Free memory for matrix struct
    }
    fclose(readFile);
    fclose(writeFile);
    free(outputFileName);
    return 0;
}

/* Encode de text
 *********************************************************/
void encodeFile(char* message, MatrixRails *matrixRails){
    int i,j,count=0;
    unsigned long messageLength = strlen(message);

    for (i = 0; i<matrixRails->rails; i++) {
        for (j = 0; j<messageLength; j++) {
            matrixRails->matrix[i][j]=0;
        }
    }
    j=0;
    while(j<messageLength){
        if(count%2==0){
            for(i=0;i<matrixRails->rails;i++){
                matrixRails->matrix[i][j]=(int)message[j];
                j++;
            }
        }
        else{
            for(i=matrixRails->rails-2;i>0;i--){
                matrixRails->matrix[i][j]=(int)message[j];
                j++;
            }
        }
        count++;
    }
}
/* Decode the text de text
 *********************************************************/
void decodeFile(char* message, MatrixRails *matrixRails){
    unsigned long messageLength = strlen(message);
    int i, j, row=0, col=0, temp1 = -1, temp2 = 0;
    char railMatrix[matrixRails->rails][messageLength];
    
    for (i = 0; i<matrixRails->rails; i++) {
        for (j = 0; j<messageLength; j++) {
            railMatrix[i][j] = 0;
            matrixRails->matrix[i][j]=0;
        }
    }
    
    for(i = 0; i < messageLength; ++i){
        railMatrix[row][col++] = '*';
        if(row == 0 || row == (matrixRails->rails)-1){
             temp1*= -1;
        }
        row = row + temp1;
    }
    
    for(i = 0; i < matrixRails->rails; ++i){
        for(j = 0; j < messageLength; ++j){
            if(railMatrix[i][j] == '*'){
                railMatrix[i][j] = message[temp2];
                matrixRails->matrix[i][j] = message[temp2];
                temp2++;
            }
        }
    }
}

/* Alloc memoery for the rai matrix
 *********************************************************/
void allocMatrixRail(MatrixRails *matrixRails, unsigned long space){
    matrixRails->matrix = malloc(space * sizeof(char*));
    for (int i = 0; i< space; i++) {
        matrixRails->matrix[i] = calloc(space,sizeof(char));
    }
}

/* Free memoery for the rai matrix
 *********************************************************/
void freeMatrixRail(MatrixRails *matrixRails, unsigned long space){
    for (int i = 0; i< space; i++) {
        free(matrixRails->matrix[i]);
    }
    free(matrixRails->matrix);
}
