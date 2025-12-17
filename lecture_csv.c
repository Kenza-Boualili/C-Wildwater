#include "lecture_csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction utilitaire pour vérifier si une sous-chaine est présente
int contientMot(char* chaine, char* sous_chaine) {
    if (chaine == NULL || sous_chaine == NULL || sous_chaine[0] == '\0') {
        return 0;
    }
    return strstr(chaine, sous_chaine) != NULL;
}

// Fonction corrigée pour identifier tous les types d'acteurs du projet
TypeNoeud deduireType(char* id) {
    // Identification des SOURCES (Spring, Well, Fountain, Resurgence, etc.)
    if (contientMot(id, "Spring") || contientMot(id, "Well") || contientMot(id, "Fountain") || contientMot(id, "Resurgence")) {
        return NOEUD_SOURCE;
    }
    
    // Identification des USINES (Reconnaît Facility, Unit, Plant, Module)
    if (contientMot(id, "Facility") || contientMot(id, "Unit") || contientMot(id, "Plant") || contientMot(id, "Module")) {
        return NOEUD_USINE;
    }
    
    // Identification des STOCKAGES
    if (contientMot(id, "Storage")) {
        return NOEUD_STOCKAGE;
    }
    
    // Identification des JONCTIONS
    if (contientMot(id, "Junction")) {
        return NOEUD_JONCTION;
    }
    
    // Identification des SERVICES (Raccordements)
    if (contientMot(id, "Service")) {
        return NOEUD_SERVICE;
    }
    
    // Identification des CLIENTS (Usagers)
    if (contientMot(id, "Cust")) {
        return NOEUD_CLIENT;
    }

    // Par défaut
    return NOEUD_JONCTION;
}

// Alloue et initialise un nouveau noeud de distribution
NoeudDistribution* creerNoeudDistribution(char* id) {
    NoeudDistribution* n = malloc(sizeof(NoeudDistribution));
    if (n == NULL) {
        return NULL;
    }

    n->identifiant = dupliquerChaine(id);
    if (n->identifiant == NULL) {
        free(n);
        return NULL;
    }
    
    n->type = deduireType(id);
    n->volume_entrant = 0;
    n->pourcentage_fuite = 0;
    n->volume_fuite = 0;
    n->volume_sortant = 0;
    
    n->parent = NULL;
    n->enfants = NULL;
    n->nb_enfants = 0;
    n->capacite_enfants = 0;

    return n;
}

void ajouterEnfant(NoeudDistribution* parent, NoeudDistribution* enfant) {
    if (parent == NULL || enfant == NULL) {
        return;
    }

    if (parent->nb_enfants >= parent->capacite_enfants) {
        int nouvelle_cap = (parent->capacite_enfants == 0) ? 5 : parent->capacite_enfants * 2;
        
        NoeudDistribution** nouveau_tableau = realloc(parent->enfants, nouvelle_cap * sizeof(NoeudDistribution*));
        if (nouveau_tableau == NULL) {
            return;
        }
        
        parent->enfants = nouveau_tableau;
        parent->capacite_enfants = nouvelle_cap;
    }

    parent->enfants[parent->nb_enfants] = enfant;
    parent->nb_enfants++;
    enfant->parent = parent;
}

LigneCSV* lire_ligne_csv(char* ligne) {
    if (ligne == NULL || ligne[0] == '\0') {
        return NULL;
    }
    
    LigneCSV* resultat = malloc(sizeof(LigneCSV));
    if (resultat == NULL) {
        return NULL;
    }
    
    resultat->usine_traitement = NULL;
    resultat->id_amont = NULL;
    resultat->id_aval = NULL;
    resultat->volume = -1;
    resultat->pourcentage_fuite = -1;
    
    char* copie = dupliquerChaine(ligne);
    char* champs[5] = {NULL};
    int colonne = 0;
    char* ptr = copie;
    char* debut = copie;

    while (*ptr != '\0' && colonne < 5) {
        if (*ptr == ';') {
            *ptr = '\0';
            champs[colonne++] = debut;
            debut = ptr + 1;
        }
        ptr++;
    }
    if (colonne < 5) {
        champs[colonne] = debut;
    }

    if (champs[0] && strcmp(champs[0], "-") != 0) {
        resultat->usine_traitement = dupliquerChaine(champs[0]);
    }
    if (champs[1] && strcmp(champs[1], "-") != 0) {
        resultat->id_amont = dupliquerChaine(champs[1]);
    }
    if (champs[2] && strcmp(champs[2], "-") != 0) {
        resultat->id_aval = dupliquerChaine(champs[2]);
    }
    if (champs[3] && strcmp(champs[3], "-") != 0) {
        resultat->volume = atof(champs[3]);
    }
    if (champs[4] && strcmp(champs[4], "-") != 0) {
        resultat->pourcentage_fuite = atof(champs[4]);
    }
    
    free(copie);
    return resultat;
}

void liberer_ligne_csv(LigneCSV* ligne) {
    if (ligne == NULL) {
        return;
    }
    free(ligne->usine_traitement);
    free(ligne->id_amont);
    free(ligne->id_aval);
    free(ligne);
}

int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche) {
    FILE* fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur: Impossible d'ouvrir %s\n", nom_fichier);
        return -1;
    }
    
    char buffer[8192];
    int lignes_traitees = 0;
    
    while (fgets(buffer, sizeof(buffer), fichier)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        
        if (buffer[0] == '\0') {
            continue;
        }

        LigneCSV* ligne = lire_ligne_csv(buffer);
        if (ligne == NULL) {
            continue;
        }

        // Cas d'une ligne de définition (Usine seule)
        if (ligne->id_aval == NULL) {
            if (ligne->volume != -1) {
                DonneesUsine* u = malloc(sizeof(DonneesUsine));
                u->identifiant = dupliquerChaine(ligne->id_amont);
                u->capacite_max = ligne->volume;
                u->total_capte = 0;
                u->total_traite = 0;
                *avl_usines = insererAVLUsine(*avl_usines, u);
            }

            if (rechercherNoeud(*avl_recherche, ligne->id_amont) == NULL) {
                NoeudDistribution* n = creerNoeudDistribution(ligne->id_amont);
                *avl_recherche = insererAVLRecherche(*avl_recherche, ligne->id_amont, n);
            }
        } 
        // Cas d'un tronçon (Lien amont -> aval)
        else {
            NoeudDistribution* parent = rechercherNoeud(*avl_recherche, ligne->id_amont);
            if (parent == NULL) {
                parent = creerNoeudDistribution(ligne->id_amont);
                *avl_recherche = insererAVLRecherche(*avl_recherche, ligne->id_amont, parent);
            }

            NoeudDistribution* enfant = rechercherNoeud(*avl_recherche, ligne->id_aval);
            if (enfant == NULL) {
                enfant = creerNoeudDistribution(ligne->id_aval);
                *avl_recherche = insererAVLRecherche(*avl_recherche, ligne->id_aval, enfant);
            }

            ajouterEnfant(parent, enfant);
            
            if (ligne->pourcentage_fuite != -1) {
                enfant->pourcentage_fuite = ligne->pourcentage_fuite;
            }

            // Calcul du volume capté (Source -> Usine)
            if (parent->type == NOEUD_SOURCE && enfant->type == NOEUD_USINE) {
                if (ligne->volume != -1) {
                    DonneesUsine* d = malloc(sizeof(DonneesUsine));
                    d->identifiant = dupliquerChaine(enfant->identifiant);
                    d->capacite_max = 0; 
                    d->total_capte = ligne->volume;
                    double p = (ligne->pourcentage_fuite != -1) ? ligne->pourcentage_fuite : 0;
                    d->total_traite = ligne->volume * (1.0 - (p / 100.0));
                    *avl_usines = insererAVLUsine(*avl_usines, d);
                }
            }
        }
        
        lignes_traitees++;
        liberer_ligne_csv(ligne);
    }
    
    fclose(fichier);
    return 0;
}

