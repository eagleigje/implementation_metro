/*
 * PROJET MAIN3 / METRO
 * Librairie permettant la manipulation de listes simplement chaînées de trucs.
*/

#ifndef LISTE_H
#define LISTE_H

#include <stdio.h>
#include "truc.h"

/**
 * Liste chaînée de "trucs".
 */
typedef struct _un_elem {
    Un_truc *truc;          ///< Une station ou une connexion
    struct _un_elem *suiv;  ///< Element suivant de la liste chaînée.
} Un_elem;

// Prototypes des fonctions
Un_elem *inserer_liste_trie(Un_elem *liste, Un_truc *truc);
Un_elem *inserer_deb_liste(Un_elem *liste, Un_truc *truc);
void ecrire_liste(FILE *flux, Un_elem *liste);
void detruire_liste(Un_elem *liste);
void detruire_liste_et_truc(Un_elem *liste);
void limites_zone(Un_elem *liste, Une_coord *limite_no, Une_coord *limite_se);
Un_elem *lire_stations(char *nom_fichier);
Un_truc *extraire_deb_liste(Un_elem **liste);
Un_truc *extraire_liste(Un_elem **liste, Un_truc *truc);

#endif
