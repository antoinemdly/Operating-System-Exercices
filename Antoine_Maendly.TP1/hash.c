#include "hash.h"
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

// Fonction qui calcule le digest d'un string donnée en argument et print le digest
int hashing_string(char* string,char* algorithm){
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    // check si l'algo est valid
    // retourne un pointeur vers une structure EVP_MD qui représente l'algo en question
    md = EVP_get_digestbyname(algorithm);
    if (md == NULL) {
        printf("Incorrect algorithm name %s\n", algorithm);
        return -1;
    }

    // initialise un nouveau digest
    mdctx = EVP_MD_CTX_new();
    if (!EVP_DigestInit_ex2(mdctx, md, NULL)) {
        printf("Erreur initialisation\n");
        EVP_MD_CTX_free(mdctx);
        return -1;
    }
    // met à jour le digest avec notre string d'entrée
    if (!EVP_DigestUpdate(mdctx, string, strlen(string))) {
        printf("Erreur update\n");
        EVP_MD_CTX_free(mdctx);
        return -1;
    }
    // finalise le digest
    if (!EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
        printf("Message digest finalization failed.\n");
        EVP_MD_CTX_free(mdctx);
        return -1;
    }
    EVP_MD_CTX_free(mdctx);

    for (i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
    printf("     %s     %s", string, algorithm);
    printf("\n");

    return 0;
}

// Fonction qui calcule le digest d'un fichier donnée en argument et print le digest
int hashing_file(char *filename, char* algorithm){
    FILE *file; // Variable pour le fichier
    char c; // variable utilisé quand on parcours le fichier, c stock chaque charctère du fichier au fur et a mesure

    // Variable pour le hachage
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;

    // On ouvre le fichier
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file : %s\n", filename);
        return -1;
    }

    // check si l'algo est valid
    // retourne un pointeur vers une structure EVP_MD qui représente l'algo en question
    md = EVP_get_digestbyname(algorithm);
    if (md == NULL) {
        printf("Incorrect algorithm name %s\n", algorithm);
        return -1;
    }

    // initialise un nouveau digest
    mdctx = EVP_MD_CTX_new();
    if (!EVP_DigestInit_ex2(mdctx, md, NULL)) {
        printf("Erreur initialisation\n");
        EVP_MD_CTX_free(mdctx);
        return -1;
    }

    // On va update le digest pour chaque caractère qu'on va lire dans le fichier.
    // On lit tous le fichier jusqu'a la fin. Quand fread est à la fin du fichier, on sort de la boucle while
    while (fread(&c, sizeof(char), 1, file))
    {
        if (!EVP_DigestUpdate(mdctx, &c, 1)) {
            printf("Erreur update\n");
            EVP_MD_CTX_free(mdctx);
            return -1;
        }
    }
    // on peut maintenant fermer le fichier
    fclose(file);

    // finalise le digest
    if (!EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
        printf("Message digest finalization failed.\n");
        EVP_MD_CTX_free(mdctx);
        return -1;
    }
    EVP_MD_CTX_free(mdctx);

    // On affiche le digest avec le nom du fichier et le nom de l'algorithme
    for (i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
    printf("     %s     %s", filename, algorithm);
    printf("\n");

    return 0;
}