/*
 * [mathparse.c]
 *
 * ====================================================================
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ====================================================================
 *
 * Abstract:
 *
 * An program which is able to parse an equation in the INFIX Form
 * and convert it to an easy to solve rpn expression.  
 * 
 * Bsp.: "(100/2)/((16/2/4)" gets converted to "100 2 / 5 2 / 8 /"
 * 
 * TODO:
 * - perform Error checks
 * - handle negative numbers
 *
 * Contact:
 * aaron@duckpond.ch (feel free to use my gpg key)
 */
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<errno.h>
#include<ctype.h>
#include<stdbool.h>
#include<string.h>

// ---------------  Defines & Macros

// Define maximum values
#define MAX_OPSTACK     128
#define MAX_QUEUE       128
#define MAX_RPN_STACK   128

// Define the precedence of each operator
#define PREC_POW    4
#define PREC_MUL    3
#define PREC_DIV    3
#define PREC_ADD    2
#define PREC_MIN    2
#define PREC_PAR    0
#define ASSOC_NONE  0

// Debug flag
#define DEBUG

// Macro which is needed by the eval_rpn function.
#define RPN_CALC(x) b = pop_rpnstack(), a = pop_rpnstack(), push_rpnstack(x)

// --------------- Prototypes

void die(const char *message);
void fill_opstack(char c);
void push_rpnstack(double v);
void push_opstack(char op);
void push_number_to_output(long num);
void push_operator_to_output(char op);
void read_rpn_expression();
double eval_rpn();
double power(double x, double y);
double pop_rpnstack();
bool isoperator(char c);
int get_op_prec(char c);
const char *shunting_yard(const char *equation);

// --------------- Global Declarations

// the global opstack
typedef struct{
    char op;
    int prec;
}operator;

// the output queue
typedef struct{
    bool is_number;
    long number;
    char operator;
}queue;

// creating an array of operator structs. -> Op Stack
operator opstack[MAX_OPSTACK];
int nopstack = 0;

// creating an array of queue structs. -> Queue
queue output[MAX_QUEUE];
int noutput = 0;

// creating the rpn stack which is used to calculate the rpn expression after converting.
double rpnstack[MAX_RPN_STACK];
int nrpnstack = 0;

// --------------- Functions

// Die with error message.
void die(const char *message)
{
    if(errno){
        perror(message);
    }else{
        printf("[ERROR]: %s\n", message);
    }

    exit(EXIT_FAILURE);
}

// Calculate y to the power of x. 
double power(double x, double y)
{
    return (y < 0) ? 0 : ((y == 0) ? 1 : x * power(x, y - 1));
}

// Check if a character is a mathematical operator.
bool isoperator(char c)
{
    return (c=='+') || (c=='-') || (c=='*') || (c=='/') || (c=='^');
}

// Fill an operator struct with the current operator.
void fill_opstack(char c)
{
    switch(c)
    {
        case '(': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_PAR; break;
        case ')': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_PAR; break;
        case '+': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_ADD; break;
        case '-': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_MIN; break;
        case '*': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_MUL; break;
        case '/': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_DIV; break;
        case '^': opstack[nopstack].op=c; opstack[nopstack].prec=PREC_POW; break;
    }
}

// Get the precedence of an operator
int get_op_prec(char c)
{
    int prec = 0;

    switch(c){
        case '(': prec = PREC_PAR; break;
        case ')': prec = PREC_PAR; break;
        case '+': prec = PREC_ADD; break;
        case '-': prec = PREC_MIN; break;
        case '*': prec = PREC_MUL; break;
        case '/': prec = PREC_DIV; break;
        case '^': prec = PREC_POW; break;
    }

    return prec;
}

// Savely push to the rpn stack.
void push_rpnstack(double v)
{
    if(nrpnstack > MAX_RPN_STACK - 1)
        die("RPN stack overflow!\n");
#ifdef DEBUG
    printf("[DEBUG]: [PUSH]\tnumber\t%g\tto rpn stack.\n", v);
#endif 
    rpnstack[nrpnstack++] = v;
}

// Savely pop from the rpn stack.
double pop_rpnstack()
{
    if(!nrpnstack)
        die("RPN stack is underflow!\n");
#ifdef DEBUG
    printf("[DEBUG]: [POP]\tnumber\t%g\tfrom rpn stack.\n", rpnstack[nrpnstack-1]);
#endif    
    return rpnstack[--nrpnstack];
}

// Savely push an operator to the opstack.
void push_opstack(char op)
{
    if(nopstack > MAX_OPSTACK - 1)
        die("Operator stack overflow!\n");
#ifdef DEBUG
    printf("[DEBUG]: [PUSH]\toperator\t%c\tto opstack.\n", op);
#endif
    fill_opstack(op);
    nopstack++;
}

// Savely pop an operator from the opstack.
operator pop_opstack()
{
    if(!nopstack)
        die("Operator stack underflow!");
#ifdef DEBUG
    printf("[DEBUG]: [POP]\toperator\t%c\tfrom opstack.\n", opstack[nopstack].op);
#endif
    return opstack[--nopstack];
}

// Push a Number to the output queue.
void push_number_to_output(long num)
{
    if(noutput > MAX_QUEUE - 1)
        die("Output stack overflow!\n");
#ifdef DEBUG
    printf("[DEBUG]: [PUSH]\tnumber\t\t%ld\tto queue.\n", num);
#endif
    output[noutput].is_number = true;
    output[noutput].number = num;
    noutput++;
}

// Push an operator to the output queue.
void push_operator_to_output(char op)
{
    if(noutput > MAX_QUEUE - 1)
        die("Output stack overflow!\n");
#ifdef DEBUG
    printf("[DEBUG]: [PUSH]\toperator\t%c\tto queue.\n", op);
#endif
    output[noutput].is_number = false;
    output[noutput].operator = op;
    noutput++;
}

#ifdef DEBUG
// [DEBUG ONLY] print the output stack as single rpn expression.
void read_rpn_expression()
{
    int i = 0;

    printf("\n[DEBUG]: RPN Expression: ");
    
    for(i = 0; i <= (noutput - 1); i++){
        if(output[i].is_number)
            printf("%ld ", output[i].number);
        else
            printf("%c ", output[i].operator);
    }

    printf("\n\n");
}
#endif

// Evaluate the rpn expression, stored in the output queue.
double eval_rpn()
{
#ifdef DEBUG
    printf("[DEBUG]: ----- Start solving rpn expression.\n");
#endif
    double a, b; 
    int i = 0;

    for(i = 0; i <= (noutput - 1); i++){            // Loop through the stack.
        if(output[i].is_number)                     // If number
            push_rpnstack(output[i].number);        // Push to rpn stack.
        else{
            switch(output[i].operator){             // If operator, evaluate.
                case '+': RPN_CALC(a + b); break;
                case '-': RPN_CALC(a - b); break;
                case '*': RPN_CALC(a * b); break;
                case '/': RPN_CALC(a / b); break;
                case '^': RPN_CALC(power(a, b)); break;
            }
        }
    }

    return pop_rpnstack(); // After the evaluation, return the last element on the stack.
}

// Perform the shunting yard algorithm on a given equation.
const char *shunting_yard(const char *equation)
{
    char *e = strdup(equation), *p = e;
    operator op;
#ifdef DEBUG
    printf("\n[DEBUG]: ----- Begin shunting yard.\n");
#endif    
    // Tokenize and perform a shunting yard algorithm.
    while(*p){                      // While there are tokens to read.
        if(isdigit(*p)){            // If the token is a number.
            long number = strtol(p,&p,10);  // Consume number.  
            push_number_to_output(number);  // Push to outqueue.
        }

        if(isoperator(*p)){         // If the token is an operator.
            while(opstack[nopstack-1].prec >= get_op_prec(*p)){ // While there is an operator on top of the stack with a greater or equal prec...
                op = pop_opstack(); // ..Pop the top operator out of the opstack.
                push_operator_to_output(op.op); 
            }
            push_opstack(*p);       // Push to opstack
            p++;
        }

        if(*p == '('){              // If the token is a left bracket.
            push_opstack(*p);       // Push it to the opstack.
            p++;
        }

        if(*p == ')'){              // If the token is a right bracket.
            while(opstack[nopstack-1].op != '('){   // Pop operators out of the opstack until a left bracket is reached.
                op = pop_opstack();
                push_operator_to_output(op.op);
            }
            pop_opstack();          // pop left brackets and discard it
            p++;
        }
    }
    while(nopstack > 0){            // While there are operators left.
        op = pop_opstack();         // Pop each out.
        push_operator_to_output(op.op);
    }
#ifdef DEBUG
    read_rpn_expression();          // Print the converted rpn expression.
#endif
    free(e);                        // Clean the mess.

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
        die("To few arguments! Give equation!");
    
    shunting_yard(argv[1]);
    
    printf("\n[OUTPUT]: %g\n\n", eval_rpn());

    return EXIT_SUCCESS; 
}
