#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avl_usines.h"
#include "avl_recherche.h"
#include "lecture_csv.h"
#include "utils.h"
#include "calculs.h"

char *generer_nom_fichier(char *prefixe) {
    time_t maintenant = time(NULL);  
    char *nom = malloc(100);        
    if (nom == NULL) {
        return NULL;
    }
    sprintf(nom, "%s_%ld.dat", prefixe, maintenant);
    return nom;
}

int main(int nombre_arguments, char* arguments[]) {
    if(nombre_arguments != 4) {
        printf("Usage: %s <csv> <histo|leaks> <argument>\n", arguments[0]);
        return 1;
    }
    
    char* fichier_csv = arguments[1];
    char* commande = arguments[2];
    char* argument = arguments[3];

    NoeudAVLUsine* avl_usines = NULL;
    NoeudAVLRecherche* avl_recherche = NULL;
    NoeudDistribution* racine_distribution = NULL;
    
    if(charger_csv(fichier_csv, &avl_usines, &avl_recherche) != 0) {
        return 1;
    }

    if (avl_recherche != NULL) {
        racine_distribution = avl_recherche->pointeur_noeud;
        while (racine_distribution != NULL && racine_distribution->parent != NULL) {
            racine_distribution = racine_distribution->parent;
        }
    }
    
    if(comparerChaines(commande, "histo") == 0) {
        int type = -1;
        
        if(comparerChaines(argument, "max") == 0) {
            type = 0;
        } else if(comparerChaines(argument, "src") == 0) {
            type = 1;
        } else if(comparerChaines(argument, "real") == 0) {
            type = 2;
        } else if(comparerChaines(argument, "all") == 0) {
            type = 3;
        }

        if(type != -1) {
            char* nom_fichier_sortie = NULL;
            if(type == 0) {
                nom_fichier_sortie = generer_nom_fichier("vol_max");
            } else if(type == 1) {
                nom_fichier_sortie = generer_nom_fichier("vol_captation");
            } else if(type == 2) {
                nom_fichier_sortie = generer_nom_fichier("vol_traitement");
            } else {
                nom_fichier_sortie = generer_nom_fichier("vol_all");
            }

            if(nom_fichier_sortie == NULL) {
                printf("Erreur: Impossible de generer le nom du fichier\n");
                libererAVLUsine(avl_usines);
                libererAVLRecherche(avl_recherche);
                return 1;
            }
        
            FILE* f = fopen(nom_fichier_sortie, "w");
            if(f == NULL) {
                printf("Erreur: Impossible d'ouvrir le fichier %s\n", nom_fichier_sortie);
                free(nom_fichier_sortie);
                libererAVLUsine(avl_usines);
                libererAVLRecherche(avl_recherche);
                return 1;
            }
        
            if(type == 0) {
                fprintf(f, "identifier;max volume (M.m3.year-1)\n");
            } else if(type == 1) {
                fprintf(f, "identifier;source volume (M.m3.year-1)\n");
            } else if(type == 2) {
                fprintf(f, "identifier;real volume (M.m3.year-1)\n");
            } 
            
            parcoursInverseAVLUsine(avl_usines, f, type);
            fclose(f);
            printf("FICHIER_GENERE:%s\n", nom_fichier_sortie);
            free(nom_fichier_sortie);
        } else {
            printf("Erreur: type d'histogramme invalide\n");
            libererAVLUsine(avl_usines);
            libererAVLRecherche(avl_recherche);
            return 1;
        }

    } else if(comparerChaines(commande, "leaks") == 0) {
        char* id_usine = argument;
        char* nom_fichier_sortie = generer_nom_fichier("leaks");
        
        if(nom_fichier_sortie == NULL) {
            printf("Erreur: Impossible de generer le nom du fichier\n");
            libererAVLUsine(avl_usines);
            libererAVLRecherche(avl_recherche);
            return 1;
        }
        
        FILE* f = fopen(nom_fichier_sortie, "w");          
        if (f == NULL) {
            printf("Erreur : Impossible d'ecrire dans %s\n", nom_fichier_sortie);
            free(nom_fichier_sortie);
            libererAVLUsine(avl_usines);
            libererAVLRecherche(avl_recherche);
            return 1;
        }
        
        fprintf(f, "identifier;Leak volume (M.m3.year-1)\n");
        
        NoeudDistribution* noeud_usine = rechercherNoeud(avl_recherche, id_usine);
        DonneesUsine* infos_usine = rechercherUsine(avl_usines, id_usine);

        if(noeud_usine != NULL && infos_usine != NULL) {
            double volume_depart = infos_usine->total_traite;
            
            if (volume_depart <= 0) {
                fprintf(f, "%s;0.00\n", id_usine);
                printf("Usine %s : volume de depart nul ou negatif\n", id_usine);
            } else {
                double fuites_totales = 0;
                
                if (noeud_usine->nb_enfants > 0) {
                    double volume_par_sortie = volume_depart / noeud_usine->nb_enfants;
                    
                    for(int i = 0; i < noeud_usine->nb_enfants; i++) {
                        fuites_totales += calculerFuites(noeud_usine->enfants[i], volume_par_sortie);
                    }
                }
                
                fprintf(f, "%s;%.2f\n", id_usine, fuites_totales / 1000.0); 
            }
        } else {
            fprintf(f, "%s;-1\n", id_usine);
            printf("Usine %s introuvable (valeur -1 enregistree)\n", id_usine);
        }
        
        fclose(f);
        printf("FICHIER_GENERE:%s\n", nom_fichier_sortie);
        free(nom_fichier_sortie);
        
    } else {
        printf("Erreur: Commande inconnue '%s'\n", commande);
        libererAVLUsine(avl_usines);
        libererAVLRecherche(avl_recherche);
        return 1;
    }
    
    libererAVLUsine(avl_usines);
    libererArbreDistribution(racine_distribution);
    libererAVLRecherche(avl_recherche);

    return 0;
}
