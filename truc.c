/*
 * PROJET MAIN3 / METRO
 * Librairie gérant les "trucs". Qui sont soit des stations, soit des connexions entre des stations.
*/

#include <stdlib.h>

#include "truc.h"

/**
 * Fonction qui permet de créer un truc.
 * @param coord : Les coordonnées du truc.
 * @param type : Son type, soit une station, soit une connexion.
 * @param data : Les données de la station, ou de la connexion.
 * @param uv : user_value qui définie une clé de tri pour les listes chaînées de trucs.
 * @return : Renvoi la structure truc créé, ou NULL en cas d'erreur.
 */
Un_truc *creer_truc(Une_coord coord, Ttype type, Tdata data, double uv) {
    Un_truc *truc = (Un_truc *)malloc(sizeof(Un_truc));
    if (truc == NULL) {
        return NULL;
    }
    
    truc->coord = coord;
    truc->type = type;
    truc->data = data;
    truc->user_val = (float)uv;
    
    return truc;
}

/**
 * Fonction qui libère la mémoire d'un truc.
 * @param truc : Le truc qui doit être supprimé de la mémoire.
 */
void detruire_truc(Un_truc *truc) {
    if (truc == NULL) {
        return;
    }
    
    if (truc->type == STA) {
        // On libère le nom de la station
        if (truc->data.sta.nom != NULL) {
            free(truc->data.sta.nom);
        }
        // On libère les connexions puis le tableau de connexions
        if (truc->data.sta.tab_con != NULL) {
            // On libère chaque connexion pointée par le tableau
            for (unsigned int i = 0; i < truc->data.sta.nb_con; i++) {
                if (truc->data.sta.tab_con[i] != NULL) {
                    free(truc->data.sta.tab_con[i]);
                }
            }
            // Puis on libère le tableau lui-même
            free(truc->data.sta.tab_con);
        }
    }
    // Pour les connexions (CON), pas de libération particulière
    
    free(truc);
}
