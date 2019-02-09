//
//  main.c
//  SimpleImageManipulation-H3
//
//  Created by Antony Morales on 05/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "header.h"

void readImageFile(char *fileName, struct Image *inputImage2){
    FILE *imageFile;
    int imageValue = 0;
    struct Image inputImage;
    
    imageFile = fopen(fileName, "r");
    if(imageFile==NULL){ //Check if the file exist
        printf("no such file.");
        return;
    }
    
    fgets(inputImage.ppmType, 3, imageFile);
    printf("Type: %s\n", inputImage.ppmType);
    
    fscanf(imageFile, "%i",&inputImage.width);
    printf("Width: %i\n", inputImage.width);
    fscanf(imageFile, "%i",&inputImage.height);
    printf("Height: %i\n", inputImage.height);
    fscanf(imageFile, "%i",&inputImage.maxVal);
    printf("Maxval: %i\n", inputImage.maxVal);
    fscanf(imageFile, "%i",&imageValue);
    
    
    //unsigned char aa = (char)87;
    inputImage.matrix = malloc(inputImage.width * sizeof(struct Pixel*));
    for (int i = 0; i < inputImage.height; i++) {
        inputImage.matrix[i] = malloc(inputImage.height * sizeof(struct Pixel*));
    }
    inputImage.matrix[0][0].r = (unsigned char)90;
    
    for (int i= 0; i<5; i++) {
        for (int j = 0; j<5; j++) {
            fread(&inputImage.matrix[i][j].r, 1, 1, imageFile);
            printf("R[%i][%i]:%i ",i,j, inputImage.matrix[i][j].r);
            fread(&inputImage.matrix[i][j].g, 1, 1, imageFile);
            printf("G[%i][%i]: %i ",i,j, inputImage.matrix[i][j].g);
            fread(&inputImage.matrix[i][j].b, 1, 1, imageFile);
            printf("B[%i][%i]: %i ",i,j, inputImage.matrix[i][j].b);
        }
        printf("\n");
    }
    fclose(imageFile);
}

void writeImageFile(char *fileNameResult){
    
}

void negativeImage(){
    
}

void scaleImage(int scaleFactor){
    
}



int main(int argc, char **argv) {
    int c, scaleFactor;
    char *fileName = NULL, *fileNameResult = NULL;
    bool negative = false;
    
    while ((c = getopt (argc, argv, "i:o:n:s:")) != -1)
        switch (c){
            case 'i': //Indicate the file to read
                fileName = optarg;
                break;
            case 'o'://Indicate the name of the image to create with the result
                fileNameResult = optarg;
                break;
            case 'n'://Use if you want to generate the negative of the image
                negative = true;
                break;
            case 's'://Use to scale the image up or down
                scaleFactor = atoi(optarg);
                break;
            case '?':
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    
    printf("************* Image Manipulation Program ****************\n");
    struct Image inputImage;
    
    readImageFile(fileName, &inputImage);
    writeImageFile(fileNameResult);
    return 0;
}
