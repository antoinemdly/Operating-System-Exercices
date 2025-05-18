#ifndef UTILITIES
#define UTILITIES

#include <time.h>  // time_t
#include <sys/stat.h> // stat structure

char *get_type(char type);

char *human_time(time_t time);

char *pointed_file(char *filename);

void perm_size_date(struct stat infos);

char *get_path(char *dir_name, char *entry_name);

#endif