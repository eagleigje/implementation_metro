/*
 * PROJET MAIN3 / METRO
 * Définition de la structure pour stocker les arbres binaires de recherche.
*/

#ifndef ABR_TYPE_H
#define ABR_TYPE_H

#include "truc.h"

typedef struct _un_nabr {
    Un_truc *truc; // La station
    struct _un_nabr *g; // Fils gauche strictement inférieur
    struct _un_nabr *d; // Fils droit
} Un_nabr;

#endif
