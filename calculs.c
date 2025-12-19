#include "calculs.h"
#include <stdlib.h> 

double calculerFuites(NoeudDistribution* noeud, double volume_arrivee) {
    if (noeud == NULL || volume_arrivee <= 0) {
        return 0;
    }
    
    // Si le noeud n'a pas d'enfants, pas de fuites supplémentaires
    if (noeud->nb_enfants == 0) {
        return 0;
    }

    double total_fuites = 0;
    
    // Calcul du volume par enfant 
    double volume_par_enfant = volume_arrivee / noeud->nb_enfants;

    // Parcours de tous les enfants
    for (int i = 0; i < noeud->nb_enfants; i++) {
        LienDistribution lien = noeud->enfants[i];
        
    // Calcul des fuites sur le tronçon actuel
        double fuite_troncon = volume_par_enfant * (lien.pourcentage_fuite / 100.0);
        
    // Calcul du volume restant après fuite
        double volume_restant = volume_par_enfant - fuite_troncon;
        
    // Ajout des fuites du tronçon actuel
        total_fuites += fuite_troncon;
        
    // Calcul récursif des fuites en aval
        if (volume_restant > 0) {
            total_fuites += calculerFuites(lien.enfant, volume_restant);
        }
    }

    return total_fuites;
}
