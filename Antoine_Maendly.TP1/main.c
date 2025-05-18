#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "option.h"
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
    // reset des variables globales de utiliser avec getopt
    optreset = 1;
    optind = 1;

    char *algo = "sha1"; // variable qui décrit l'algorithme utilisé pour le digest. La valeur par défault est sha1
    int fflag = 0; // variable à 1 si le flag -f est donné par l'utilisateur, sinon elle est à 0

    int opt; // variable pour la boucle while avec getopt
    while ((opt = getopt(argc, argv, "ft:")) != -1) { // -f n'attend pas d'argument mais -t attend un argument après (un seul)
        switch (opt) {
        case 'f': // si l'utilisateur a donner le flag -f
            fflag = 1; // on met la variable à 1

            break;
        case 't': // si l'utilisateur a donner le flag -t
            algo = optarg; // on récupère le nom de l'algo donné par l'utilisateur grace à la variable optarg qui pointe sur l'argument de -t

            break;
        // case '?':
        //     usage();
        default:
            usage(); // si l'utilisateur a mal entrée les options ou a donné une option invalide
            return -1;
        }
    }

    if (fflag){ // si le flag -f a été activé, on utilise la fonction hash_file

        hash_files(argc, argv, algo); // la fonction loop sur tous les arguments sans flag restant et hash les fichiers donné
    }
    else{ // si le flag -f n'a pas été donné, les arguments restant sont la chaine de caractère à hasher
        char *concatenated_string = get_string(argc, argv); // ici on crée une seul chaine de caractère à partir de tous les arguments donnée

        hashing_string(concatenated_string, algo); // puis on hash le string
    }

    return 0;
}