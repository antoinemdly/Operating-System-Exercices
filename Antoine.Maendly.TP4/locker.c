#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

// Fonction qui affiche l'utilisation du programme
void usage(void){
    printf("Usage :\n");
    printf("    ./locker file\n");
}

// Fonction qui affiche un message correspondant lorsqu'un verrou a bien été posé
void success_lock(struct flock *fl){
    if (fl->l_type == F_UNLCK){
        printf(GREEN"Successfully removed lock from %ld to %ld\n"RESET, fl->l_start, fl->l_start + fl->l_len);
    }
    else if (fl->l_type == F_RDLCK){
        printf(GREEN"Succesfully put a read lock from %ld to %ld\n"RESET, fl->l_start, fl->l_start + fl->l_len);
    }
    else {
        printf(GREEN"Succesfully put a write lock from %ld to %ld\n"RESET, fl->l_start, fl->l_start + fl->l_len);
    }
}

// Fonction qui affiche les infos sur l'état d'un verrou
void disp_lock_info(struct flock *fl){
    if (fl->l_type == F_UNLCK){ /* pas de verrou */
        printf("No lock at the position\n");
    }
    else if (fl->l_type == F_RDLCK){ /* read lock */
        printf(RED"There is a read lock detained by %d from %ld to %ld\n"RESET, fl->l_pid, fl->l_start, fl->l_start + fl->l_len);
    }
    else{ /* write lock*/
        printf(RED"There is a write lock detained by %d from %ld to %ld\n"RESET, fl->l_pid, fl->l_start, fl->l_start + fl->l_len);
    }
}

// Fonction qui prend des caractères et qui changes les options correspondante pour la fonction fcntl
void set_options(char opt_cmd,char opt_type,char opt_whence,int *cmd,int *type,int *whence){
    switch (opt_cmd)
        {
            case 'g':
                *cmd = F_GETLK;
                break;
            case 's':
                *cmd = F_SETLK;
                break;
            case 'w':
                *cmd = F_SETLKW;
                break;
            // case '?':
            //     break;
        }

        switch (opt_type)
        {
            case 'r':
                *type = F_RDLCK;
                break;
            case 'w':
                *type = F_WRLCK;
                break;
            case 'u':
                *type = F_UNLCK;
                break;
        }
        switch (opt_whence)
        {
            case 'c':
                *whence = SEEK_CUR;
                break;
            case 'e':
                *whence = SEEK_END;
                break;
        }

        //printf("inputs : %d %d %d\n",cmd,type,whence);
        //printf("inputs : %d %d %d\n",F_GETLK,F_RDLCK,SEEK_SET);
}

// Fonction pour la commande F_GETLK et qui vérifie les erreurs potentielles
int get_lock(int fd,int cmd, struct flock *fl){
    if (fcntl(fd, cmd, fl) == -1) {
        if (errno == EBADF || errno == EINVAL) {
            fprintf(stderr,"Error with F_GETLK : %s\n", strerror(errno));
            return -1;
        }
        else{
            fprintf(stderr,"Unsupported error with F_GETLK : %s\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

// Fonction qui affiche l'aide
void help(){
    printf(RED "    Format: cmd type start length [whence]\n" RESET);
    printf("    cmd: ’g’ (F_GETLK), ’s’ (F_SETLK), ’w’ (F_SETLKW), ’?’ to display this help\n");
    printf("    type: ’r’ (F_RDLCK), ’w’ (F_WRLCK) or ’u’ (F_UNLCK)\n");
    printf("    start: lock starting offset\n");
    printf("    length: number of bytes to lock\n");
    printf("    whence: ’s’ (SEEK_SET, default), ’c’ (SEEK_CUR) or ’e’(SEEK_END\n\n");
    printf("    Type 'q' or 'quit' to exit the program\n");
}

// Programme main
int main(int argc, char *argv[]){
    if (argc < 1){
        usage();
    }
    char *filename = argv[1];

    // ouverture du fichier source
    int fd = open(filename,O_RDWR);
    if (fd < 0){
        fprintf(stderr,"Error opening file: %s\n", strerror(errno));
        return -1;
    }

    // On récupère le pid du processus
    pid_t pid = getpid();

    // Buffer pour la lecture de l'entrée standard
    char input[1024];

    /*
    *   Boucle while infini qui ne s'arrête que l'utilisateur tape 'q' ou 'quit'
    *   A chaque itération, on lis l'entrée standard et on récupère les options grace à sscanf
    *   Ensuite on utilise la fonction fcntl avec les options donnés et en vérifiant les erreurs possibles
    */
    while(1){
        printf(CYAN"PID"RESET "=" YELLOW "%d" RESET "> ", pid); /* affiche le pid du processus */

        if (fgets(input, sizeof(input), stdin) != NULL) { /* Lecture de l'entrée standard dans le buffer 'input' */
            input[strcspn(input, "\n")] = '\0'; /* On remplace le retour à la ligne (mis automatiquement par fgets) */

            // Commande Help
            if(strcmp(input,"?") == 0 || strcmp(input,"help") == 0){
                help();
            }
            // Commande pour quitter
            else if(strcmp(input, "q") == 0 || strcmp(input, "quit") == 0){
                printf("Quitting...\n");
                break;
            }
            // l'utilisateur n'a rien mis
            else if(strcmp(input, "") == 0){
                // on ne fait rien
            }
            // Entrer une commande pour le lock
            else{
                char opt_cmd, opt_type, opt_whence;
                int start, length;

                /*
                *   Si l'utilisateur n'a pas donné l'option whence
                *   On met la valeur par défaut pour l'option whence (Ne change rien dans le switch)
                *   et aussi les différentes options sont lu dans les variables
                */
                if(sscanf(input,"%c %c %d %d %c", &opt_cmd, &opt_type, &start, &length, &opt_whence) < 5){
                    opt_whence = 's';
                }

                // Les variables pour la fonction fcntl
                int cmd,type;
                int whence = SEEK_SET;

                // On attribue les caractères aux variables correspondantes F_GETLK, F_UNLK etc...
                set_options(opt_cmd,opt_type,opt_whence,&cmd,&type,&whence);

                // Initialisation du verrou
                struct flock fl;
                fl.l_type = type;
                fl.l_whence = whence;
                fl.l_start = start;
                fl.l_len = length;

                // Commande GETLK
                if (cmd == F_GETLK){
                    // On lis le verrou et on check les erreurs
                    if (get_lock(fd,cmd,&fl) != -1){
                        // Pas d'erreur avec fcntl()
                        // On check l'état du verrou et on l'affiche
                        disp_lock_info(&fl);
                    }
                    
                }
                // Commande SETLK
                else if (cmd == F_SETLK){
                    if (fcntl(fd, cmd, &fl) == -1) {
                        // Il y a déjà un lock sur le fichier
                        if (errno == EACCES || errno == EAGAIN || errno == EINVAL) {
                            // On lis le lock qui est déjà en place et on affiche ces données
                            if (get_lock(fd,F_GETLK,&fl) != -1){
                                disp_lock_info(&fl);
                            }
                        }
                        // Potentielles autres erreurs par supporter par le programmes
                        else{
                            fprintf(stderr,"Unsupported error with F_SETLK : %s\n", strerror(errno));
                        }
                    }

                    // Il n'y a pas eu d'erreurs avec fcntl
                    else{
                        success_lock(&fl); /* On affiche le message de succès */
                    }
                }
                // Commande SETLKW
                else if (cmd == F_SETLKW){
                    // Si il y a eu une erreur avec fcntl
                    if (fcntl(fd, cmd, &fl) == -1) {
                        fprintf(stderr,"Couldn't set the lock : %s\n", strerror(errno));
                    }
                    // Il n'y a pas eu d'erreurs
                    else{
                        success_lock(&fl); /* On affiche le message de succès */
                    }
                }
            }
        }
    };

    // sortie de la boucle while
    return 0;
}