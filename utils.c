#include "utils.h"
#include <string.h>
#include <stdlib.h> 


//Compare deux chaînes de caractères.
//Retourne 0 si les chaînes sont identiques (ou toutes les deux NULL)

int comparerChaines(char* chaine1, char* chaine2) {
    //  même adresse ou NULL
    if (chaine1 == chaine2) {
        return 0;  // Même chaîne ou toutes les deux NULL
    }
    if (chaine1 == NULL) {
        return -1; // NULL est plus petit
    }
    if (chaine2 == NULL) {
        return 1;  // Toute chaîne non NULL est plus grande que NULL
    }
    
    // Utilisation de strcmp pour la comparaison
    return strcmp(chaine1, chaine2);
}

char* dupliquerChaine(char* source) {
    //On vérifie le paramètre d'entrée
    if (source == NULL) {
        return NULL;
    }
    
    // On calcule la longueur de la chaîne source
    size_t longueur = strlen(source);
    
    // On alloue la mémoire pour la copie (+1 pour le caractère nul)
    char* copie = malloc((longueur + 1) * sizeof(char));
    if (copie == NULL) {
        return NULL;  // Échec de l'allocation
    }
    
    // Copie du contenu de la chaîne source vers la nouvelle zone mémoire
    strcpy(copie, source);
    
    return copie;
}

// Retourne le maximum de deux entiers.
int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
