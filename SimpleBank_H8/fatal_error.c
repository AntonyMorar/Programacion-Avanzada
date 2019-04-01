//  Antony Morales
//  fatal_error.c
//  SimpleBank_H8
//
//  Created by Antony Morales on 29/03/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include "fatal_error.h"

void fatalError(const char * message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

