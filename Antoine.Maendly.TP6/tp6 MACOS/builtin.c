#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<limits.h>
#include<stdlib.h>
#include<signal.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Function that take a destination and change the current working directory (relative or absolute)
int cd(char *dst){
    
    // the path is absolute, so we use the chdir function directly on dst
    if(dst[0] == '/'){
        if (chdir(dst) < 0){
            perror("Error changing working directory");
            return -1;
        }
    }

    // the path is relative
    else{
        // buffer to get the path to the CWD
        char buf[PATH_MAX];
        char *path = NULL;

        // Getting the Current Working Directory
        if((path = getcwd(buf, sizeof(buf))) == NULL){
            perror("Error with getcwd");
            return -1;
        }
        
        // Buffer to concatenate CWD + relative path
        char *new_path = malloc(sizeof(char) * PATH_MAX);

        strcpy(new_path, path); // Copying CWD
        strcat(new_path, "/"); // Adding /
        strcat(new_path, dst); // Adding the relative path

        // printf("newpath : %s\n", new_path);

        if (chdir(new_path) < 0){
            perror("Error changing working directory");
            return -1;
        }
    }

    return 0;
}

// Display just the prompt with the current working directory
void prompt(void){
    char buf[PATH_MAX];
    char *path = NULL;

    if((path = getcwd(buf, sizeof(buf))) == NULL){
        perror("Error with getcwd");
    }
    else{
        printf("%s%s%s (%s%d%s)> ",CYAN ,buf ,RESET ,RED, getpid(),RESET);
    }
}

/*
*
*   If there is a background or foreground process running,
*   the function tries to send a SIGTERM and if it doesn't work, it sends a SIGKILL which can't be ignored
*   Exiting the shell is in the main function
*
*/

void exit_command(int *background_pid, int *foreground_pid){
    // Exiting background the job
    if (*background_pid != 0){

        printf("Background job is running, trying to terminate it\n");

        if (kill(*background_pid, SIGTERM) < 0){
            perror("Couldn't SIGTERM background job");
            printf("Sending SIGKILL to background job...\n");
            kill(*background_pid, SIGKILL);
        }
        else {
            printf("Background job terminated successfully\n");
        }
    }

    // Exiting the foreground job
    if (*foreground_pid != 0){

        printf("Foreground job is running, trying to terminate it\n");

        if (kill(*foreground_pid, SIGTERM) < 0){
            perror("Couldnt SIGTERM foreground job");
            printf("Sending SIGKILL to foreground job...\n");
            kill(*foreground_pid, SIGKILL);
        }
        else {
            printf("Foreground job terminated successfully\n");
        }
    }

    printf("Exiting...\n");
}