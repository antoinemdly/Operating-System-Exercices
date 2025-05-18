#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <ctype.h> // test de nombre

#define TOO_LOW (-1)
#define TOO_HIGH 1
#define WIN 0
#define LOSE 2

// Fonction qui affiche l'utilisation du programme
void usage(void){
    printf("Usage :\n");
    printf("    ./server [port]\n");
}

// Fonction qui retourne un nombre aléatoire entre min et max en lisant le fichier /dev/urandom
int random_number(int min, int max){
    // On ouvre le fichier /dev/urandom
    FILE *file = fopen("/dev/urandom", "rb");
    if (file == NULL) {
        perror("Error with fopen");
        return -1;
    }

    // nombre aléatoire qu'on va retourner
    int random = 0;

    size_t bytes_read = fread(&random, sizeof(random), 1, file); // On va lire un byte dans le fichier urandom qui est donc aléatoire

    if (bytes_read != 1) {
        perror("Error with fread");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    random = random % max; // On prend le modulo max pour pas que la valeur sois plus grande que le max

    while(random <= min){ // Si la valeur est plus petite, on relis un byte jusq'a ce que la valeur sois plus grande ou egal au min
        size_t bytes_read = fread(&random, sizeof(random), 1, file);

        if (bytes_read != 1) {
            perror("Error with fread");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        random = random % max; // encore une fois, on prend le modulo max pour la borne supérieur
    }
    
    fclose(file);

    return random;
}

// Programme main du server
int main(int argc, char *argv[]){
    if (argc < 2){
        usage();
    }
    int port = atoi(argv[1]); // On récupère le port en argument et on le convertie en int

    // On initalise notre adresse
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address)); // On met la mémoire à 0 partout
    address.sin_family = AF_INET; // famille d'adresse  : internet domain
    address.sin_addr.s_addr = htonl(INADDR_ANY); // Ici, on met INADDR_ANY pour recevoir n'importe quel adresse ip : 0.0.0.0
    address.sin_port = htons(port); // htons nous convertie notre int en un port valide

    // On crée le socket sur le server
    int serverSocket = 0;
    if( (serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating the socket");
        exit(EXIT_FAILURE);
    }

    // On lie le socket à notre adresse défini
    if(bind(serverSocket, (struct sockaddr *) &address, sizeof(address)) < 0){
        perror("Bind error");
        exit(EXIT_FAILURE);
    }

    // Le socket peut accepter des connections
    if(listen(serverSocket, 5) < 0){
        perror("listen error");
        exit(EXIT_FAILURE);
    }


    /*
    *
    * Première boucle while qui tourne tant que l'utilisateur ne ferme pas le server, si il fait ctrl-c.
    * Dans cette boucle, on va attendre continuellement des connections de clients et
    * pour chaque clients qui se connecte, on accepte sa connection et on fork notre processus pour gérer ce client
    * 
    */
    while(1){
        // On initialise l'adresse pour le client
        struct sockaddr_in clientAddress;
        unsigned int clientLength = sizeof(clientAddress);

        // On accepte les connections entrante
        int clientSocket = 0;
        if( (clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength)) < 0){
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        // Quand une connection est accepté, on fork notre processus
        int pid = fork();
        if(pid == -1){
            perror("Error forking");
            exit(EXIT_FAILURE);
        }

        /* 
        * 
        * On gère notre client, qu'on vient d'accepter, dans le proccesus enfant.
        * Le proccesus parent va refaire la boucle et donc accepter d'autres clients
        * 
        */
        if (pid > 0){ /* Si on est dans le proccesus enfant */
            int table[2]; /* Notre message qu'on transmet au client */

            int min = 10;
            int max = 50;
            int random = random_number(min,max); /* Le numéro aléatoire que le client doit trouver */

            int MAX_ATTEMPT = 10;
            int Current_attempt = 0;

            // On récupère l'adresse ip du client
            char ipv4_buffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddress.sin_addr, ipv4_buffer, INET_ADDRSTRLEN);

            // printf("Client %d is connected. His random number is %d. His IP adresse is %s\n\n", clientSocket, random, ipv4_buffer);
            printf("Client %d connecté avec l’ip %s\n", clientSocket, ipv4_buffer);
            printf("La valeur %d est choisie pour le client %d\n\n", random, clientSocket);


            // Envoie du message initial
            table[0] = min;
            table[1] = max;
        
            if (send(clientSocket, table, sizeof(table), 0) < 0){
                perror("Error when sending");
                exit(EXIT_FAILURE);
            }

            /*
            *
            * Deuxième boucle while qui tourne jusqu'a ce que le joueur a fini de jouer (il a perdu ou gagné ou il a quitter)
            * A chaque boucle, on lis ce que le client envoie, on incrémente son nombre d'essaies et ensuite on check
            * son message et on lui renvoie des indications ou si il a gagné/perdu
            * 
            */
            while(1){
                // On attend de recevoir le messge du client
                ssize_t bytesRead = recv(clientSocket, table, sizeof(table), 0);
                if (bytesRead < 0){
                    perror("Error receiving data from client");
                    exit(EXIT_FAILURE);
                }
                else if(bytesRead == 0){ /* Si le client a quitté */
                    printf("The client quitted...\n");
                    break;
                }

                // On met à jour le nombre d'essais
                Current_attempt++;

                // Affichage des infos pour le server
                printf("--------- For %s : %d ---------\n", ipv4_buffer,random);
                printf("Client %d propose %d\n", clientSocket, table[1]);
                printf("Attempts left : %d\n\n", 10 - Current_attempt);

                if(Current_attempt >= MAX_ATTEMPT){ /* si le joueur a dépassé le nombre d'essaie, on lui renvoie LOSE et on sors de la boucle */
                    table[0] = LOSE;
                    table[1] = random;
                    if (send(clientSocket, table, sizeof(table), 0) < 0){
                        perror("Error when sending");
                        exit(EXIT_FAILURE);
                    }

                    printf("\nClient %d lost :(\n", clientSocket);
                    printf("Closing socket for Client %d with ip %s.\n\n", clientSocket,ipv4_buffer);
                    
                    break;
                }
                else if(table[1] < random){ /* Si le nombre est trop bas */
                    table[0] = TOO_LOW;
                }
                else if(table[1] > random){ /* Si le nombre est trop haut */
                    table[0] = TOO_HIGH;
                }
                else{ /* le client a trouvé le nombre, on lui renvoie WIN et on sors de la boucle */
                    table[0] = WIN;
                    table[1] = random;
                    if (send(clientSocket, table, sizeof(table), 0) < 0){
                        perror("Error when sending");
                        exit(EXIT_FAILURE);
                    }

                    printf("\nClient %d won :)\n", clientSocket);
                    printf("Closing socket for Client %d with ip %s.\n\n", clientSocket, ipv4_buffer);

                    break;
                }

                // Envoie du message si il n'a pas encore trouvé
                table[1] = random;
                if (send(clientSocket, table, sizeof(table), 0) < 0){
                    perror("Error when sending");
                    exit(EXIT_FAILURE);
                }
            }

            if(close(clientSocket) < 0){ /* On a finis, on peut donc fermer le socket */
                perror("Error closing the socket");
                exit(EXIT_FAILURE);
            }
        }
    }

    // On quitte le programme donc on ferme le socket
    if(close(serverSocket) < 0){
        perror("Error closing the socket");
        exit(EXIT_FAILURE);
    }

    return 0;
}