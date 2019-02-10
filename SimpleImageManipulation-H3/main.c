//
//  main.c
//  SimpleImageManipulation-H3
//
//  Created by Antony Morales on 05/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "ppmEditor.h"

int main(int argc, char **argv) {
    int c, scaleFactor = 100;
    char *fileName = NULL, *fileNameResult = NULL;
    bool negative = false;
    
    while ((c = getopt (argc, argv, "i:o:s:n")) != -1)
        switch (c){
            case 'i': //Indicate the file to read
                fileName = optarg;
                break;
            case 'o'://Indicate the name of the image to create with the result
                fileNameResult = optarg;
                break;
            case 's'://Use to scale the image up or down
                scaleFactor = atoi(optarg);
                break;
            case 'n'://Use if you want to generate the negative of the image
                negative = true;
                break;
            default:
                abort ();
        }
    
    if(fileName == NULL && fileNameResult == NULL){
        printf ("Arguments -i and -o are required.\n");
        return 1;
    }
    
    printf("************* Image Manipulation Program ****************\n");
    struct Image inputImage;
    //void * aaa = (void *)0x7ffeefbff430;
    
    readImageFile(fileName, &inputImage);
    if(negative == true) negativeImage(&inputImage);
    struct Image *finalImg = scaleImage(scaleFactor, &inputImage);
    writeImageFile(fileNameResult, finalImg);
    freeImage(&inputImage);
    freeImage(finalImg);
    
    return 0;
}
