#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>

#include "builtin.h"
#include "argument.h"
#include "jobs.h"

// Global variables that says if a foreground or background job is running
pid_t background_pid = 0;
pid_t foreground_pid = 0;

// Handler function for all signals, used in the sigaction function
void sig_handler(int signum, siginfo_t* info, void* mydata){

    // Standard behavior -> ignoring the signal received and currently handling (didnt set because it works without)
    // sigset_t block;
    // sigemptyset(&block);
    // sigaddset(&block, signum);
    // sigprocmask(SIG_BLOCK, &block, NULL);

    // ---- Handling SIGINT ----
    if (signum == SIGINT){
        
        // Redirecting SIGINT to the foreground process
        if(foreground_pid != 0) {
            kill(foreground_pid, SIGINT);
        }
        else{ // doing nothing
            printf("\n");
            prompt();
            fflush(stdout); // Prevent from shell not displaying the prompt
        }

    }

    // ---- Handling SIGCHLD ----
    else if (signum == SIGCHLD){

        // Receiving from the background job -> has ended
        if (info->si_pid == background_pid){
            if (info->si_code){
                
                printf("\nBackground job exited with code %d\n", info->si_code);
                background_pid = 0;
                prompt();
                fflush(stdout); // because for some reason the shell doesnt printf otherwise

            }

            waitpid(info->si_pid,NULL,0);
        }
    }

    else if(signum == SIGHUP){
        if (foreground_pid != 0){
            if (kill(foreground_pid, SIGHUP) < 0){
                perror("Couldnt SIGHUP foreground job");
            }
            else{
                printf("Foreground job ended by SIGHUP\n");
            }
        }
        if (background_pid != 0){
            if (kill(background_pid, SIGHUP) < 0){
                perror("Couldnt SIGHUP background job");
            }
            else{
                printf("Background job ended by SIGHUP\n");
            }
        }

        printf("\nExiting sucessfully after sighup...\n");
        exit(EXIT_SUCCESS);
    }


    // sigprocmask(SIG_UNBLOCK, &block, NULL);


}

// Programme main
int main(int argc, char *argv[]){

    // Blocking SIGTERM and SIGQUIT
    sigset_t block;
    sigemptyset(&block);
    sigaddset(&block, SIGTERM);
    sigaddset(&block, SIGQUIT);
    sigprocmask(SIG_BLOCK, &block, NULL);

    // Setting the handler for the signals on SIGINT, SIGCHLD and SIGHUP
    struct sigaction act = {0};
    act.sa_flags = SA_SIGINFO | SA_RESTART;
    act.__sigaction_u.__sa_sigaction = &sig_handler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGCHLD, &act, NULL);
    sigaction(SIGHUP, &act, NULL);

    // Displaying intial prompt
    prompt();
    
    // Variables for reading user input
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;

    // while loop that only stops if there user entered the command exit
    while ((linelen = getline(&line, &linecap, stdin)) > 0){

        line[linelen - 1] = '\0'; // removing endl

        // Parsing the input of the user
        char **parsed = parsing(line);
        if (parsed == NULL){
            perror("Couldn't parse the input\n");
            continue;
        }

        // Counting the number of args
        int arg_count = count_arg(parsed);

        // Doing nothing if no argument is given
        if (arg_count == 0){
            prompt();
            continue;
        }

        // --- CD command ---
        else if (strcmp(parsed[0], "cd") == 0){
            if(!(arg_count < 3)){
                printf("Only one argument for CD\n");
            }
            
            if (cd(parsed[1]) < 0){
                printf("Couldn't change directory\n");
            }
        }

        // --- EXIT command ---
        else if (strcmp(parsed[0], "exit") == 0){
            

            exit_command(&background_pid, &foreground_pid);

            free(parsed);

            exit(EXIT_SUCCESS); // exiting the shell


        }

        // --- JOBS handling ---
        else if (strcmp(parsed[arg_count - 1],"&") == 0){
            
            // if the last argument is '&', we launch the background job
            handle_background_job(parsed, arg_count, &background_pid);

        }
        else{
            
            // otherwise, we launch it as a foreground job
            handle_foreground_job(parsed, &foreground_pid, &act);
        
        }

        free(parsed);
        prompt();        
        
    }


    return 0;
}