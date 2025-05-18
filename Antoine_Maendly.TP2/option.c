#include "option.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Fonction qui print les instructions pour l'utilisation du programme
void usage(){
    printf("\n");
    printf("Usage : ultrals [-f] file directory");
    printf("\n\n");
}

// check si le flag -f a été mis
// retourne 1 si oui et 0 sinon
int check_fflag(int argc, char *argv[]){
    // reset des variables globales de utiliser avec getopt
    optreset = 1;
    optind = 1;

    int fflag = 0; // variable à 1 si le flag -f est donné par l'utilisateur, sinon elle est à 0

    int opt; // variable pour la boucle while avec getopt
    while ((opt = getopt(argc, argv, "f")) != -1) { // -f n'attend pas d'argument mais -t attend un argument après (un seul)
        switch (opt) {
        case 'f': // si l'utilisateur a donner le flag -f
            fflag = 1; // on met la variable à 1

            break;
        default:
            usage(); // si l'utilisateur a mal entrée les options ou a donné une option invalide
            return -1;
        }
    }
    return fflag;
}