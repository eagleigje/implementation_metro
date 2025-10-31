/*
 * PROJET MAIN3 / METRO
 * Définition des connexions.
 * Une connexion correspond à une fraction de ligne unidirectionnelle reliant deux stations.
*/

#ifndef CONNEXION_H
#define CONNEXION_H

typedef struct _une_ligne Une_ligne;

typedef struct _une_connexion {
    struct _un_truc *sta_dep; // Station de depart
    struct _un_truc *sta_arr; // Station d'arrivée
    Une_ligne *ligne; // Ligne
} Une_connexion;

#endif
