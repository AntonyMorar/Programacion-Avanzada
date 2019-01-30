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
#include <unistd.h>

struct Matrix{
    int rows;
    int columns;
    float * matrix;
};
void readFile(struct Matrix *matrix, char * fileName);
void writeFile(struct Matrix *matrix, char* name);
void dotProduct(struct Matrix *matrixA, struct Matrix *matrixB, char *name);

int main(int argc, char **argv){
    int c;
    char *matrixAName = NULL, *matrixBName= NULL, *matrixCName= NULL;
    struct Matrix matrixA;
    struct Matrix matrixB;
    
    //With getopt the flags of matrix1 -1 and matrix2 -o are declared
    while ((c = getopt (argc, argv, "1:2:r:")) != -1)
        switch (c){
            case '1':
                matrixAName = optarg;
                break;
            case '2':
                matrixBName = optarg;
                break;
            case 'r':
                matrixCName = optarg;
                break;
            case '?':
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    printf("************* Numeric base conversion program ****************\n");
    readFile(&matrixA, matrixAName);
    readFile(&matrixB, matrixBName);
    dotProduct(&matrixB, &matrixA, matrixCName);
    return 0;
}

void dotProduct(struct Matrix *matrixA, struct Matrix *matrixB, char *name){
    struct Matrix resultMatrix; //Init a new matrix that will be the result of the dot product or matrix multiplication
    resultMatrix.matrix = NULL;
    int helper = 0, counter = 0;
    float matrixNumber = 0;
    
    //Assign rows, columns and space to new matrix depending on the rows and columns of the 2 multiplication matrix
    resultMatrix.rows = matrixA->rows;
    resultMatrix.columns = matrixB->columns;
    resultMatrix.matrix = malloc((resultMatrix.rows) * (resultMatrix.columns) * sizeof(float));
    
    if(matrixA->columns == matrixB->rows){//Check if the matrix could be multiplied
        for(int i=0; i< matrixA->rows; i++){//Start a loop to multiply and sum the new values of the matrix
            for(int j = 0; j<matrixB->columns; j++){
                for(int k=0; k<matrixA->columns; k++){
                    matrixNumber += (*(matrixA->matrix + (k+helper)))*(*(matrixB->matrix + (k*3) + j));
                }
                *(resultMatrix.matrix + counter) = matrixNumber;
                matrixNumber=0;
                counter++;
            }
            helper += matrixB->rows;
        }
        writeFile(&resultMatrix, name); //Write the final matrix in a file
        //Free the sapce of the 3 matrix in the program
        free(resultMatrix.matrix);
        free(matrixA->matrix);
        free(matrixB->matrix);
        printf("Product-Point made and saved %s file :)\n", name);
    }else{
        printf("The matrix can´t be multiplied\n");
    }
}

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
//Write the matrix in a new file on relative directory
void writeFile(struct Matrix *matrix, char* name){
    FILE* matrixFile = fopen(name, "w");
    fprintf(matrixFile,"%d %d\n", matrix->rows, matrix->columns);
    for(int i = 0; i <  matrix->rows * matrix->columns; i++){
        fprintf(matrixFile, "%f ", matrix->matrix[i]);
        if((i+1)%matrix->columns == 0){
            fprintf(matrixFile, "\n");
        }
    }
    fclose(matrixFile);
}
