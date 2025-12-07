/*
 * PROJET MAIN3 / METRO
 * Définition des connexions.
 * Une connexion correspond à une fraction de ligne unidirectionnelle reliant deux stations.
*/

#ifndef CONNEXION_H
#define CONNEXION_H

#include <stdio.h>

typedef struct _une_ligne Une_ligne;
typedef struct _un_nabr Un_nabr;
typedef struct _un_elem Un_elem;

/**
 * Une connexion correspond à une fraction de ligne unidirectionnelle reliant deux stations.
 * La structure correspondante contient des liens vers les stations de départ et d'arrivée ainsi qu'un lien
 * vers le descriptif dela ligne correspondante.
 */
typedef struct _une_connexion {
    struct _un_truc *sta_dep;   ///< Station de départ.
    struct _un_truc *sta_arr;   ///< Station d'arrivée.
    Une_ligne *ligne;           ///< Ligne.
} Une_connexion;

// Prototypes des fonctions
Un_elem *lire_connexions(char *nom_fichier, Une_ligne *liste_ligne, Un_nabr *abr_sta);
void ecrire_connexions(FILE *flux, Un_elem *liste_con);

#endif
