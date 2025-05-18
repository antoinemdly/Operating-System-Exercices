#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "copy.h"

void time_copy(int (*f)(char*, char*, unsigned int), char *src, char *dst, unsigned int buf_size, int f_type) {
    // Mesure du temps et récupération du nombres de bytes copiés
    struct timespec start, finish;
    int copied_bytes;

    clock_gettime(CLOCK_MONOTONIC, &start);
    copied_bytes = f(src, dst, buf_size);
    clock_gettime(CLOCK_MONOTONIC, &finish);

    double seconds_elapsed = finish.tv_sec-start.tv_sec;
    seconds_elapsed += (finish.tv_nsec-start.tv_nsec)/1000000000.0;

    // Lecture de la taille du fichier source
    struct stat fileStat;
    if (stat(src, &fileStat) < 0){
        printf("Error stat: %s\n", strerror(errno));
    }

    // Caractère 'f' pour afficher copy ou copyf
    char char_f;
    if (f_type){
        char_f = 'f';
    }
    else{
        char_f = '\0';
    }

    printf("Copied (%cread/%cwrite) \"%s\" into \"%s\" (%lldKB in chunks of %d bytes) in %f ms\n",char_f,char_f,src,dst,fileStat.st_size/1024,buf_size,seconds_elapsed);
}

int main(int argc, char *argv[]){

    if (argc < 3){
        printf("Must have at least 2 argments\n");
        usage();
    }

    // tests avec la fonction copy
    time_copy(copy,argv[1],argv[2],1,0);     /* Buffer de 1 byte */
    time_copy(copy,argv[1],argv[2],32768,0); /* Buffer de 32KB */

    // tests avec la fonction copyf
    time_copy(copyf,argv[1],argv[2],1,1);     /* Buffer de 1 byte */
    time_copy(copyf,argv[1],argv[2],32768,1); /* Buffer de 32KB */


    return 0;
}