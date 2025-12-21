#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "avl_usines.h"
#include "avl_recherche.h"
#include "lecture_csv.h"
#include "utils.h"
#include "calculs.h"

// Génère un nom de fichier unique basé sur le temps
char* generer_nom_fichier(char* prefixe) {
    time_t maintenant = time(NULL);
    char* nom = malloc(100);
    if (nom == NULL) {
        return NULL;
    }
    sprintf(nom, "%s_%ld.dat", prefixe, maintenant);
    return nom;
}

// Convertit le type d'histogramme en valeur numérique
int get_type_histo(char* argument) {
    if (argument == NULL) {
        return -1;
    }

    switch(argument[0]) {
     case 'm': // max
    if (strcmp(argument, "max") == 0) {
     return 0;
    }
    break;
    case 's': // src
    if (strcmp(argument, "src") == 0) {
    return 1;
    }
     break;
    case 'r': // real
    if (strcmp(argument, "real") == 0) {
    return 2;
    }
     break;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    // Vérification du nombre d'arguments
    if (argc != 4) {
    printf("Usage: %s <csv> <histo|leaks> <argument>\n", argv[0]);
    return 1;
    }

    char* fichier_arg = argv[1]; // Nom du fichier ou "-" pour le pipe
    char* commande    = argv[2]; // histo ou leaks
    char* argument    = argv[3]; // type d'histo ou ID usine

    FILE* flux_entree = NULL;
    
    // Initialisation du flux d'entrée (Pipe ou Fichier)
    if (strcmp(fichier_arg, "-") == 0) {
    flux_entree = stdin; // Lecture directe depuis le pipe de notre script shell
    } else {
    flux_entree = fopen(fichier_arg, "r");
    }
    if (flux_entree == NULL) {
    fprintf(stderr, "Erreur: impossible d'ouvrir le flux d'entrée %s\n", fichier_arg);
    return 1;
    }

    NoeudAVLUsine* avl_usines = NULL;
    NoeudAVLRecherche* avl_recherche = NULL;

    // Chargement des données avec le flux ouvert et la commande pour optimisation
    if (charger_csv(flux_entree, &avl_usines, &avl_recherche, commande) != 0) {
    fprintf(stderr, "Erreur lors du chargement des données\n");
    if (flux_entree != stdin) {
    fclose(flux_entree);
    }
    return 1;
}

    // On ferme le fichier seulement si il a été ouvert par fopen
    if (flux_entree != stdin) {
    fclose(flux_entree);
    }

    // Traitement de la commande "histo"
    if (strcmp(commande, "histo") == 0) {
    int type = get_type_histo(argument);
    char* prefixe = NULL;
    char* entete = NULL;
        
        switch(type) {
        case 0: // max
        prefixe = "vol_max";
        entete = "identifier;max volume (k.m3.year-1)\n";
        break;
         case 1: // src
        prefixe = "vol_captation";
        entete = "identifier;source volume (k.m3.year-1);capacity_ref\n";
        break;
        case 2: // real
        prefixe = "vol_traitement";
        entete = "identifier;real volume (k.m3.year-1);capacity_ref\n";
        break;
        default:
        printf("Erreur: type d'histogramme invalide\n");
        libererAVLUsine(avl_usines);
        libererAVLRecherche(avl_recherche);
        return 1;
        }

        char* nom_fichier = generer_nom_fichier(prefixe);
        FILE* f = fopen(nom_fichier, "w");
        if (f != NULL) {
            fprintf(f, "%s", entete);
            parcoursInverseAVLUsine(avl_usines, f, type);
            fclose(f);
            printf("FICHIER_GENERE:%s\n", nom_fichier);
        }
        free(nom_fichier);
    }

    // Traitement de la commande "leaks"
    else if (strcmp(commande, "leaks") == 0) {
        // nom de fichier fixe pour l'historique 
        char* nom_fichier = "leaks.dat"; 

        // vérification de l'existence du fichier pour l'en-tête 
        FILE* test_f = fopen(nom_fichier, "r");
        int existe_deja = (test_f != NULL);
        if (existe_deja) {
            fclose(test_f);
        }

        // ouverture en mode "a" pour ajouter à la fin 
        FILE* f = fopen(nom_fichier, "a");
        if (f == NULL) {
            printf("Erreur ouverture fichier historique %s\n", nom_fichier);
            libererAVLUsine(avl_usines);
            libererAVLRecherche(avl_recherche);
            return 1;
        }

        // ecriture de l'en-tête seulement si le fichier est nouveau
        if (!existe_deja) {
            fprintf(f, "identifier;Leak volume (M.m3.year-1)\n");
        }

        // recherche et calcul des fuites
        // recherche efficace via l'AVL de recherche 
        NoeudDistribution* noeud_usine = rechercherNoeud(avl_recherche, argument);
        DonneesUsine* infos_usine = rechercherUsine(avl_usines, argument);
        
        if (noeud_usine == NULL || infos_usine == NULL) {
            // si l'usine n'est pas trouvée, on écrit -1 
            fprintf(f, "%s;-1\n", argument);
        } else {
            double volume_depart = infos_usine->total_traite;
            double fuites_totales = 0.0;

            if (volume_depart > 0 && noeud_usine->nb_enfants > 0) {

                // Répartition équitable entre les enfants 
            double volume_par_sortie = volume_depart / noeud_usine->nb_enfants;

            for (int i = 0; i < noeud_usine->nb_enfants; i++) {
            LienDistribution lien = noeud_usine->enfants[i];
            double fuite_troncon = volume_par_sortie * (lien.pourcentage_fuite / 100.0);
                    
                    // Somme des pertes locales et aval 
            fuites_totales += fuite_troncon;
            double volume_restant = volume_par_sortie - fuite_troncon;
            if (volume_restant > 0) {
            fuites_totales += calculerFuites(lien.enfant, volume_restant);
            }
        }
    }
            // Écriture du résultat (conversion k.m3 -> M.m3) 
        fprintf(f, "%s;%.2f\n", argument, fuites_totales / 1000.0);
        }

        fclose(f);
        // Affichage pour le script Shell
        printf("FICHIER_GENERE:%s\n", nom_fichier);
    }
    
    libererAVLUsine(avl_usines);
    libererAVLRecherche(avl_recherche);
    
    return 0;
}

