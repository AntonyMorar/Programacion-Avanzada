//
//  rail_fence.h
//  RailFenceCipher_H4
//
//  Created by Antony Morales on 17/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#ifndef rail_fence_h
#define rail_fence_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define BUFFER_SIZE 511

typedef struct MatrixRails{
    char **matrix;
    int rails;
}MatrixRails;

int railFenceF(char* fileName, int rails, int encode);
void encodeFile(char* message, MatrixRails *matrixRails);
void decodeFile(char* message, MatrixRails *matrixRails);
void allocMatrixRail(MatrixRails *matrixRails, unsigned long space);
void freeMatrixRail(MatrixRails *matrixRails, unsigned long space);

#endif /* rail_fence_h */
