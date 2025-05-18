#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h>

/*
*
*   Function that handles a background job.
*   1. The function fork and redirect STDIN to /dev/null
*   2. Remove the & from the arguments
*   3. Execute the program
*   4. The parent doesn't wait for the child but set the global variable to the pid of the children
*   the handler for the signals will handle the child when it finishes
*
*/
void handle_background_job(char **parsed, int arg_count, int *background_pid){
    printf("Lauching a background job...\n");

    int pid;
    if((pid = fork()) == 0){ // child
        
        int fd;

        // Opening /dev/null
        if ((fd = open("/dev/null", O_RDONLY)) < 0){
            perror("error opening /dev/null");
            exit(-1);
        }

        // Redirecting STDIN to /dev/null
        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("Error with dup2");
            close(fd);
            exit(-1);
        }

        close(fd);

        parsed[arg_count - 1] = NULL; // Removing & from the arguments

        // Executing the program
        if(execvp(parsed[0], parsed) < 0){
            perror("Incorrect command");
            exit(-1);
        }


        // exit(0); // execvp replace curren process and exit after finishing -> no need to exit

    }
    else if (pid > 0){

        // Sets the global variable to the pid of the children and doesn't wait
        *background_pid = pid;

    }
    else{
        perror("Error with fork");
        exit(EXIT_FAILURE);
    }

}

/*
*
*   Function that handles the foreground job.
*   I had to remove the handler during this function because otherwise the sigaction handler takes care of the child for some reason
*   The function just forks, and execut the program in the child
*   The parent just waits for the child to finish and sets the global variable to the pid of the child, in case a ex
* 
*/
void handle_foreground_job(char **parsed, int *foreground_pid, struct sigaction *act){

    // Removing handler on SIGCHLD because for some reason the child process gets waited in the handler and not directly in the parent ???
    struct sigaction sig_act = {0};
    sig_act.sa_handler = SIG_DFL;
    sigaction(SIGCHLD, &sig_act, NULL);

    int pid;

    // CHILD : We just fork and execute the program
    if((pid = fork()) == 0){
        if(execvp(parsed[0], parsed) < 0){
            perror("Incorrect command");
            exit(EXIT_FAILURE);
        }
    }

    // PARENT : We set the pid to the foreground in case we receive signals and we wait for the child
    else if (pid > 0) {
        
        *foreground_pid = pid;

        int status;
        pid_t child = waitpid(pid, &status, 0);
        
        // DEBUGGING
        // printf("Waited for child %d\n", child);
        // if (child == -1) {
        //     if (errno == ECHILD) {
        //         printf("No child process to wait for.\n");
        //     } else {
        //         perror("waitpid failed");
        //     }
        //     return;
        // }

        int exit_code = WEXITSTATUS(status);

        if (WIFEXITED(status)) {
            printf("Foreground job exited with code %d\n", exit_code);
        }
        else if (WIFSIGNALED(status)){
            printf("Foreground job terminated by signal %d\n", WTERMSIG(status));
        }
        else{
            printf("Foreground job exited\n");
        }

        // We finished waiting, so we set the foreground job to 0 again
        *foreground_pid = 0;

        // Setting back the handler
        sigaction(SIGCHLD, act, NULL);

    }
    else{
        perror("Error with fork");
        exit(EXIT_FAILURE);
    }
}