//
//  ppmEditor.c
//  SimpleImageManipulation-H3
//
//  Created by Antony Morales on 10/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include "ppmEditor.h"
#include <stdlib.h>
#include <string.h>
/* Read the image file and save in the Image struct
 *********************************************************/
void readImageFile(char *fileName, struct Image *inputImage){
    FILE *imageFile;
    int imageValue = 0;
    
    imageFile = fopen(fileName, "r");
    if(imageFile==NULL){ //Check if the file exist
        printf("no such file.\n");
        return;
    }
    fgets(inputImage->ppmType, 3, imageFile); //Scan de headers in the file
    fscanf(imageFile, "%i",&inputImage->width);
    fscanf(imageFile, "%i",&inputImage->height);
    fscanf(imageFile, "%i",&inputImage->maxVal);
    fscanf(imageFile, "%i",&imageValue);
    allocImage(inputImage);
    
    for (int i= 0; i<inputImage->height; i++) { //Scan the pixels matrix in the file and save in the matrix struct
        for (int j = 0; j<inputImage->width; j++) {
            fread(&inputImage->matrix[i][j].r, 1, 1, imageFile);
            fread(&inputImage->matrix[i][j].g, 1, 1, imageFile);
            fread(&inputImage->matrix[i][j].b, 1, 1, imageFile);
        }
    }
    fclose(imageFile);
}
/* Write the new image file
 *********************************************************/
void writeImageFile(char *fileNameResult, struct Image *inputImage){
    FILE *outputImageFile;
    
    outputImageFile = fopen(fileNameResult, "w");
    if(outputImageFile==NULL){ //Check if the file exist
        printf("no such file\n.");
        return;
    }
    fprintf(outputImageFile, "%s\n", inputImage->ppmType);
    fprintf(outputImageFile, "%d %d\n", inputImage->width, inputImage->height);
    fprintf(outputImageFile, "%d\n", inputImage->maxVal);
    
    for (int i= 0; i<inputImage->height; i++) {
        for (int j = 0; j<inputImage->width; j++) {
            //printf("g: %i\n", &inputImage->matrix[i][j].g);
            fwrite(&inputImage->matrix[i][j].r, 1, 1, outputImageFile);
            fwrite(&inputImage->matrix[i][j].g, 1, 1, outputImageFile);
            fwrite(&inputImage->matrix[i][j].b, 1, 1, outputImageFile);
        }
    }
    fclose(outputImageFile);
    printf("> New image created as %s\n\n", fileNameResult);
}
/* Invert the colors of the image
 *********************************************************/
void negativeImage(struct Image *inputImage){
    for (int i= 0; i<inputImage->height; i++) {
        for (int j = 0; j<inputImage->width; j++) {
            inputImage->matrix[i][j].r = ((int)inputImage->matrix[i][j].r * -1)+inputImage->maxVal;
            inputImage->matrix[i][j].g = ((int)inputImage->matrix[i][j].g * -1)+inputImage->maxVal;
            inputImage->matrix[i][j].b = ((int)inputImage->matrix[i][j].b * -1)+inputImage->maxVal;
        }
    }
}
/* Scale the image to any scale factor, return pointer to free memory in main
 *********************************************************/
struct Image* scaleImage(int scaleFactor, struct Image *inputImage){
    float scale = (float)scaleFactor/100;
    struct Image *newImage;
    newImage = malloc(sizeof(struct Image));
    
    strncpy(newImage->ppmType, inputImage->ppmType, 2);
    newImage->height = inputImage->height * scale;
    newImage->width = inputImage->width * scale;
    newImage->maxVal = inputImage->maxVal;
    
    if(newImage->height == 0 || newImage->width == 0) printf("***** ERROR: The scale is too small\n\t THE IMAGE WASN´T SCALED *****\n");

    allocImage(newImage);
    
    if(scale > 0){
        for (int i= 0; i<newImage->height; i++) {
            for (int j = 0; j<newImage->width; j++) {
                newImage->matrix[i][j] = inputImage->matrix[(int)(i/scale)][(int)(j/scale)];
            }
        }
    }else{
        printf("***** ERROR: Program can´t have a negative scale factor, to reduce th image,apply scale between 0 and 100\n\tTHE IMAGE WASN´T SCALED *****\n");
    }
    return newImage; //return new image with new size
}
/* Reserve memomry for an image struct and his Pixel matrix
 *********************************************************/
void allocImage(struct Image *image){
    image->matrix = malloc(image->width * sizeof(struct Pixel*));
    for (int i = 0; i < image->height; i++) {
        image->matrix[i] = calloc(image->height, sizeof(struct Pixel*));
    }
}
/* Free memomry for an image struct and his Pixel matrix
 *********************************************************/
void freeImage(struct Image *image){
    for (int i = 0; i < image->height; i++) {//Free inputImage dynamic memory
        free(image->matrix[i]);
    }
    free(image->matrix);
}
