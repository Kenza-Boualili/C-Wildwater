#include "lecture_csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction pour vérifier si une sous-chaine est présente
int contientMot(char* chaine, char* sous_chaine) {
    if (chaine == NULL || sous_chaine == NULL || sous_chaine[0] == '\0') {
        return 0;
    }
    return strstr(chaine, sous_chaine) != NULL;
}

// Fonction pour identifier tous les types du projet
TypeNoeud deduireType(char* id) {
    if (id == NULL || id[0] == '\0') {
        return NOEUD_JONCTION;
    }
    
    // On regarde le premier caractère pour décider plus rapidement
    switch(id[0]) {
        case 'S': // Spring, Source, Storage, Service
        if (id[1] == 'p' || (id[1] == 'o' && id[2] == 'u')) {
        return NOEUD_SOURCE;
        }
        if (id[1] == 't') {
        return NOEUD_STOCKAGE;
        }
        if (id[1] == 'e') {
        return NOEUD_SERVICE;
        }
        break;
        case 'F': // Facility, Fountain
        case 'U': // Unit
        case 'P': // Plant
        case 'M': // Module
        return NOEUD_USINE;
        case 'W': // Well
        case 'R': // Resurgence
         return NOEUD_SOURCE;
        case 'J': // Junction
         return NOEUD_JONCTION;
        case 'C': // Cust
        return NOEUD_CLIENT;
    }
    
    return NOEUD_JONCTION;
}

// On alloue et initialise un nouveau noeud de distribution
NoeudDistribution* creerNoeudDistribution(char* id) {
    NoeudDistribution* nouveau_noeud = malloc(sizeof(NoeudDistribution));
    if (nouveau_noeud == NULL) {
        return NULL;
    }

    // Duplication de l'identifiant (pour éviter les problèmes de pointeurs)
    nouveau_noeud->identifiant = dupliquerChaine(id);
    if (nouveau_noeud->identifiant == NULL) {
        free(nouveau_noeud);
        return NULL;
    }
    
    nouveau_noeud->type = deduireType(id);
    nouveau_noeud->volume_entrant = 0;  //Initialisation des données hydrauliques
    nouveau_noeud->pourcentage_fuite = 0;
    nouveau_noeud->volume_fuite = 0;
    nouveau_noeud->volume_sortant = 0;
    
    nouveau_noeud->parent = NULL; //Initialisation des relations
    nouveau_noeud->enfants = NULL;
    nouveau_noeud->nb_enfants = 0;
    nouveau_noeud->capacite_enfants = 0;

    return nouveau_noeud;
}

void ajouterEnfant(NoeudDistribution* parent, NoeudDistribution* enfant, double pourcentage_fuite){
    if (parent == NULL || enfant == NULL) {
        return;
    }

    //Vérification si le tableau d'enfants est plein
    if (parent->nb_enfants >= parent->capacite_enfants) {
    int nouvelle_capacite = (parent->capacite_enfants == 0) ? 4 : parent->capacite_enfants * 2;  

    LienDistribution* nouveau_tableau = realloc(parent->enfants, nouvelle_capacite * sizeof(LienDistribution));
    if (nouveau_tableau == NULL) {
    return;
    }

     parent->enfants = nouveau_tableau;
    parent->capacite_enfants = nouvelle_capacite;
    }

    parent->enfants[parent->nb_enfants].enfant = enfant;
    parent->enfants[parent->nb_enfants].pourcentage_fuite = pourcentage_fuite;
    parent->nb_enfants++;
    enfant->parent = parent;
}

// Fonction pour trouver la racine du réseau de distribution d'eau
NoeudDistribution* trouverRacine(NoeudAVLRecherche* avl) {
    if (avl == NULL) {
        return NULL;
    }

    NoeudDistribution* resultat_gauche = trouverRacine(avl->gauche);
    if (resultat_gauche != NULL) {
        return resultat_gauche;
    }

    if (avl->pointeur_noeud != NULL && avl->pointeur_noeud->type == NOEUD_SOURCE && avl->pointeur_noeud->parent == NULL) {
        return avl->pointeur_noeud;
    }

    return trouverRacine(avl->droit);
}


LigneCSV* lire_ligne_csv(char* ligne) {
    if (ligne == NULL || ligne[0] == '\0') {
        return NULL;
    }
    
    LigneCSV* resultat = malloc(sizeof(LigneCSV));
    if (resultat == NULL) {
        return NULL;
    }
    
    // Initialisation des valeurs
    resultat->usine_traitement = NULL;
    resultat->id_amont = NULL;
    resultat->id_aval = NULL;
    resultat->volume = -1;
    resultat->pourcentage_fuite = -1;
    
    char* copie = dupliquerChaine(ligne);
    char* pointeur_sauvegarde = copie;
    char* morceau;
    int colonne = 0;
    
    // Séparation par ';' en utilisant strtok
    // strtok parcourt la chaîne et "isole" chaque colonne en utilisant ';' comme délimiteur
    // La boucle s'arrête si on atteint la fin de la ligne ou si on a récupéré les 5 colonnes voulues
    while ((morceau = strtok(pointeur_sauvegarde, ";")) != NULL && colonne < 5) {
        if (colonne == 0) {
            pointeur_sauvegarde = NULL;
        }
        
        switch(colonne) {
        case 0: // usine_traitement
        if (strcmp(morceau, "-") != 0) {
        resultat->usine_traitement = dupliquerChaine(morceau);
        }
         break;
        case 1: // id_amont
        if (strcmp(morceau, "-") != 0) {
        resultat->id_amont = dupliquerChaine(morceau);
        }
        break;
        case 2: // id_aval
        if (strcmp(morceau, "-") != 0) {
        resultat->id_aval = dupliquerChaine(morceau);
        }
         break;
        case 3: // volume
        if (strcmp(morceau, "-") != 0) {
         resultat->volume = atof(morceau);
        }
        break;
        case 4: // pourcentage_fuite
        if (strcmp(morceau, "-") != 0) {
        resultat->pourcentage_fuite = atof(morceau);
        }
        break;
        }
        colonne++;
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

// Fonction de vérification et de débogage pour détecter les usines 
// dont la capacité maximale n'a pas été initialisée correctement
// Cette fonction parcourt l'arbre AVL des usines et affiche un avertissement
// pour chaque usine dont la capacité maximale est égale à 0.0
// (ce qui peut nous indiquer un problème dans le chargement des données)
void verifierUsinesSansCapacite(NoeudAVLUsine* racine) {
    if (racine == NULL) {
    return;
    }
    
    verifierUsinesSansCapacite(racine->gauche);
    
    if (racine->donnees->capacite_max == 0.0) {
    fprintf(stderr, "ATTENTION: Usine %s a capacité_max = 0.0\n", 
    racine->donnees->identifiant);
    }
    
    verifierUsinesSansCapacite(racine->droit);
}

int charger_csv(FILE* fichier, NoeudAVLUsine** avl_usines, NoeudAVLRecherche** avl_recherche, char* commande){
    if (fichier == NULL) {
        return -1;
    }

    // On identifie le mode pour éviter les chargements inutiles
    int mode_histo = (strcmp(commande, "histo") == 0);
    char tampon[8192];

    while (fgets(tampon, sizeof(tampon), fichier)) {
        // Nettoyage rapide du retour à la ligne
        tampon[strcspn(tampon, "\r\n")] = 0;
        
        if (tampon[0] == '\0' || tampon[0] == '#') continue;

        // strtok initialise le découpage sur la chaîne 'tampon'
        char *c_usine = strtok(tampon, ";");

        // Appels suivants : on passe NULL pour dire à strtok de continuer 
        // à partir de la position mémorisée lors de l'appel précédent.
        char *c_amont = strtok(NULL, ";");   // Récupère la 2ème colonne
        char *c_aval  = strtok(NULL, ";");   // Récupère la 3ème colonne
        char *c_vol   = strtok(NULL, ";");   // Récupère la 4ème colonne
        char *c_fuite = strtok(NULL, ";");   // Récupère la 5ème colonne

        // On traite les tirets comme des valeurs NULL
        if (c_usine && c_usine[0] == '-') {
        c_usine = NULL;
        }

        if (c_amont && c_amont[0] == '-') {
        c_amont = NULL;
        }

        if (c_aval && c_aval[0] == '-') {
        c_aval = NULL;
        }

        // cas 1: definition d'une usine
        if (c_aval == NULL && c_amont != NULL && c_vol && c_vol[0] != '-') {
            // On construit l'AVL de recherche que si c'est nécessaire (leaks)
            if (!mode_histo) {
                NoeudDistribution* noeud_usine = rechercherNoeud(*avl_recherche, c_amont);
                if (noeud_usine == NULL) {
                    noeud_usine = creerNoeudDistribution(c_amont);
                    *avl_recherche = insererAVLRecherche(*avl_recherche, c_amont, noeud_usine);
                }
                noeud_usine->type = NOEUD_USINE;
            }
            
            double val_vol = atof(c_vol);
            DonneesUsine* usine = rechercherUsine(*avl_usines, c_amont);
            if (usine == NULL) {
                usine = malloc(sizeof(DonneesUsine));
                usine->identifiant = dupliquerChaine(c_amont);
                usine->capacite_max = val_vol;
                usine->total_capte = 0.0;
                usine->total_traite = 0.0;
                *avl_usines = insererAVLUsine(*avl_usines, usine);
            } else {
                usine->capacite_max = val_vol;
            }
        }

        //cas 2: tronçon
        else if (c_amont != NULL && c_aval != NULL) {
            // en mode histo on ignore la création de l'arbre de distribution
            if (!mode_histo) {
                NoeudDistribution* n_amont = rechercherNoeud(*avl_recherche, c_amont);
                if (n_amont == NULL) {
                n_amont = creerNoeudDistribution(c_amont);
                *avl_recherche = insererAVLRecherche(*avl_recherche, c_amont, n_amont);
                }
                
                NoeudDistribution* n_aval = rechercherNoeud(*avl_recherche, c_aval);
                if (n_aval == NULL) {
                 n_aval = creerNoeudDistribution(c_aval);
                *avl_recherche = insererAVLRecherche(*avl_recherche, c_aval, n_aval);
                }
                
                double fuite = (c_fuite && c_fuite[0] != '-') ? atof(c_fuite) : 0.0;
                ajouterEnfant(n_amont, n_aval, fuite);
            }
            
            // sous cas: captage(source -> usine)
            // on traite le captage même en mode histo pour remplir avl_usines
            if (c_vol && c_vol[0] != '-') {
                TypeNoeud t_amont = deduireType(c_amont);
                if (t_amont == NOEUD_SOURCE && deduireType(c_aval) == NOEUD_USINE) {
                double v_capte = atof(c_vol);
                double pc_fuite = (c_fuite && c_fuite[0] != '-') ? atof(c_fuite) : 0.0;
                    
                DonneesUsine* usine = rechercherUsine(*avl_usines, c_aval);
                if (usine == NULL) {
                usine = malloc(sizeof(DonneesUsine));
                usine->identifiant = dupliquerChaine(c_aval);
                 usine->capacite_max = 0.0;
                usine->total_capte = v_capte;
                usine->total_traite = v_capte * (1.0 - pc_fuite / 100.0);
                *avl_usines = insererAVLUsine(*avl_usines, usine);
                } else {
                usine->total_capte += v_capte;
                usine->total_traite += (v_capte * (1.0 - pc_fuite / 100.0));
                }
            }
        }
     }
}
    
    return 0;
}
