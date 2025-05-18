#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#define TOO_LOW (-1)
#define TOO_HIGH 1
#define WIN 0
#define LOSE 2

// Fonction qui affiche l'utilisation du programme
void usage(void){
    printf("Usage :\n");
    printf("    ./client [adress_ip] [port]\n");
}

// Programme main pour le client
int main(int argc, char *argv[]){
    if (argc < 3){
        usage();
    }
    char *adresse = argv[1]; /* On récupère l'adresse en argument */
    int port = atoi(argv[2]); /* On récupère le port en argument et on le convertie en int */

    // On initalise notre adresse
    struct sockaddr_in address;
    memset( &address, 0, sizeof(address) ); /* On met la mémoire à 0 partout */

    // On récupère une adresse IP valide avec inet_pton
    int inter = 0;
    if ((inter = inet_pton( AF_INET, adresse, &(address.sin_addr) )) == 0){
        printf("Adress IP was not parseable in the specified adresse family\n");
        exit(EXIT_FAILURE);
    }
    else if(inter < 0){
        perror("Error with inet_pton");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET; /* famille d'adresse  : internet domain */
    address.sin_port = htons(port); /* htons nous convertie notre int en un port valide */

    // On crée le socket pour le client
    int clientSocket = 0;
    if( (clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("Error creating the socket");
        exit(EXIT_FAILURE);
    }

    // On se connect au server
    if (connect(clientSocket, (struct sockaddr *) &address, sizeof(address)) < 0){
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }


    int input[2]; /* Notre message qu'on transmet au server */

    // Après la connection on recoit le message initial avec l'interval
    ssize_t bytesRead = recv(clientSocket, input, sizeof(input), 0);
    if (bytesRead < 0){
        perror("Error receiving data from client");
        exit(EXIT_FAILURE);
    }
    else if(bytesRead == 0){ /* Si le client a quitté */
        printf("The server quitted...\n");
    }

    // On affiche le max et le min
    printf("-------- Game --------\n");
    printf("Try to find the exact number.\n");
    printf("The number is between %d and %d.\n\n", input[0], input[1]);

    while(1){

        // On lis l'entrée de l'utilisateur et on check si elle est valide
        printf("Enter your number : ");
        if(scanf("%d", &input[1]) != 1){
            printf("\n\nNot a valid input !\n\n");
            while (getchar() != '\n'); /* On lit le reste qui n'a pas été lu */

            continue; /* On saute, pour redemander une entrée */
        }
        else{ /* Input correct, on envoie les donnés */
            if (send(clientSocket, input, sizeof(input), 0) < 0){
                perror("Error when sending");
                exit(EXIT_FAILURE);
            }
            else{
                printf("\nProposition envoyée : %d\n", input[1]);
            }
        }

        // On recois la réponse du server
        ssize_t bytesRead = recv(clientSocket, input, sizeof(input), 0);
        if (bytesRead < 0){
            perror("Error receiving data from client");
            exit(EXIT_FAILURE);
        }
        else if(bytesRead == 0){ /* Si le server a quitté */
            printf("The server quitted...\n");
            break;
        }
        else{
            printf("La valeur réelle est: %d\n", input[1]);
        }

        // On check la réponse et on affiche l'état du jeux
        if(input[0] == TOO_LOW){
            printf("Too low !\n\n");
        }
        else if(input[0] == TOO_HIGH){
            printf("Too high !\n\n");
        }
        else if(input[0] == LOSE){
            printf("\nYou lost\n\n"); /* On a perdu, on sort de la boucle while */
            break;
        }
        else{
            printf("\nYou Won !\n\n"); /* On a gagné, on sort de la boucle while */
            break;
        }
    }

    if(close(clientSocket) < 0){ /* On a finis, on peut donc fermer le socket */
        perror("Error closing the socket");
        exit(EXIT_FAILURE);
    }

    return 0;
}