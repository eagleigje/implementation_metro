/*
 * PROJET MAIN3 / METRO
 * Utilitaires pour la lecture de fichiers CSV.
 */

#include "csv_utils.h"
#include <stdlib.h>
#include <string.h>

/**
 * Initialise un lecteur CSV pour parcourir une ligne.
 * Le lecteur est positionné au début de la ligne, afin de lire le premier champ.
 * @param lecteur : Pointeur vers le lecteur à initialiser.
 * @param ligne : La ligne CSV à parcourir (ne doit pas être NULL).
 * @param separateur : Le caractère séparateur (';' généralement).
 */
void csv_init_lecteur(Un_lecteur_csv *lecteur, char *ligne, char separateur) {
    lecteur->ptr_courant = ligne;
    lecteur->separateur = separateur;
    lecteur->fini = 0;
    lecteur->tampon[0] = '\0';
}

/**
 * Avance au champ suivant dans la ligne CSV.
 * Cette fonction extrait le prochain champ et le place dans le tampon interne.
 * Les espaces en début et fin de champ sont automatiquement supprimés.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : 1 si un champ a été lu avec succès, 0 si nous sommes en fin de ligne ou s'il y a une erreur.
 */
int csv_champ_suivant(Un_lecteur_csv *lecteur) {
    // Si déjà fini ou pointeur NULL, retourner 0
    if (lecteur->fini || lecteur->ptr_courant == NULL || *lecteur->ptr_courant == '\0') {
        lecteur->fini = 1;
        return 0;
    }

    // Sauter les espaces en début de champ
    while (*lecteur->ptr_courant == ' ') {
        lecteur->ptr_courant++;
    }

    // Vérifier si on est à la fin après avoir sauté les espaces
    if (*lecteur->ptr_courant == '\0' || *lecteur->ptr_courant == '\n') {
        lecteur->fini = 1;
        return 0;
    }

    // Copier le champ dans le tampon jusqu'au séparateur, fin de ligne ou \n
    int i = 0;
    while (*lecteur->ptr_courant != lecteur->separateur &&
           *lecteur->ptr_courant != '\0' &&
           *lecteur->ptr_courant != '\n' &&
           i < CSV_TAMPON_TAILLE - 1) {
        lecteur->tampon[i++] = *lecteur->ptr_courant++;
    }
    lecteur->tampon[i] = '\0';

    // Enlever les espaces en fin de champ
    while (i > 0 && lecteur->tampon[i - 1] == ' ') {
        lecteur->tampon[--i] = '\0';
    }

    // Sauter le séparateur si présent
    if (*lecteur->ptr_courant == lecteur->separateur) {
        lecteur->ptr_courant++;
    }

    return 1; // Champ lu avec succès
}

/**
 * Récupère le champ courant comme chaîne de caractères.
 * Le champ est déjà nettoyé (espaces en début/fin enlevés) par csv_champ_suivant().
 *
 * ATTENTION : Le pointeur retourné pointe vers le tampon interne du lecteur.
 * S'il est libéré, il sera écrasé au prochain appel de csv_champ_suivant().
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : Pointeur vers le champ dans le tampon interne.
 */
char *csv_lire_texte(Un_lecteur_csv *lecteur) {
    return lecteur->tampon;
}

/**
 * Récupère le champ courant comme nombre flottant.
 * Utilise atof() pour la conversion.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : La valeur convertie en float. Retourne 0.0 si le champ ne peut pas être converti.
 */
float csv_lire_flottant(Un_lecteur_csv *lecteur) {
    return atof(lecteur->tampon);
}

/**
 * Récupère le champ courant comme entier.
 * Utilise atoi() pour la conversion.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : La valeur convertie en int. Retourne 0 si le champ ne peut pas être converti.
 */
int csv_lire_entier(Un_lecteur_csv *lecteur) {
    return atoi(lecteur->tampon);
}

/**
 * Vérifie si le lecteur a atteint la fin de la ligne.
 * Utile pour détecter si tous les champs ont été lus.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : 1 si fin de ligne atteinte, 0 sinon.
 */
int csv_est_fini(Un_lecteur_csv *lecteur) {
    return lecteur->fini;
}
