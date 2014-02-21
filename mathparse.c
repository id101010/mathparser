/*
 * mathparse.c
 *
 * Parsing equation of the form: (x/y)/((z/c)/n)
 * 
 */
#include<stdio.h>
#include<assert.h>
#include<errno.h>
#include<ctype.h>
#include<stdbool.h>

#define MAX_EQU_LENGTH 255

void die(const char *message);
int from_ascii_to_number(char c);
int filter_numbers_from_equation(char *equation, int start_value, int number_of_digits);
int parse_equation(char *equation);

void die(const char *message)
{
    if(errno){
        perror(message);
    }else{
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

int from_ascii_to_number(char c)
{
    return (float)(c - '0');
}

int filter_numbers_from_equation(char *equation, int start_value, int number_of_digits)
{
    unsigned int i = 0;
    char buf[MAX_EQU_LENGTH];
    char *pbuf = buf;
    
    for(i = start_value; i <= (number_of_digits + start_value - 1); i++)
        buf[i - start_value] = *(equation + i);
    
    buf[start_value + number_of_digits - 1] = '\0';
    
    return atoi(buf);
}

int get_numbers(char *equation)
{
    char *str = strdup(equation), *p = str;
    
    while(*p){ // While there are more characters to process.
        if(isdigit(*p)){
            long number = strtol(p, &p, 10);
            printf("%ld\n", number);
        } else {
            p++;
        }
    }

    return 0;
}
    
int parse_equation(char *equation)
{
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
        die("To few arguments! Give equation!");

    char *equation = argv[1];
    
    get_numbers(equation);

    //printf("\nSUCCESS: %s = %f\n", equation, parse_equation(equation));

    return 1; 
}
