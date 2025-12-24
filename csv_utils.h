/*
 * PROJET MAIN3 / METRO
 * Utilitaires pour la lecture de fichiers CSV.
 */

#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#define CSV_TAMPON_TAILLE 512

// Codes d'erreur renvoyés lors de la lecture d'un CSV
#define CSV_OK 0                         // Aucun problème détecté.
#define CSV_ERR_TEXTE 1                  // Champ texte invalide ou trop long.
#define CSV_ERR_FLOAT 2                  // Valeur numérique invalide.
#define CSV_ERR_ALLOC 3                  // Erreur d'allocation mémoire.
#define CSV_ERR_REFERENCE 4              // Référence inconnue (ligne ou station).
#define CSV_ERR_CODE_LIGNE 5             // Code de ligne manquant ou invalide.
#define CSV_ERR_VITESSE 6                // Vitesse manquante ou invalide.
#define CSV_ERR_INTERVALLE 7             // Intervalle manquant ou invalide.
#define CSV_ERR_COULEUR 8                // Couleur manquante ou invalide.
#define CSV_ERR_STATION_DEP 9            // Station de départ manquante ou invalide.
#define CSV_ERR_STATION_ARR 10           // Station d'arrivée manquante ou invalide.
#define CSV_ERR_TEMPS 11                 // Temps de parcours manquant ou invalide.
#define CSV_ERR_LIGNE_INCONNUE 12        // Ligne inconnue dans la configuration.
#define CSV_ERR_STATION_DEP_INCONNUE 13  // Station de départ inconnue.
#define CSV_ERR_STATION_ARR_INCONNUE 14  // Station d'arrivée inconnue.

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
int csv_lire_flottant_valide(Un_lecteur_csv *lecteur, float *valeur);
int csv_lire_entier(Un_lecteur_csv *lecteur);
int csv_est_fini(Un_lecteur_csv *lecteur);

#endif
