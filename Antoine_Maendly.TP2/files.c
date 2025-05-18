#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h> // erreur
#include <time.h> // Pour ctime
#include <limits.h>  // Pour PATH_MAX

#include "utilities.h"

// définition des couleurs pour printer en couleur
#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN    "\x1B[36m"
#define WHITE   "\x1B[37m"

// Fonction qui affiche les informations d'un fichier
// si l'argument fflag est égal a 1, on lit les infos des fichier pointé par les liens symbolique
// Si le type du fichier est un dossier, la fonction va récursivement afficher les informations sur tous les fichiers à l'intérieur
int display_stats(char *filename, int fflag){
    struct stat infos;
    int is_link = 0; // permet de garder en mémoire si le fichier est un lien symbolique

    // lecture de l'inode du fichier "filename"
    if( lstat( filename, &infos ) < 0 ) {
        fprintf( stderr, "Cannot stat %s: %s\n", filename, strerror(errno) );
        return -1;
    }
    else { // il n'y a pas eu d'erreur lors de la lecture de l'inode
        // printf("Number of Links: %ld\n", (long) infos.st_nlink);

        // si -f est donné et le fichier est un lien symbolique
        if (fflag && S_ISLNK(infos.st_mode)){
            is_link = 1; // on garde en mémoire que le fichier est un lien symbolique

            // On récupère le fichier pointer par le lien symbolique
            char *pointed = pointed_file(filename);

            // on lit l'inode du fichier pointer
            if( lstat( pointed, &infos ) < 0 ) {
                fprintf( stderr, "Cannot stat %s: %s\n", pointed, strerror(errno) );
                return -1;
            }
        }


        // On affiche le type, les permissions, la taille, et la derniere date de modification
        perm_size_date(infos);


        // AFFICHAGE DU CHEMIN RELATIF et APPEL RECURSIF

        // Si le fichier est un lien symbolique on affiche : "filename" -> le fichier pointer (grace a la fonction pointed_file)
        if (S_ISLNK(infos.st_mode) || is_link){ // si le fichier est un lien symbolique ou  (si -f a été donné) is_link = 1

            printf(RED "%s" RESET, filename); // lien symbolique en rouge

            printf(CYAN " -> " RESET); // fleche en cyan
            printf(MAGENTA "%s" RESET,pointed_file(filename)); // le fichier pointé en magenta
        }
        // Si le ficher est un dossier et que on n'est pas en train de lire un lien symbolique qui pointe vers ce dossier (avec -f flag)
        // On affiche les infos pour tous les fichiers dedans
        else if (S_ISDIR(infos.st_mode) && !is_link){

            printf(BLUE "%s\n" RESET, filename); //chemin du dossier en bleu

            char *dir_name = filename;
            DIR *d = opendir(dir_name);
            struct dirent *entry;
            char *entry_name;   //nom d'une entrée

            //En cas d'erreur d'ouverture
            if (! d) {
                fprintf(stderr, "Cannot open directory '%s': %s\n", dir_name, strerror(errno));
                return -1;
            }

            // Boucle sur chaque entrée
            // Pour chaque entrée , on utiliser notre fonction display_stats récursivement pour afficher les infos de chaque fichiers dans le dossier
            while( (entry = readdir(d)) != NULL ) {

                // Obtient le nom de l'entrée
                entry_name = entry->d_name;

                // On ignore les dossiers '.' et ".." et on ne les affiche pas récursivement
                if ( entry_name[0] == '.' || strcmp(entry_name, "..") == 0 ){
                    continue;
                }

                // on récupère le chemin jusqu'a l'entrée actuelle
                char *path = get_path(dir_name, entry_name);

                // on affiche les infos pour ce fichier dans notre dossier
                display_stats(path, fflag);
            }

        }
        // Pour les fichier courrant
        else {
            printf("%s", filename); // on print juste le chemin relatif du fichier en blanc
        }
    }

    printf("\n");
    return 0;
}