#include "utils.h"
#include <stdlib.h>

int comparerChaines(char* chaine1, char* chaine2) {
    if (chaine1 == NULL && chaine2 == NULL){
        return 0;
    }
    if (chaine1 == NULL){
        return -1;
    }
    if (chaine2 == NULL){
        return 1;
    }
    int i = 0;
    while (chaine1[i] != '\0' && chaine2[i] != '\0') {
        if (chaine1[i] < chaine2[i]) {
            return -1;  
        } else if (chaine1[i] > chaine2[i]) {
            return 1;   
        }
        i++; 
    }
    if (chaine1[i] == '\0' && chaine2[i] == '\0') {
        return 0; 
    } else if (chaine1[i] == '\0') {
        return -1; 
    } else {
        return 1;
    }
}

char* dupliquerChaine(char* source) {
    if (source == NULL) {
        return NULL;
    }
    
    int longueur = 0;
    while (source[longueur] != '\0') {
        longueur++;
    }
    char* copie = malloc((longueur + 1) * sizeof(char));
    if (copie == NULL) {
        return NULL;
    }
    for (int i = 0; i <= longueur; i++) {
        copie[i] = source[i];
    }    
    return copie;
}


int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
