//
//  main.c
//  randomNumber
//
//  Created by Antony Morales on 15/01/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

int main(int argc, const char * argv[]) {
    srand ( time(NULL) ); //Seed that makes diferent random numbers
    int number = rand() % 500;
    int userNumber;
    bool userWin = false;
    
    printf("Random number generated from 0 to 500!! \n", number);
    
    while(!userWin){
        printf("Write a number: ");
        scanf("%d", &userNumber);
        
        if(userNumber < number){
            printf("-----> The number is bigger\n\n");
        }else if(userNumber > number){
            printf("-----> The number is smaller\n\n");
        }else{
            printf("************* YOU WIN *************\n");
            userWin = true;
        }
    }
}

void clearBufferEnter(){
    char garbage;
    while((garbage=getchar()) != '/n' && garbage != EOF);
}
