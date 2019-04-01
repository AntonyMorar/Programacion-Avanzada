//  Antony Morales
//  bank_codes.h
//  SimpleBank_H8
//
//  Created by Antony Morales on 29/03/19.
//  Copyright © 2019 Antony999k. All rights reserved.
//

// The different types of operations available
typedef enum valid_operations {CHECK, DEPOSIT, WITHDRAW, TRANSFER, EXIT} operation_t;

// The types of responses available
typedef enum valid_responses {OK, INSUFFICIENT, NO_ACCOUNT, BYE, ERROR} response_t;
