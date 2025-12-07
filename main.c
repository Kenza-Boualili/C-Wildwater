#include <stdio.h>
#include <stdlib.h>
#include "avl_usines.h"
#include "avl_recherche.h"
#include "lecture_csv.h"
#include "utils.h"
#include "calculs.h"

int main(int nombre_arguments, char* arguments[]) {
    if(nombre_arguments < 4) {
        printf("Usage: %s <commande> <argument> <fichier_csv>\n", arguments[0]);
        return 1;
    }

    char* commande = arguments[1];
    char* argument = arguments[2];
    char* fichier_csv = arguments[3];

    NoeudAVLUsine* avl_usines = NULL;
    NoeudAVLRecherche* avl_recherche = NULL;

    if(charger_csv(fichier_csv, &avl_usines, &avl_recherche) != 0) {
        return 1;
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
            char* nom_fichier_sortie;
            
            if(type == 0) {
                nom_fichier_sortie = "vol_max.dat";
            } else if(type == 1) {
                nom_fichier_sortie = "vol_captation.dat";
            } else if(type == 2) {
                nom_fichier_sortie = "vol_traitement.dat";
            } else {
                nom_fichier_sortie = "vol_all.dat";
            }

            FILE* f = fopen(nom_fichier_sortie, "w");
            if(f != NULL) {
                if(type == 0) {
                    fprintf(f, "identifier;max volume (k.m3/an)\n");
                } else if(type == 1) {
                    fprintf(f, "identifier;source volume (k.m3/an)\n");
                } else if(type == 2) {
                    fprintf(f, "identifier;real volume (k.m3/an)\n");
                } else {
                    fprintf(f, "identifier;capacity;source;treated\n");
                }

                parcoursInverseAVLUsine(avl_usines, f, type);
                fclose(f);
            }
        }
    } else if(comparerChaines(commande, "leaks") == 0) {
        char* id_usine = argument;
        
        NoeudDistribution* noeud_usine = rechercherNoeud(avl_recherche, id_usine);
        DonneesUsine* infos_usine = rechercherUsine(avl_usines, id_usine);

        if(noeud_usine != NULL && infos_usine != NULL) {
            double volume_depart = infos_usine->total_traite;
            double fuites_totales = 0;
            
            double volume_par_sortie = 0;
            if(noeud_usine->nb_enfants > 0) {
                volume_par_sortie = volume_depart / noeud_usine->nb_enfants;
            }
            
            for(int i=0; i<noeud_usine->nb_enfants; i++) {
                fuites_totales += calculerFuites(noeud_usine->enfants[i], volume_par_sortie);
            }

            printf("%.2f", fuites_totales); 
        } else {
            printf("Usine %s introuvable\n", id_usine);
            libererAVLUsine(avl_usines);
            libererAVLRecherche(avl_recherche);
            return 1;
        }
    }

    libererAVLUsine(avl_usines);
    libererAVLRecherche(avl_recherche);

    return 0;
}
