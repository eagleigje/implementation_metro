/*
 * PROJET MAIN3 / METRO
 * Définition des lignes. Une ligne est définie par son code, la vitesse moyenne des rames qui
 * l’utilisent, l’intervalle moyen entre deux rames et la couleur à utiliser pour la dessiner.
*/

#ifndef LIGNE_H
#define LIGNE_H

typedef struct _une_ligne {
    char *code; // Le nom de la ligne A, B .., M1, M2, T1...
    char *color; // La couleur de la ligne #RRGGBB
    float vitesse; // Vitesse moyenne des rames en km/h
    float intervalle; // Intervalle moyen entre 2 rames
    struct _une_ligne *suiv;
} Une_ligne;

// Prototypes des fonctions
Une_ligne *lire_lignes(char *nom_fichier);
void afficher_lignes(Une_ligne *lligne);
void detruire_lignes(Une_ligne *lligne);
Une_ligne *chercher_ligne(Une_ligne *lligne, char *code);

#endif
