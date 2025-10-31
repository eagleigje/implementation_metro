/*
 * PROJET MAIN3 / METRO
 * Définition de la structure pour stocker les arbres binaires de recherche.
*/

#ifndef ABR_TYPE_H
#define ABR_TYPE_H

#include "truc.h"

/**
 * Arbre binaire de recherche.
 */
typedef struct _un_nabr {
    Un_truc *truc;      ///< Les informations concernant la station
    struct _un_nabr *g; ///< Fils gauche strictement inférieur
    struct _un_nabr *d; ///< Fils droit
} Un_nabr;

#endif
