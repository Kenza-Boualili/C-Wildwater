#include "calculs.h"
#include <stdlib.h> 

double calculerFuites(NoeudDistribution* noeud, double volume_arrivee) {
    if (noeud == NULL || volume_arrivee <= 0){
        return 0;
    } 

    double pourcentage = noeud->pourcentage_fuite;
    if (pourcentage < 0) {
        pourcentage = 0;  
    }
    if (pourcentage > 100) {
        pourcentage = 100; 
    }
    
    double fuite_ici = 0;
    if (pourcentage > 0) {
        fuite_ici = volume_arrivee * (pourcentage / 100.0);
    }
    double volume_restant = volume_arrivee - fuite_ici;
    if (volume_restant < 0) {
        volume_restant = 0;
    }
    if (noeud->nb_enfants == 0 || volume_restant == 0) {
        return fuite_ici;
    }
    double volume_par_enfant = volume_restant / noeud->nb_enfants;
    double total_fuites_enfants = 0;
    for (int i = 0; i < noeud->nb_enfants; i++) {
        total_fuites_enfants += calculerFuites(noeud->enfants[i], volume_par_enfant);
    }
    
    return fuite_ici + total_fuites_enfants;
}
