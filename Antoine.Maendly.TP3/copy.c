#include <stdio.h>
#include <unistd.h> // read
#include <stdlib.h>
#include <fcntl.h> // argument pour open
#include <errno.h>
#include <string.h> // strerror
#include <dirent.h>
#include <limits.h> // max path
#include <sys/stat.h>

// Fonction qui print les instructions pour l'utilisation du programme
void usage(){
    printf("Usage : copybench SRC DST");
    printf("\n");
}

int copy(char *src, char *dst, unsigned int buf_size){
    
    // ouverture du fichier source
    int src_fd = open(src,O_RDONLY);
    if (src_fd < 0){
        fprintf(stderr,"Error opening file: %s\n", strerror(errno));
        return -1;
    }

    /*
    * ouverture du fichier de destination
    * si le fichier n'existe pas, on le crée avec O_CREAT
    * avec les permissions user rw, group r, others r
    */
    int dst_fd = open(dst, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (dst_fd < 0){
        fprintf(stderr,"Error opening file: %s\n", strerror(errno));
        close(src_fd);
        return -1;
    }

    // Allocation du buffer
    char *buf = malloc(buf_size * sizeof(char));
    if (!buf){
        fprintf(stderr,"Error memory allocation: %s\n", strerror(errno));
        return -1;
    }

    ssize_t Rbytes, Wbytes; // bytes qui ont été lu ou écris
    int copied_bytes = 0;

    /*
    *   Tant qu'il n'y a pas d'erreur, On lis des blocs de taille buf_size puis on 
    *   les écrits dans le fichier de destinations
    *   si il y a une erreur dans la lecture ou l'écriture, on ferme les fichiers
    */
    while((Rbytes = read(src_fd,buf,buf_size)) > 0){ /* lecture */
        Wbytes = write(dst_fd,buf,Rbytes); /* écriture */
        if (Wbytes < 0){
            fprintf(stderr,"Error writting file: %s\n", strerror(errno));
            close(src_fd);
            close(dst_fd);
            return -1;
        }
        copied_bytes += Wbytes; /* sommes des bytes copier */
    }

    free(buf); /* on a plus besoin du buffer */

    // Si on est sortie de la boucle à cause d'une erreur de lecture
    if(Rbytes < 0){
        fprintf(stderr,"Error reading file: %s\n", strerror(errno));
        return -1;
    }

    // On peut fermer les fichiers car on a fini
    close(src_fd);
    close(dst_fd);

    // Sucess
    return copied_bytes;
}

int copyf(char *src, char *dst, unsigned int buf_size){
    // ouverture du fichier source
    FILE *src_file = fopen(src,"r");
    if (!src_file){
        fprintf(stderr,"Error opening file: %s\n", strerror(errno));
        return -1;
    }

    // ouverture du fichier de destination
    // fopen crée le fichier si il n'existe pas en mode écriture
    FILE *dst_file = fopen(dst,"w");
    // Permission du fichier (user rw, group r, others r)
    if (chmod(dst, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0){
        fprintf(stderr,"Error changing permission for dst : %s\n", strerror(errno));
    }
    if (!dst_file){
        fprintf(stderr,"Error opening file: %s\n", strerror(errno));
        fclose(src_file);
        return -1;
    }

    // Allocation du buffer
    char *buf = malloc(buf_size * sizeof(char));
    if (!buf){
        fprintf(stderr,"Error memory allocation: %s\n", strerror(errno));
        return -1;
    }

    ssize_t Rbytes, Wbytes; // bytes qui ont été lu ou écris
    int copied_bytes = 0;

    /*
    *   Tant qu'il n'y a pas d'erreur, On lis des blocs de taille buf_size puis on 
    *   les écrits dans le fichier de destinations
    *   si il y a une erreur dans la lecture ou l'écriture, on ferme les fichiers
    */
    while((Rbytes = fread(buf,1,buf_size,src_file)) > 0){ /* lecture de buf_size éléments, tous de taille 1 byte */
        Wbytes = fwrite(buf,1,buf_size,dst_file); /* écriture de buf_size éléments, tous de taille 1 byte */
        if (Wbytes <= 0){
            fprintf(stderr,"Error writting file: %s\n", strerror(errno));
            fclose(src_file);
            fclose(dst_file);
            return -1;
        }
        copied_bytes += Wbytes; /* sommes des bytes copier */
    }

    free(buf); /* on a plus besoin du buffer */

    // Si on est sortie de la boucle à cause d'une erreur de lecture
    if(ferror(src_file)){
        fprintf(stderr,"Error reading file: %s\n", strerror(errno));
        return -1;
    }

    // On peut fermer les fichiers car on a fini
    fclose(src_file);
    fclose(dst_file);

    // Sucess
    return copied_bytes;
}