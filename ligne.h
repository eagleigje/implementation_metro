/*
 * PROJET MAIN3 / METRO
 * Définition des lignes. Une ligne est définie par son code, la vitesse moyenne des rames qui
 * l’utilisent, l’intervalle moyen entre deux rames et la couleur à utiliser pour la dessiner.
*/

#ifndef LIGNE_H
#define LIGNE_H

#define CSV_MAX_LINE_LENGTH 256     // Taille maximale d'une ligne dans les fichiers CSV.
#define LINE_MAX_CODE_SIZE 5        // La taille max du code d'une ligne de métro y compris le \0.
#define LINE_COLOR_SIZE 8           // La taille de la couleur d'une ligne exemple #808080, y compris le \0.

/**
 *  Une ligne est définie par son code, la vitesse moyenne des rames qui l’utilisent,
 *  l’intervalle moyen entre deux rames et la couleur à utiliser pour la dessiner.
 */
typedef struct _une_ligne {
    char *code;                 ///< Le nom de la ligne A, B .., M1, M2, T1...
    char *color;                ///< La couleur de la ligne au format RRGGBB.
    float vitesse;              ///< Vitesse moyenne des rames en km/h.
    float intervalle;           ///< Intervalle moyen entre 2 rames
    struct _une_ligne *suiv;    ///< Elément suivant de la liste chaînée.
} Une_ligne;

// Prototypes des fonctions
Une_ligne *lire_lignes(char *nom_fichier);
void afficher_lignes(Une_ligne *ligne);
void detruire_lignes(Une_ligne *ligne);
Une_ligne *chercher_ligne(Une_ligne *ligne, char *code);

#endif
