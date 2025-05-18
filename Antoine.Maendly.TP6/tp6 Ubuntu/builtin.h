#ifndef BUILTIN
#define BUILTIN

int cd(char *dst);

void exit_command(int *background_pid, int *foreground_pid);

void prompt(void);

#endif