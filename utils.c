#define _POSIX_C_SOURCE 200809L
#include "utils.h"
#include <stdlib.h>
#include <string.h>

int comparerChaines(char* chaine1, char* chaine2) {
    if (chaine1 == chaine2) return 0;
    if (chaine1 == NULL) return -1;
    if (chaine2 == NULL) return 1;
    return strcmp(chaine1, chaine2);
}

char* dupliquerChaine(char* source) {
    if (source == NULL) return NULL;
    return strdup(source);
}
