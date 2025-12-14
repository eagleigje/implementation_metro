/*
*  PROJET MAIN3 / METRO
 * Gestion des arbres binaires de recherche.
*/

#include "abr.h"

#include <stdlib.h>
#include <string.h>
#include <strings.h>

/**
 * Fonction qui réalise l'allocation d'un nœud de l'arbre binaire de recherche et son initialisation.
 * @param truc : Le truc qu'il faut utiliser pour le nouveau nœud.
 * @return : Renvoi le nouveau nœud créé.
 */
Un_nabr *creer_nabr(Un_truc *truc) {
    // Allocation mémoire.
    Un_nabr *nabr = (Un_nabr *)malloc(sizeof(Un_nabr));

    // Au départ le nœud n'a pas de fils.
    nabr->truc = truc;
    nabr->d = NULL;
    nabr->g = NULL;

    return nabr;
}

/**
 * Fonction qui insère un nouveau nœud dans un arbre binaire de recherche.
 * Cette fonction est récursive.
 * @param abr : L'arbre binaire de recherche dans lequel insérer le nœud n.
 * @param n : Le nœud que l'on veut insérer dans l'arbre.
 * @return : L'arbre modifié, NULL en cas d'erreur.
 */
Un_nabr *inserer_abr(Un_nabr *abr, Un_nabr *n) {
    // Cas de base : position d'insertion trouvée (arbre vide ou feuille atteinte)
    // On retourne le nouveau nœud qui sera attaché à l'arbre.
    if (abr == NULL) {
        return n;
    };

    // Nous allons comparer le nom de la station de n par rapport à la liste des noeuds.
    // strcasecmp(s1, s2) renvoi un entier tel que :
    // Une valeur égale à 0 si s1 et s2 sont égales (insensible à la casse).
    // Une valeur négative si s1 est inférieure à s2.
    // Une valeur positive si s1 est supérieure à s2.
    int cmp = strcasecmp(n->truc->data.sta.nom, abr->truc->data.sta.nom);

    if (cmp < 0) {
        // On insère à gauche (nom strictement inférieur).
        abr->g = inserer_abr(abr->g, n);
    } else {
        // On insère à droite (nom supérieur ou égal).
        abr->d = inserer_abr(abr->d, n);
    }

    return abr;
}

/**
 * Fonction qui construit un ABR à partir d'une liste de stations.
 * @param liste_sta : La liste des stations triée par user_value à transformer en ABR.
 * @return : L'ABR construit.
 */
Un_nabr *construire_abr(Un_elem *liste_sta) {
    // L'arbre est vide au départ.
    Un_nabr *abr = NULL;
    Un_elem *courant = liste_sta;

    // Nous parcourons la liste des stations jusqu'à la fin.
    while (courant != NULL) {
        // S'il ne s'agit d'une station, on saute (ce qui ne devrait pas arriver).
        if (courant->truc->type != STA) continue;
        // On crée le nouveau nœud.
        Un_nabr *nouveau = creer_nabr(courant->truc);
        if (nouveau != NULL) {
            // Et on l'insère au bon endroit dans l'ABR.
            abr = inserer_abr(abr, nouveau);
        }
        courant = courant->suiv;
    }

    return abr;
}

/**
 * Fonction qui libère la mémoire d'un arbre ABR.
 * Cette fonction est récursive.
 * @param abr : L'arbre ABR à détruire.
 */
void detruire_abr(Un_nabr *abr) {
    // Cas de base : Arbre vide ou feuille atteinte, afin de remonter jusqu'à la racine.
    if (abr == NULL) {
        return;
    }
    // On parcourt les deux côtés du nœud actuel.
    detruire_abr(abr->g);
    detruire_abr(abr->d);
    // On libère seulement le nœud de l'ABR ?
    free(abr);
}

/**
 * Fonction qui cherche une station par son nom, dans l'ABR.
 * Cette fonction est récursive et insensible à la casse.
 * @param abr : ABR à parcourir.
 * @param nom : Le nom à rechercher (insensible à la casse).
 * @return : Le truc correspond à la recherche ou NULL si non trouvé.
 */
Un_truc *chercher_station(Un_nabr *abr, char *nom) {
    // Si l'arbre est vide ou si on est arrivé sur une feuille sans trouver la station.
    if (abr == NULL) {
        return NULL;
    }

    // Comparaison insensible à la casse avec strcasecmp
    int cmp = strcasecmp(nom, abr->truc->data.sta.nom);
    if (cmp == 0) {
        // La station a été trouvée.
        return abr->truc;
    }
    else if (cmp < 0) {
        // On part sur la gauche du nœud actuel.
        return chercher_station(abr->g, nom);
    }
    else {
        // On part sur la droite du nœud actuel.
        return chercher_station(abr->d, nom);
    }
}