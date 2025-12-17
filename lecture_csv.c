#include "lecture_csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir %s\n", nom_fichier);
        return -1;
    }

    char buffer[1024];
    // Lecture ligne par ligne du fichier CSV [cite: 13]
    while (fgets(buffer, sizeof(buffer), fichier)) {
        buffer[strcspn(buffer, "\r\n")] = 0; // Nettoyage des retours à la ligne
        if (buffer[0] == '\0') continue;

        // Tableau de pointeurs vers chaque colonne (initialisé à NULL pour la sécurité) [cite: 13]
        char *champs[5] = {NULL, NULL, NULL, NULL, NULL};
        char *ptr = buffer;
        int col = 0;
        champs[col++] = ptr;

        // Découpage "in-place" : on remplace les ';' par des '\0' pour isoler les chaînes [cite: 13]
        while (*ptr && col < 5) {
            if (*ptr == ';') {
                *ptr = '\0';
                champs[col++] = ptr + 1;
            }
            ptr++;
        }

        // Sécurité : Si l'ID amont (colonne 2) est absent, on ignore la ligne [cite: 13]
        if (col < 2 || champs[1] == NULL) continue;

        // Analyse du contenu des champs (gestion des tirets '-') [cite: 13]
        char* id_amont = (champs[1][0] == '-') ? NULL : champs[1];
        char* id_aval = (col > 2 && champs[2] != NULL && champs[2][0] != '-') ? champs[2] : NULL;
        double volume = (col > 3 && champs[3] != NULL && champs[3][0] != '-') ? atof(champs[3]) : -1.0;
        double fuite = (col > 4 && champs[4] != NULL && champs[4][0] != '-') ? atof(champs[4]) : -1.0;

        // Cas 1 : Ligne de définition d'une USINE seule [cite: 13]
        if (id_amont != NULL && id_aval == NULL) {
            if (volume != -1.0) {
                DonneesUsine* u = malloc(sizeof(DonneesUsine));
                u->identifiant = dupliquerChaine(id_amont);
                u->capacite_max = volume;
                u->total_capte = 0; 
                u->total_traite = 0;
                *avl_usines = insererAVLUsine(*avl_usines, u);
            }
            // Ajout au dictionnaire de recherche si inexistant [cite: 13]
            if (rechercherNoeud(*avl_recherche, id_amont) == NULL) {
                *avl_recherche = insererAVLRecherche(*avl_recherche, id_amont, creerNoeudDistribution(id_amont));
            }
        } 
        // Cas 2 : Ligne de tronçon (AMONT -> AVAL) [cite: 13]
        else if (id_amont != NULL && id_aval != NULL) {
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
            if (fuite != -1.0) enfant->pourcentage_fuite = fuite;

            // Calcul spécifique pour les histogrammes Usines (Source -> Usine) [cite: 13]
            if (parent->type == NOEUD_SOURCE && enfant->type == NOEUD_USINE && volume != -1.0) {
                DonneesUsine* d = malloc(sizeof(DonneesUsine));
                d->identifiant = dupliquerChaine(enfant->identifiant);
                d->capacite_max = 0; 
                d->total_capte = volume;
                double p = (fuite != -1.0) ? fuite : 0;
                d->total_traite = volume * (1.0 - (p / 100.0));
                *avl_usines = insererAVLUsine(*avl_usines, d);
            }
        }
    }
    fclose(fichier);
    return 0;
}
