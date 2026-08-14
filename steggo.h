#ifndef STEGGO_H
#define STEGGO_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

#define RED    "\033[31m"
#define YELLOW "\033[33m"
#define GREEN  "\033[32m"
#define RESET  "\033[0m"

typedef unsigned int uint ;

typedef enum 
{
    type_unsupport = 0,
    type_encode,
    type_decode

}Operation_type;

typedef enum 
{
    FAILURE = 0,
    SUCCESS

}Status;

#define MAGIC "*#" 

Operation_type validate_operation(char *argv[]);

#endif
