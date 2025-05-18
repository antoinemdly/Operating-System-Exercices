#ifndef HASH
#define HASH

// Fonction qui calcule le digest d'un string donnée en argument et print le digest
int hashing_string(char* string, char* algorithm);

// Fonction qui calcule le digest d'un fichier donnée en argument et print le digest
int hashing_file(char *filename, char* algorithm);

#endif