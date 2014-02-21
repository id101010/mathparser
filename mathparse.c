/*
 * mathparse.c
 *
 * Parsing an equation int the INFIX Form to a rpn expression.  
 * Bsp.: (100/2)/((16/2/4) -> 100 2 / 5 2 / 8 /
 * 
 * The algorithm works by using two stacks, a numstack and an operator stack.
 * 
 */
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<errno.h>
#include<ctype.h>
#include<stdbool.h>

// Defines
//#define EXIT_FAILURE    1
//#define EXIT_SUCCESS    0
#define MAX_EQU_LENGTH  255
#define MAX_NUMSTACK    64
#define MAX_OPSTACK     64

// --------------- Prototypes
void die(const char *message);
void push_numstack(long num);
void push_opstack();
void pop_opstack();
int pop_numstack();
int parse_numbers(char *equation);

// --------------- Global Declarations
typedef struct{
    char op;
    int prec;
    int assoc;
}operator;

// creating a global Numstack by defining an array of numbers.
long numstack[MAX_NUMSTACK];
long nnumstack = 0;

// creating a global Opstack by defining an array of operator structs.
operator opstack[MAX_OPSTACK];
int nopstack = 0;


// --------------- Functions

void die(const char *message)
{
    if(errno){
        perror(message);
    }else{
        printf("ERROR: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

void push_numstack(long num)
{
    if(nnumstack > MAX_NUMSTACK - 1){
        die ("Number stack overflow!\n");
    }

    numstack[nnumstack++] = num;
}

int pop_numstack()
{
    if(!nnumstack){
        die("Number stack empty!\n");
    }

    return numstack[--nnumstack];
}

void push_opstack()
{
}

void pop_opstack()
{
}

int parse_numbers(char *equation)
{
    char *str = strdup(equation), *p = str;
    
    while(*p){ // While there are more characters to process.
        if(isdigit(*p)){
            long number = strtol(p, &p, 10);
            printf("DEBUG: Pushing number: %ld to numstack.\n", number);
            push_numstack(number);
        } else {
            p++;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
        die("To few arguments! Give equation!");

    char *equation = argv[1];
    
    parse_numbers(equation);

    //printf("\nSUCCESS: %s = %f\n", equation, parse_equation(equation));

    return EXIT_SUCCESS; 
}
