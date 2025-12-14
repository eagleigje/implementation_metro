/*
 * PROJET MAIN3 / METRO
 * Utilitaires pour la lecture de fichiers CSV.
 */

#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stddef.h>

#define CSV_TAMPON_TAILLE 512

/**
 * Lecteur CSV pour parcourir les champs d'une ligne, un à un.
 */
typedef struct _un_lecteur_csv {
    char *ptr_courant;              ///<  Pointeur vers le début de la ligne à parcourir
    char separateur;                ///<  Le caractère séparateur CSV à utiliser
    char tampon[CSV_TAMPON_TAILLE]; ///<  Tampon qui permet de conserver le champ extrait du CSV.
    int fini;                       ///<  Indicateur de fin de ligne (0 = pas fini, 1 = fini)
} Un_lecteur_csv;

// Prototypes des fonctions
void csv_init_lecteur(Un_lecteur_csv *lecteur, char *ligne, char separateur);
int csv_champ_suivant(Un_lecteur_csv *lecteur);
char *csv_lire_texte(Un_lecteur_csv *lecteur, size_t max_size);
float csv_lire_flottant(Un_lecteur_csv *lecteur);
int csv_lire_entier(Un_lecteur_csv *lecteur);
int csv_est_fini(Un_lecteur_csv *lecteur);

#endif
