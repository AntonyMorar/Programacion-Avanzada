//
//  ppmEditor.c
//  SimpleImageManipulation-H3
//
//  Created by Antony Morales on 10/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include "ppmEditor.h"
#include <stdlib.h>
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
    fgets(inputImage->ppmType, 3, imageFile);
    fscanf(imageFile, "%i",&inputImage->width);
    fscanf(imageFile, "%i",&inputImage->height);
    fscanf(imageFile, "%i",&inputImage->maxVal);
    fscanf(imageFile, "%i",&imageValue);
    allocImage(inputImage);
    
    for (int i= 0; i<inputImage->height; i++) {
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
/* Scale the image to any scale factor
 *********************************************************/
void scaleImage(int scaleFactor, struct Image *inputImage){
    float scale = (float)scaleFactor/100;
    if(scale == 1) return; //Check if image needs to scale
    struct Image newImage;
    void *pointerHelper;
    
    strcpy(newImage.ppmType, inputImage->ppmType);
    newImage.height = inputImage->height * scale;
    newImage.width = inputImage->width * scale;
    newImage.maxVal = inputImage->maxVal;
    
    if(newImage.height == 0 || newImage.width == 0) printf("***** ERROR: The scale is too small\n\t THE IMAGE WASN´T SCALED *****\n");
    
    allocImage(&newImage);
    
    if(scale > 0){
        for (int i= 0; i<newImage.height; i++) {
            for (int j = 0; j<newImage.width; j++) {
                newImage.matrix[i][j] = inputImage->matrix[(int)(i/scale)][(int)(j/scale)];
            }
        }
    }else{
        printf("***** ERROR: Program can´t have a negative scale factor, to reduce th image,apply scale between 0 and 100\n\tTHE IMAGE WASN´T SCALED *****\n");
    }
    
    printf("    Size: %d * %d\n", inputImage->height, inputImage->width);
    printf("    Pointer: %p\n", inputImage);
    pointerHelper = inputImage;
    *inputImage = newImage; //Image now have the direction of the new image, image is equal to new image
    freeImage(&newImage); //Free the memory of newImage
    printf("New Pointer: %p\n", inputImage);
    printf("New Size: %d * %d\n", inputImage->height, inputImage->width);
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
