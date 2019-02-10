//
//  ppmEditor.h
//  SimpleImageManipulation-H3
//
//  Created by Antony Morales on 10/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#ifndef ppmEditor_h
#define ppmEditor_h

#include <stdio.h>
struct Pixel{
    unsigned char r, g, b;
};

struct Image{
    char ppmType[2];
    int width, height, maxVal;
    struct Pixel **matrix;
};

void readImageFile(char *fileName, struct Image *inputImage);
void writeImageFile(char *fileNameResult, struct Image *inputImage);
void negativeImage(struct Image *inputImage);
void scaleImage(int scaleFactor, struct Image *inputImage);
void allocImage(struct Image *image);
void freeImage(struct Image *image);

#endif /* ppmEditor_h */
