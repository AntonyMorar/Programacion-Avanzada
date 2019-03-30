//  main.c
//  NumericalBaseConversion-H1
//
//  Created by Antony Adrian Morales Rosas on 20/01/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//
// All the functions recive and return some parameters
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#define SPACE 50

char * decimalToNBase(int number, int targetBase);
int nBaseToDecimal(char * number, int initialBase);
char numberToChar(int number);
int charToNumber(char number);
char *strrev(char *str);

int main(int argc, char **argv) {
    char *ivalue = NULL, *ovalue= NULL;
    int index;
    int c;
    
    printf("************* Numeric base conversion program ****************\n");
    //With getopt the flags of input -i and output -o are declared
    while ((c = getopt (argc, argv, "i:o:")) != -1)
        switch (c){
            case 'i':
                ivalue = optarg;
                break;
            case 'o':
                ovalue = optarg;
                break;
            case '?':
                if (optopt == 'i')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%i'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    
    //Loop review the remaining arguments that are the numbers to be converted
    for (index = optind; index < argc; index++){
        if((atoi(ivalue) >= 2 && atoi(ivalue) <= 35) && (atoi(ivalue) >= 2 && atoi(ivalue) <= 35)){
            printf("Converting %s from base %s to base %s\n", argv[index], ivalue, ovalue);
            printf("Converted to base %s: ", ovalue);
            if(atoi(ovalue) == 10){
                printf("%d\n\n", nBaseToDecimal(argv[index], atoi(ivalue)));
            }else if(atoi(ivalue) == 10){
                printf("%s\n\n", decimalToNBase(atoi(argv[index]), atoi(ovalue)));
            }else if(atoi(ivalue) != 10){
                printf("%s\n\n" ,decimalToNBase(nBaseToDecimal(argv[index], atoi(ivalue)), atoi(ovalue)));
            }
        }else{
            printf("ERROR: The input bases doesn´t exist (use bases from 2 to 35)\n");
            exit(0);
        }
    }
    return 0;
}

/* Converts any base to decimal base
 *********************************************************/
int nBaseToDecimal(char * number, int initialBase){
    int length = (int)strlen(number), decimalNumber = 0, powCount=0;
    char charNumber;
    
    if(initialBase == 2){//Determine if the initial base is binary
        for(int i=length-1; i>=0; i--){
            charNumber = number[i];
            if(charNumber == '1'){
                decimalNumber += pow(initialBase,powCount);
            }
            powCount++;
        }
    }else{
        for(int i = length-1; i>=0; i--){
            charNumber = number[i];
            decimalNumber += (charToNumber(charNumber)) * pow(initialBase ,powCount);
            powCount++;
        }
    }
    return decimalNumber;
}

/* Converts decimal to any base
 *********************************************************/
char * decimalToNBase(int number, int targetBase){
    char *nBaseNumber = NULL; //Init the final base string
    int counter = 0;
    int base = targetBase;
    
    nBaseNumber = malloc(SPACE);
    
    while(number > 0){
        nBaseNumber[counter] = numberToChar(number % base);
        number /= base;
        counter++;
    }
    return strrev(nBaseNumber);
}

/* Converts all the char values in decimal numbers, works with all letters (best implementation of hexToTenDictionary)
 *********************************************************/
int charToNumber(char number){
    if (number >= '0' && number <= '9'){
        return (int)number - '0';
    }else if(number >= 'A' && number <= 'Z'){ //Asccii letters A to Z is equal to 65 to 90
        return (int)number - 'A' + 10;
    }else if(number >= 'a' && number <= 'z'){ //Asccii letters A to Z is equal to 97 to 122
        return (int)number - 'a' + 10;
    }
    return -1;
}

/* Converts decimal numbers the char values in (best implementation of tenToHexDictionary)
 *********************************************************/
char numberToChar(int number){
    if(number >= 0 && number <= 9){
        return (char)number + '0';
    }else{ //Only tranform to Uppercase letter because only use to print it
        return (char)(number - 10 + 'A');
    }
}

//Function to reverse a string Writted by Ignacio Vazquez-Abrams in stackoverflow.com
char *strrev(char *str){
    char *p1, *p2;
    if (! str || ! *str)
        return str;
    for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2){
        *p1 ^= *p2;
        *p2 ^= *p1;
        *p1 ^= *p2;
    }
    return str;
}
