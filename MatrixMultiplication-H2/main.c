//
//  main.c
//  MatrixMultiplication-H2
//
//  Created by Antony Morales on 27/01/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//  Antony Adrian Morales Rosas

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Matrix{
    int rows;
    int columns;
    float * matrix;
};

void readFile(struct Matrix *matrix, char * fileName){
    FILE *matrixFile;
    float matrixnumber; //Variable to asign the temporary float of the file
    int i = 0;
    
    matrixFile = fopen(fileName, "r");
    if(matrixFile==NULL){ //Check if the file exist
        printf("no such file.");
        return;
    }
    //Get the rows and columns of the file
    fscanf(matrixFile, "%f", &matrixnumber);
    matrix->rows = matrixnumber;
    fscanf(matrixFile, "%f", &matrixnumber);
    matrix->columns = matrixnumber;
    
    matrix->matrix = malloc((matrix->rows) * (matrix->columns) * sizeof(float));
    
    while ((fscanf(matrixFile, "%f", &matrixnumber))==1) { //Save the matrix
        *(matrix->matrix + i) = matrixnumber;
        i++;
    }
    fclose(matrixFile);
}

void writeFile(struct Matrix *matrix){
    FILE* matrixFile = fopen("Matrix_C.txt", "w");
    
    fprintf(matrixFile,"%d %d\n", matrix->rows, matrix->columns);
    for(int i = 0; i <  matrix->rows * matrix->columns; i++){
        fprintf(matrixFile, "%f ", matrix->matrix[i]);
        if((i+1)%matrix->columns == 0){
            fprintf(matrixFile, "\n");
        }
    }
    
    fclose(matrixFile);
}

void dotProduct(struct Matrix *matrixA, struct Matrix *matrixB){
    struct Matrix resultMatrix;
    resultMatrix.matrix = NULL;
    int helper = 0, counter = 0;
    float matrixNumber = 0;
    
    resultMatrix.rows = matrixA->rows;
    resultMatrix.columns = matrixB->columns;
    resultMatrix.matrix = malloc((resultMatrix.rows) * (resultMatrix.columns) * sizeof(float));
    
    if(matrixA->columns == matrixB->rows){
        
        for(int i=0; i< matrixA->rows; i++){
            for(int j = 0; j<matrixB->columns; j++){
                for(int k=0; k<matrixA->columns; k++){
                    //printf("A:%f * ", *(matrixA->matrix + (k+helper)));
                    //printf("B:%f ** ", *(matrixB->matrix + (k*3) + j));
                    //printf(" (%f)", (*(matrixA->matrix + (k+helper)))*(*(matrixB->matrix + (k*3) + j)));
                    printf("i:%d, j:%d, k:%d\n", i ,j ,k);
                    matrixNumber += (*(matrixA->matrix + (k+helper)))*(*(matrixB->matrix + (k*3) + j));
                }
                printf("FINAL: %f\n", matrixNumber);
                *(resultMatrix.matrix + counter) = matrixNumber;
                matrixNumber=0;
                counter++;
            }
            helper += matrixB->rows;
        }
        
        writeFile(&resultMatrix);
        free(resultMatrix.matrix);
        
    }else{
        printf("The matrix can´t be multiplied\n");
    }
    
}

int main(int argc, const char * argv[]) {
    struct Matrix matrixA;
    struct Matrix matrixB;
    readFile(&matrixA, "Matrix_A.txt");
    readFile(&matrixB, "Matrix_B.txt");
    
    dotProduct(&matrixB, &matrixA);
    
    free(matrixA.matrix);
    free(matrixB.matrix);
    return 0;
}
