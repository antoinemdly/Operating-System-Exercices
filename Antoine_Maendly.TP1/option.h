#ifndef OPTION
#define OPTION

void usage();

// Fonction qui hash tous les arguments sans flag restant, les arguments désigne des fichiers
int hash_files(int argc, char *argv[], char* algo);

// Fontion qui va renvoyer un string unique à partir de tous les arguments après les flags
char *get_string(int argc, char *argv[]);

#endif