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

// ---------------  Defines

// Define maximum values
#define MAX_EQU_LENGTH  64
#define MAX_NUMSTACK    64
#define MAX_OPSTACK     64
#define MAX_QUEUE       64

// Define the precedence and associativity of each operator
#define PREC_POW    4
#define PREC_MUL    3
#define PREC_DIV    3
#define PREC_ADD    2
#define PREC_MIN    2
#define PREC_PAR    0
#define ASSOC_POW   "right"
#define ASSOC_MUL   "left"
#define ASSOC_DIV   "left"
#define ASSOC_ADD   "left"
#define ASSOC_MIN   "left"
#define ASSOC_NONE  0

// --------------- Prototypes

void die(const char *message);
void push_numstack(long num);
int pop_numstack();
int parse_numbers(char *equation);
//TODO: Add finnished functions.

// --------------- Global Declarations

// the global opstack
typedef struct{
    char op;
    int prec;
    char *assoc;
}operator;

// the output queue
typedef struct{
    bool is_number;
    long number;
    char operator;
}queue;

// creating a global Numstack by defining an array of numbers.
long numstack[MAX_NUMSTACK];
long nnumstack = 0;

// creating an array of operator structs. -> Op Stack
operator opstack[MAX_OPSTACK];
int nopstack = 0;

// creating an array of queue structs. -> Queue
queue output[MAX_QUEUE];
int noutput = 0; 

// --------------- Functions

// Die with error message.
void die(const char *message)
{
    if(errno){
        perror(message);
    }else{
        printf("ERROR: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

// Check if a character is a mathematical operator.
bool is_operator(char c)
{
    return (c=='(') || (c==')') ||  (c=='+') || (c=='-') || (c=='*') || (c=='/') || (c=='^');
}

// Fill an operator struct with the current operator.
void fill_opstack(char c)
{
    switch(c)
    {
        case '(': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_PAR; opstack[nopstack].assoc=ASSOC_NONE; break;
        case ')': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_PAR; opstack[nopstack].assoc=ASSOC_NONE; break;
        case '+': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_ADD; opstack[nopstack].assoc=ASSOC_ADD; break;
        case '-': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_MIN; opstack[nopstack].assoc=ASSOC_MIN; break;
        case '*': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_MUL; opstack[nopstack].assoc=ASSOC_MUL; break;
        case '/': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_DIV; opstack[nopstack].assoc=ASSOC_DIV; break;
        case '^': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_POW; opstack[nopstack].assoc=ASSOC_POW; break;
    }
}

// Savely push a number to the numstack.
void push_numstack(long num)
{
    if(nnumstack > MAX_NUMSTACK - 1)
        die ("Number stack overflow!\n");

    numstack[nnumstack++] = num;
}

// Savely pop a number from the numstack.
int pop_numstack()
{
    if(!nnumstack)
        die("Number stack empty!\n");

    return numstack[--nnumstack];
}

// [DEBUG ONLY] print the local numstack
void print_numstack()
{
    printf("\nDEBUG: Printing numstack!\n");
    int i = 0;
    for(i = (nnumstack - 1); i >= 0; i--){
        printf("DEBUG: NUMSTACK[%d] = %ld\n", i, numstack[i]);
    }
}

// Savely push an operator to the opstack.
void push_opstack(char op)
{
    if(nopstack > MAX_OPSTACK - 1)
        die("Operator stack overflow!\n");

    fill_opstack(op);
    nopstack++;
}

// Savely pop an operator from the opstack.
operator *pop_opstack()
{
    if(!nopstack)
        die("Operator stack empty!");

    return &opstack[--nopstack];
}

// [DEBUG ONLY] print the local opstack
void print_opstack()
{
    printf("\nDEBUG: Printing opstack!\n");
    int i = 0;
    for(i = (nopstack - 1); i >= 0; i--){
        printf("DEBUG: OPSTACK[%d]:\n", i);
        printf("\tOperator = %c\n", opstack[i].op);
        printf("\tPrecedence = %d\n", opstack[i].prec);
        printf("\tAssociativity = %s\n\n", opstack[i].assoc);
    }
}

// Push a Number to the output queue
void push_number_to_output(long num)
{
    if(noutput > MAX_QUEUE - 1)
        die("Output stack overflow!\n");
    
    output[noutput].is_number = true;
    output[noutput].number = num;
    noutput++;
}

// Push an operator to the output queue
void push_operator_to_output(char op)
{
    if(noutput > MAX_QUEUE - 1)
        die("Output stack overflow!\n");

    output[noutput].is_number = false;
    output[noutput].operator = op;
    noutput++;
}

// Flush RPN expression!
void flush_rpn()
{
    //TODO Implement!
    // The following is only for debugging purposes!

    printf("\nDEBUG: Printing output queue!\n");
    int i = 0;
    for(i = (noutput - 1); i >= 0; i--){
        
        printf("DEBUG: QUEUE[%d]:\n", i);
        
        if(output[i].is_number)
            printf("\tNumber = %ld\n", output[i].number);
        else
            printf("\tOperator = %c\n", output[i].operator );
    }
}

// Read the whole equation and push all numbers to the numstack.
int parse_numbers(char *equation)
{
    // Copy an in a new instance of a string, because 
    // strtol consumes each char after conversion.
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

    free(str);

    return EXIT_SUCCESS;
}

// Read the whole equation and push all operators to the opstack.
int parse_operators(char *equation)
{
    // Unnecessary in this case but it looks cooler. ;)
    char *str = strdup(equation), *p = str;

    while(*p){ // While there are more characters to process.
        if(is_operator(*p)){
            printf("DEBUG: Pushing operator %c to opstack.\n", *p);
            push_opstack(*p);
            p++;
        } else {
            p++;
        }
    }

    free(str);

    return EXIT_SUCCESS;
}

const char *shunting_yard(const char *equation)
{
    char *e = strdup(equation), *p = e;
    bool fparensleft, fparensright = false; // Some flags

    while(*p){ // Loop trough the whole expression. 
        if(isdigit(*p)){ // If a number is detected
            push_number_to_output(strtol(p,&p, 10));
        }
        if(is_operator(*p)){ // If an operator is detected
            push_opstack(*p);
            p++;
            
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2)
        die("To few arguments! Give equation!");

    char *equation = argv[1];
    
    //parse_numbers(equation);
    //parse_operators(equation);

    //print_numstack();
    //print_opstack();

    shunting_yard(equation);
    flush_rpn();

    //printf("\nSUCCESS: %s = %f\n", equation, parse_equation(equation));

    return EXIT_SUCCESS; 
}
