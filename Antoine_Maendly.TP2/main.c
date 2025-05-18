// #include <stdio.h>
// #include <stdlib.h>
#include <unistd.h>
// #include <string.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <errno.h>
// #include <time.h>

#include "files.h"
#include "option.h"
#include "utilities.h"

int main(int argc, char *argv[]){

    int fflag = check_fflag(argc,argv); // variable à 1 si le flag -f est donné par l'utilisateur, sinon elle est à 0
    if (fflag == -1){
        return -1;
    }

    // pour le reste des arguments donnés, on affiche les infos de tous les fichiers récursivement
    for (int i = optind; i < argc; i++){
        display_stats(argv[i], fflag);
    }

    return 0;
}