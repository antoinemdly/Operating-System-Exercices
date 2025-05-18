#ifndef JOBS
#define JOBS

void handle_background_job(char **parsed, int arg_count, int *background_pid);

void handle_foreground_job(char **parsed, int *foreground_pid, struct sigaction *act);

#endif