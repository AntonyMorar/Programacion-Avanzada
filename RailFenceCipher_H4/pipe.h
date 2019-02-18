//
//  pipe.h
//  RailFenceCipher_H4
//
//  Created by Antony Morales on 17/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#ifndef pipe_h
#define pipe_h

#include <stdio.h>
#include <unistd.h> // Library to access POSIX functions
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define BUFFER_SIZE 511

/* Function declarations
 *********************************************************/
void createProces(void);
void preparePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in);
void closePipes(int pipe_out[], int pipe_in[], FILE ** fp_out, FILE ** fp_in);
void parentMenu(FILE * fp_out, FILE * fp_in);
void childProcess(FILE * fp_out, FILE * fp_in);
int railFenceF(char* fileName, int rails, int encode);

#endif /* pipe_h */
