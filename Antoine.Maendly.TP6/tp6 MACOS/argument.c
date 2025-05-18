#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Function take a string and returns an array of string which have been delimited by spaces
char** parsing(char *line){
    char **arg = malloc(sizeof(char*) * 2); // 1 argument libre + NULL
    arg[0] = strtok(line," ");
    arg[1] = NULL;
    char *next;
    int arg_num = 1;

    while((next = strtok(NULL, " ")) != NULL){
        char **temp = realloc(arg, sizeof(char*) * (arg_num + 1));
        if (temp == NULL){
            perror("Reallocation error");
            return NULL;
        }
        temp[arg_num] = next;
        arg_num++;
        arg = temp;
        arg[arg_num] = NULL;
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