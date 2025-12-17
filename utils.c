#include "utils.h"
#include <stdlib.h>
#include <string.h>

int comparerChaines(char* chaine1, char* chaine2) {
    if (chaine1 == chaine2) return 0;
    if (!chaine1) return -1;
    if (!chaine2) return 1;
    return strcmp(chaine1, chaine2);
}

char* dupliquerChaine(char* source) {
    if (!source) return NULL;
    return strdup(source);
}

int max(int a, int b) {
    return (a > b) ? a : b;
}
