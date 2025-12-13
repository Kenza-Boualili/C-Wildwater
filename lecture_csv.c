#include "lecture_csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int contientMot(char* chaine, char* sous_chaine) {
    if(chaine == NULL || sous_chaine == NULL) {
        return 0;
    }
    
    int i = 0;
    while(chaine[i] != '\0') {
        int j = 0;
        int tmp = 1;
        
        while(sous_chaine[j] != '\0') {
            if(chaine[i + j] == '\0') {
                tmp = 0;
                break;
            }
            if(chaine[i + j] != sous_chaine[j]) {
                tmp = 0;
                break;
            }
            j++;
        }
        
        if(tmp == 1) {
            return 1;
        }
        i++;
    }
    return 0;
}

TypeNoeud deduireType(char* id) {
    if(contientMot(id, "Spring")) {
        return NOEUD_SOURCE;
    }
    if(contientMot(id, "Facility")) {
        return NOEUD_USINE;
    }
    if(contientMot(id, "Storage")) {
        return NOEUD_STOCKAGE;
    }
    if(contientMot(id, "Junction")) {
        return NOEUD_JONCTION;
    }
    if(contientMot(id, "Service")) {
        return NOEUD_SERVICE;
    }
    if(contientMot(id, "Cust")) {
        return NOEUD_CLIENT;
    }
    return NOEUD_JONCTION;
}

NoeudDistribution* creerNoeudDistribution(char* id) {
    NoeudDistribution* n = malloc(sizeof(NoeudDistribution));
    if(n == NULL) {
        return NULL;
    }

    n->identifiant = dupliquerChaine(id);
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
    if(parent == NULL || enfant == NULL) {
        return;
    }

    if(parent->nb_enfants >= parent->capacite_enfants) {
        int nouvelle_cap;
        if(parent->capacite_enfants == 0) {
            nouvelle_cap = 5;
        } else {
            nouvelle_cap = parent->capacite_enfants * 2;
        }
        
        NoeudDistribution** nouveau_tableau = malloc(nouvelle_cap * sizeof(NoeudDistribution*));
        if(nouveau_tableau == NULL) {
            printf("Erreur memoire ajout enfant\n");
            return;
        }
        
        int i;
        for(i = 0; i < parent->nb_enfants; i++) {
            nouveau_tableau[i] = parent->enfants[i];
        }
        
        if(parent->enfants != NULL) {
            free(parent->enfants);
        }
        
        parent->enfants = nouveau_tableau;
        parent->capacite_enfants = nouvelle_cap;
    }

    parent->enfants[parent->nb_enfants] = enfant;
    parent->nb_enfants++;
    enfant->parent = parent;
}

LigneCSV* lire_ligne_csv(char* ligne) {
    if(ligne == NULL) {
        return NULL;
    }
    
    LigneCSV* resultat = malloc(sizeof(LigneCSV));
    if(resultat == NULL) {
        return NULL;
    }
    
    resultat->usine_traitement = NULL;
    resultat->id_amont = NULL;
    resultat->id_aval = NULL;
    resultat->volume = -1;
    resultat->pourcentage_fuite = -1;
    
    char* copie = dupliquerChaine(ligne);
    if(copie == NULL) {
        free(resultat);
        return NULL;
    }
    
    char* tmp = copie;
    int colonne = 0;
    
    for(int i = 0; copie[i] != '\0'; i++) {
        if(copie[i] == ';') {
            copie[i] = '\0';
            
            if(colonne == 0) {
                if(comparerChaines(tmp, "-") != 0) {
                resultat->usine_traitement = dupliquerChaine(tmp);
                }
            } else if(colonne == 1) {
                if(comparerChaines(tmp, "-") != 0) {
                resultat->id_amont = dupliquerChaine(tmp);
                 }
            } else if(colonne == 2) {
                if(comparerChaines(tmp, "-") != 0) { 
                resultat->id_aval = dupliquerChaine(tmp);
                }
            } else if(colonne == 3) {
                if(comparerChaines(tmp, "-") != 0) {
                    resultat->volume = atof(tmp);
                }
            } else if(colonne == 4) {
                if(comparerChaines(tmp, "-") != 0) {
                    resultat->pourcentage_fuite = atof(tmp);
                }
            }
            
            tmp = &copie[i + 1];
            colonne++;
        }
    }
    
    free(copie);
    return resultat;
}

void liberer_ligne_csv(LigneCSV* ligne) {
    if(ligne == NULL) {
        return;
    }
    
    if(ligne->usine_traitement != NULL) {
        free(ligne->usine_traitement);
    }
    if(ligne->id_amont != NULL) {
        free(ligne->id_amont);
    }
    if(ligne->id_aval != NULL) {
        free(ligne->id_aval);
    }
    
    free(ligne);
}

int charger_csv(char* nom_fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche) {
    FILE* fichier = fopen(nom_fichier, "r");
    if(fichier == NULL) {
        printf("Erreur: Impossible d'ouvrir %s\n", nom_fichier);
        return 1;
    }
    
    char buffer[1024];
    int lignes_lues = 0;
    
    while(fgets(buffer, sizeof(buffer), fichier)) {
        lignes_lues++;
        
        int i = 0;
        while(buffer[i] != '\0' && buffer[i] != '\n') {
            i++;
        }
        buffer[i] = '\0';
        
        if(i == 0 || contientMot(buffer, "Source")) {
            continue;
        }

        LigneCSV* ligne = lire_ligne_csv(buffer);
        if(ligne == NULL) {
            continue;
        }
        
        if(ligne->id_aval == NULL || comparerChaines(ligne->id_aval, "-") == 0) {
            if(ligne->volume != -1) {
                DonneesUsine* u = (DonneesUsine*)malloc(sizeof(DonneesUsine));
                u->identifiant = dupliquerChaine(ligne->id_amont);
                u->capacite_max = ligne->volume;
                u->total_capte = 0;
                u->total_traite = 0;
                *avl_usines = insererAVLUsine(*avl_usines, u);
            }

            if(rechercherNoeud(*avl_recherche, ligne->id_amont) == NULL) {
                NoeudDistribution* n = creerNoeudDistribution(ligne->id_amont);
                *avl_recherche = insererAVLRecherche(*avl_recherche, ligne->id_amont, n);
            }
        } else {
            NoeudDistribution* parent = rechercherNoeud(*avl_recherche, ligne->id_amont);
            if(parent == NULL) {
                parent = creerNoeudDistribution(ligne->id_amont);
                *avl_recherche = insererAVLRecherche(*avl_recherche, ligne->id_amont, parent);
            }

            NoeudDistribution* enfant = rechercherNoeud(*avl_recherche, ligne->id_aval);
            if(enfant == NULL) {
                enfant = creerNoeudDistribution(ligne->id_aval);
                *avl_recherche = insererAVLRecherche(*avl_recherche, ligne->id_aval, enfant);
            }

            ajouterEnfant(parent, enfant);
            
            if(ligne->pourcentage_fuite != -1) {
                enfant->pourcentage_fuite = ligne->pourcentage_fuite;
            }
            
            if(parent->type == NOEUD_SOURCE && enfant->type == NOEUD_USINE) {
                if(ligne->volume != -1) {
                    DonneesUsine* donneesTmp = malloc(sizeof(DonneesUsine));
                    if(donneesTmp == NULL) {
                        liberer_ligne_csv(ligne);
                        continue;
                    }
                    
                    donneesTmp->identifiant = dupliquerChaine(enfant->identifiant);
                    donneesTmp->capacite_max = 0;
                    donneesTmp->total_capte = ligne->volume;
                    
                    double pourcentage;
                    if(ligne->pourcentage_fuite != -1) {
                        pourcentage = ligne->pourcentage_fuite;
                    } else {
                        pourcentage = 0;
                    }
                    donneesTmp->total_traite = ligne->volume * (1.0 - (pourcentage / 100.0));
                    
                    *avl_usines = insererAVLUsine(*avl_usines, donneesTmp);
                }
            }
        }
        
        liberer_ligne_csv(ligne);
        
        if(lignes_lues % 100000 == 0) {
            printf("Lignes lues: %d\n", lignes_lues);
        }
    }
    
    fclose(fichier);
    printf("CSV charge: %d lignes traitees avec succes.\n", lignes_lues);
    return 0;
}
