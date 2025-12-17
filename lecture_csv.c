#include "lecture_csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Optimisation : Identification par premier caractère ou strncmp (plus rapide que strstr)
TypeNoeud deduireType(char* id) {
    if (id == NULL) return NOEUD_JONCTION;
    switch (id[0]) {
        case 'S':
            if (strncmp(id, "Spring", 6) == 0) return NOEUD_SOURCE;
            if (strncmp(id, "Service", 7) == 0) return NOEUD_SERVICE;
            if (strncmp(id, "Storage", 7) == 0) return NOEUD_STOCKAGE;
            break;
        case 'W':
            if (strncmp(id, "Well", 4) == 0) return NOEUD_SOURCE;
            break;
        case 'F':
            if (strncmp(id, "Facility", 8) == 0) return NOEUD_USINE;
            if (strncmp(id, "Fountain", 8) == 0) return NOEUD_SOURCE;
            break;
        case 'R':
            if (strncmp(id, "Resurgence", 10) == 0) return NOEUD_SOURCE;
            break;
        case 'U':
            if (strncmp(id, "Unit", 4) == 0) return NOEUD_USINE;
            break;
        case 'P':
            if (strncmp(id, "Plant", 5) == 0) return NOEUD_USINE;
            break;
        case 'M':
            if (strncmp(id, "Module", 6) == 0) return NOEUD_USINE;
            break;
        case 'J':
            if (strncmp(id, "Junction", 8) == 0) return NOEUD_JONCTION;
            break;
        case 'C':
            if (strncmp(id, "Cust", 4) == 0) return NOEUD_CLIENT;
            break;
    }
    return NOEUD_JONCTION;
}

// Version optimisée sans allocation de structure LigneCSV intermédiaire
int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) return -1;

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fichier)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        if (buffer[0] == '\0') continue;

        char *champs[5] = {NULL};
        char *ptr = buffer;
        int col = 0;
        champs[col++] = ptr;

        // Parsing manuel rapide (remplace strtok ou lire_ligne_csv complexe)
        while (*ptr && col < 5) {
            if (*ptr == ';') {
                *ptr = '\0';
                champs[col++] = ptr + 1;
            }
            ptr++;
        }

        char* u_traitement = (champs[0][0] == '-') ? NULL : champs[0];
        char* id_amont = (champs[1][0] == '-') ? NULL : champs[1];
        char* id_aval = (champs[2][0] == '-') ? NULL : champs[2];
        double volume = (champs[3][0] == '-') ? -1.0 : atof(champs[3]);
        double fuite = (champs[4][0] == '-') ? -1.0 : atof(champs[4]);

        if (id_amont && !id_aval) { // Ligne Usine (Définition)
            if (volume != -1) {
                DonneesUsine* u = malloc(sizeof(DonneesUsine));
                u->identifiant = dupliquerChaine(id_amont);
                u->capacite_max = volume;
                u->total_capte = 0; u->total_traite = 0;
                *avl_usines = insererAVLUsine(*avl_usines, u);
            }
            if (rechercherNoeud(*avl_recherche, id_amont) == NULL) {
                *avl_recherche = insererAVLRecherche(*avl_recherche, id_amont, creerNoeudDistribution(id_amont));
            }
        } else if (id_amont && id_aval) { // Tronçon
            NoeudDistribution* parent = rechercherNoeud(*avl_recherche, id_amont);
            if (!parent) {
                parent = creerNoeudDistribution(id_amont);
                *avl_recherche = insererAVLRecherche(*avl_recherche, id_amont, parent);
            }
            NoeudDistribution* enfant = rechercherNoeud(*avl_recherche, id_aval);
            if (!enfant) {
                enfant = creerNoeudDistribution(id_aval);
                *avl_recherche = insererAVLRecherche(*avl_recherche, id_aval, enfant);
            }
            ajouterEnfant(parent, enfant);
            if (fuite != -1) enfant->pourcentage_fuite = fuite;

            // Mise à jour rapide des volumes captés/traités pour les usines
            if (parent->type == NOEUD_SOURCE && enfant->type == NOEUD_USINE && volume != -1) {
                DonneesUsine* d = malloc(sizeof(DonneesUsine));
                d->identifiant = dupliquerChaine(enfant->identifiant);
                d->capacite_max = 0; d->total_capte = volume;
                double p = (fuite != -1) ? fuite : 0;
                d->total_traite = volume * (1.0 - (p / 100.0));
                *avl_usines = insererAVLUsine(*avl_usines, d);
            }
        }
    }
    fclose(fichier);
    return 0;
}
