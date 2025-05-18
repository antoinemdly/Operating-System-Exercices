#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Function take a string and returns an array of string which have been delimited by spaces
char** parsing(char *line){

    // Initialising
    char **arg = malloc(sizeof(char*) * 20); // 19 argument libre + NULL
    arg[0] = strtok(line," ");
    arg[1] = NULL;
    char *next;
    int arg_num = 1;

    // We loop over all the strings until the last one (separated by spaces)
    while((next = strtok(NULL, " ")) != NULL){
        char **temp;

        // If we exceeded the capacity of 19 arguments
        if (arg_num >= 19){
            temp = realloc(arg, sizeof(char*) * (arg_num * 2)); // There is a maximum of 39 arguments
            if (temp == NULL){
                perror("Reallocation error");
                return NULL;
            }
            temp[arg_num] = next;
            arg_num++;
            arg = temp;
            arg[arg_num] = NULL;

        }

        // Update the array and set NULL after the last argument
        else{
            arg[arg_num] = next;

            arg_num++;
            
            arg[arg_num] = NULL;
        }
        
    }

    return arg;
}

// A function that returns the number of arguments in the parsed array of arguments
int count_arg(char **parsed){
    int arg_count = 0;

    for(int i = 0; parsed[i] != NULL; i++){
        arg_count++;
    }

    return arg_count;
}