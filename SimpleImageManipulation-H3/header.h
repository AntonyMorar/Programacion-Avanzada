//
//  header.h
//  SimpleImageManipulation-H3
//
//  Created by Antony Morales on 05/02/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#ifndef header_h
#define header_h

struct Pixel{
    unsigned char r, g, b;
};

struct Image{
    char ppmType[2];
    int width, height, maxVal;
    struct Pixel **matrix;
};

#endif /* header_h */
