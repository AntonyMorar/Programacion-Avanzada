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
#define BUFFER_SIZE 511

void railFence(char* fileName, int rails, int encode);
void encodeFile(char* message, int rails, char ***railMatrixP);
void decodeFile(char* message, int rails);

#endif /* rail_fence_h */
