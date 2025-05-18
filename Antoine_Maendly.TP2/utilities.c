#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h> // erreur
#include <time.h> // Pour ctime
#include <limits.h>  // Pour PATH_MAX

// prend en argument dirent d -> d.d_type
// retourne un char correspondant
char *get_type(char type){
    // DT_UNKNOWN also

    switch (type)
    {
    case DT_DIR:
        return "dir";
    case DT_LNK:
        return "link";
    case DT_REG:
        return "reg";
    default:
        return "else";
    }
}

// Retourne le temps en format normal en string
char *human_time(time_t time){
    char *modified_time = ctime(&time);

    modified_time[strcspn(modified_time, "\n")] = '\0'; // cherche le symbole de retour à la ligne et le remplace par \0 (on enlève le retour à la ligne)

    return modified_time;
}

// fonction qui prend un lien symbolique et qui retourne le nom du fichier pointé
char *pointed_file(char *filename){
    char *buffer = malloc(PATH_MAX); // taille max d'un chemin possible

    // check erreur d'allocation mémoire
    if (!buffer) {
        perror("malloc failed");
        return NULL;
    }

    // Readlink, on lit le lien symbolique
    ssize_t info = readlink(filename, buffer, PATH_MAX);

    // check si la lecture a eu une erreur
    if (info == -1) {
        perror("readlink");
        return NULL;
    }

    return buffer;
}

// print toute les données nécesaire
// on print : le type, les permissions, la taille et la derniere date de modification
void perm_size_date(struct stat infos){
    // type de fichier
    if (S_ISDIR(infos.st_mode)){
        printf("\n");
        printf("d");
    }
    else if (S_ISLNK(infos.st_mode)){
        printf("l");
    }
    else {
        printf("-");
    }
    // permission du fichier
    printf( (infos.st_mode & S_IRUSR) ? "r" : "-");
    printf( (infos.st_mode & S_IWUSR) ? "w" : "-");
    printf( (infos.st_mode & S_IXUSR) ? "x" : "-");
    printf( (infos.st_mode & S_IRGRP) ? "r" : "-");
    printf( (infos.st_mode & S_IWGRP) ? "w" : "-");
    printf( (infos.st_mode & S_IXGRP) ? "x" : "-");
    printf( (infos.st_mode & S_IROTH) ? "r" : "-");
    printf( (infos.st_mode & S_IWOTH) ? "w" : "-");
    printf( (infos.st_mode & S_IXOTH) ? "x" : "-");

    // taille du fichier
    printf("  %8lld", (long long) infos.st_size);
    
    printf("  %s  ", human_time(infos.st_mtime)); // derniere modification
}

// fonction qui retourne le chemin complet étant donné un dossier et un nom d'une entrée dans ce dossier
char *get_path(char *dir_name, char *entry_name){
    // + 1 pour le /
    char *path = malloc((strlen(dir_name) + strlen(entry_name) + 1)*sizeof(char));

    // erreur d'allocation memoire
    if (path == NULL){
        perror("Malloc error");
        return NULL;
    }

    strcpy(path, dir_name);

    // nom du dossier + /
    strcat(path, "/");

    // nom du dossier + / + nom du fichier
    strcat(path, entry_name);

    return path;
}