#include "lecture_csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Déduction de type optimisée
TypeNoeud deduireType(char* id) {
    if (id == NULL) return NOEUD_JONCTION;
    switch (id[0]) {
        case 'S':
            if (strncmp(id, "Spring", 6) == 0) return NOEUD_SOURCE;
            if (strncmp(id, "Service", 7) == 0) return NOEUD_SERVICE;
            if (strncmp(id, "Storage", 7) == 0) return NOEUD_STOCKAGE;
            break;
        case 'W': if (strncmp(id, "Well", 4) == 0) return NOEUD_SOURCE; break;
        case 'F':
            if (strncmp(id, "Facility", 8) == 0) return NOEUD_USINE;
            if (strncmp(id, "Fountain", 8) == 0) return NOEUD_SOURCE;
            break;
        case 'R': if (strncmp(id, "Resurgence", 10) == 0) return NOEUD_SOURCE; break;
        case 'U': if (strncmp(id, "Unit", 4) == 0) return NOEUD_USINE; break;
        case 'P': if (strncmp(id, "Plant", 5) == 0) return NOEUD_USINE; break;
        case 'M': if (strncmp(id, "Module", 6) == 0) return NOEUD_USINE; break;
        case 'J': if (strncmp(id, "Junction", 8) == 0) return NOEUD_JONCTION; break;
        case 'C': if (strncmp(id, "Cust", 4) == 0) return NOEUD_CLIENT; break;
    }
    return NOEUD_JONCTION;
}

NoeudDistribution* creerNoeudDistribution(char* id) {
    NoeudDistribution* n = malloc(sizeof(NoeudDistribution));
    if (!n) return NULL;
    n->identifiant = dupliquerChaine(id);
    n->type = deduireType(id);
    n->nb_enfants = 0; n->capacite_enfants = 0; n->enfants = NULL;
    n->volume_entrant = 0; n->pourcentage_fuite = 0;
    return n;
}

void ajouterEnfant(NoeudDistribution* parent, NoeudDistribution* enfant) {
    if (!parent || !enfant) return;
    if (parent->nb_enfants >= parent->capacite_enfants) {
        int n_cap = (parent->capacite_enfants == 0) ? 5 : parent->capacite_enfants * 2;
        NoeudDistribution** n_tab = realloc(parent->enfants, n_cap * sizeof(NoeudDistribution*));
        if (!n_tab) return;
        parent->enfants = n_tab;
        parent->capacite_enfants = n_cap;
    }
    parent->enfants[parent->nb_enfants++] = enfant;
    enfant->parent = parent;
}

int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (!fichier) return -1;

    char buffer[2048];
    while (fgets(buffer, sizeof(buffer), fichier)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        if (buffer[0] == '\0') continue;

        char *champs[5] = {NULL, NULL, NULL, NULL, NULL};
        char *ptr = buffer;
        int col = 0;
        champs[col++] = ptr;

        while (*ptr && col < 5) {
            if (*ptr == ';') { *ptr = '\0'; champs[col++] = ptr + 1; }
            ptr++;
        }

        if (col < 2 || !champs[1]) continue;

        char* id_amont = (champs[1][0] == '-') ? NULL : champs[1];
        char* id_aval = (col > 2 && champs[2] && champs[2][0] != '-') ? champs[2] : NULL;
        double vol = (col > 3 && champs[3] && champs[3][0] != '-') ? atof(champs[3]) : -1.0;
        double fkt = (col > 4 && champs[4] && champs[4][0] != '-') ? atof(champs[4]) : -1.0;

        if (id_amont && !id_aval) {
            if (vol != -1.0) {
                DonneesUsine* u = malloc(sizeof(DonneesUsine));
                if (u) {
                    u->identifiant = dupliquerChaine(id_amont);
                    u->capacite_max = vol; u->total_capte = 0; u->total_traite = 0;
                    *avl_usines = insererAVLUsine(*avl_usines, u);
                }
            }
            if (!rechercherNoeud(*avl_recherche, id_amont))
                *avl_recherche = insererAVLRecherche(*avl_recherche, id_amont, creerNoeudDistribution(id_amont));
        } else if (id_amont && id_aval) {
            NoeudDistribution* p = rechercherNoeud(*avl_recherche, id_amont);
            if (!p) { p = creerNoeudDistribution(id_amont); *avl_recherche = insererAVLRecherche(*avl_recherche, id_amont, p); }
            NoeudDistribution* e = rechercherNoeud(*avl_recherche, id_aval);
            if (!e) { e = creerNoeudDistribution(id_aval); *avl_recherche = insererAVLRecherche(*avl_recherche, id_aval, e); }
            ajouterEnfant(p, e);
            if (fkt != -1.0) e->pourcentage_fuite = fkt;

            if (p->type == NOEUD_SOURCE && e->type == NOEUD_USINE && vol != -1.0) {
                DonneesUsine* d = malloc(sizeof(DonneesUsine));
                if (d) {
                    d->identifiant = dupliquerChaine(e->identifiant);
                    d->capacite_max = 0; d->total_capte = vol;
                    double perte = (fkt != -1.0) ? fkt : 0;
                    d->total_traite = vol * (1.0 - (perte / 100.0));
                    *avl_usines = insererAVLUsine(*avl_usines, d);
                }
            }
        }
    }
    fclose(fichier);
    return 0;
}
