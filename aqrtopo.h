/*
 * PROJET MAIN3 / METRO
 * Définition de la gestion des arbres quaternaires.
*/

#ifndef AQRTOPO_H
#define AQRTOPO_H

#include "truc.h"
#include "liste.h"

/**
 * Définition d'un nœud d'un arbre quaternaire.
 * Un nœud de cet arbre peut avoir entre 0 et 4 fils suivant la répartition des objets dans les 4 partitions que ce
 * sous-arbre contient.
 */
typedef struct _un_noeud {
    Un_truc *truc;          ///< Une station ou une connexion
    Une_coord limite_no;    ///< Limite zone nord ouest.
    Une_coord limite_se;    ///< Limite zone sud est.
    struct _un_noeud *no;   ///< Fils pour quart NO
    struct _un_noeud *so;   ///< Fils pour quart SO
    struct _un_noeud *ne;   ///< Fils pour quart NE
    struct _un_noeud *se;   ///< Fils pour quart SE
} Un_noeud;

// Prototypes des fonctions
Un_noeud *inserer_aqr(Un_noeud *aqr, Une_coord limite_no, Une_coord limite_se, Un_truc *truc);
Un_noeud *construire_aqr(Un_elem *liste);
void detruire_aqr(Un_noeud *aqr);
Un_truc *chercher_aqr(Un_noeud *aqr, Une_coord coord);
Un_elem *chercher_zone(Un_noeud *aqr, Un_elem *liste, Une_coord limite_no, Une_coord limite_se);

#endif
