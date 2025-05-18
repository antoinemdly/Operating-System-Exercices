#include "option.h"
#include "hash.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Fonction qui print les instructions pour l'utilisation du programme
void usage(){
    printf("\n");
    printf("Usage files : hash [-t algorithm_name] [-f file1 file2 ...]");
    printf("\n");
    printf("Usage string : hash [-t algorithm_name] this is my string");
    printf("\n\n");
}

// Fonction qui hash tous les arguments sans flag restant, les arguments désigne des fichiers
int hash_files(int argc, char *argv[], char* algo){
    // On démarre la boucle a optind car cette variable est placé sur le numéro du première argument après les flags.
    // On parcours tous les arguments jusqu'aux derniers
    for(int i = optind; i < argc; i++){
        char* filename = argv[i]; // On récupère le nom du fichier

        hashing_file(filename, algo); // on hash le fichier avec notre fontion hashing_file
    }

    return 0;
}

// Fontion qui va renvoyer un string unique à partir de tous les arguments après les flags
char *get_string(int argc, char *argv[]){

    int arg_num = argc - optind; // On récupère le nombre d'argument situé après les flags (qu'on a pas encore traité)
        
    // On calcule la taille totale requise pour mettre tous les strings ensemble
    int total_length = 0;
    for (int i = optind; i < argc; i++) { // Pour tous les arguments en question, on ajoute leur taille à total_length
        total_length += strlen(argv[i]) + 1; // on ajoute ici +1 pour ajouter le caractère de fin de string '\0'
    }

    // On alloue la mémoire pour le grand string
    char* concatenated_string = malloc(total_length * sizeof(char));
    if (concatenated_string == NULL) {
        printf("Error when allocating memory\n");
        return NULL;
    }

    // On parcours tous les arguments après les flags et on les concatène au fur et a mesure à notre grand string dont on a alloué assez de mémoire
    for(int i = optind; i < argc; i++){

        strcat(concatenated_string, argv[i]); // Concatenation du grand string et les arguments

        if (i < argc - 1) {
            strcat(concatenated_string, " "); // Sauf pour le dernier, pour chaque string qu'on concatène, on ajoute un espace
        }
    }

    return concatenated_string;
}