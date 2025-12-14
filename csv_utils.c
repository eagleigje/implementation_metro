/*
 * PROJET MAIN3 / METRO
 * Utilitaires pour la lecture de fichiers CSV.
 */

#include <stdlib.h>
#include <string.h>

#include "csv_utils.h"
#include "outils.h"

/**
 * Fonction d'initialisation d'un lecteur CSV pour parcourir une ligne.
 * Le lecteur est positionné au début de la ligne, afin de lire le premier champ.
 * @param lecteur : Pointeur vers le lecteur à initialiser.
 * @param ligne : La ligne CSV à parcourir (ne doit pas être NULL).
 * @param separateur : Le caractère séparateur (';' généralement).
 */
void csv_init_lecteur(Un_lecteur_csv *lecteur, char *ligne, char separateur) {
    lecteur->ptr_courant = ligne;       // Pointeur vers le début de la ligne à parcourir
    lecteur->separateur = separateur;   // Le caractère séparateur CSV à utiliser
    lecteur->fini = 0;                  // Indicateur de fin de ligne (0 = pas fini, 1 = fini)
    lecteur->tampon[0] = '\0';          // Initialisation du tampon vide
}

/**
 * Fonction qui permet d'avancer au champ suivant dans la ligne CSV.
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

    // On vérifie si on est arrivé à la fin de ligne
    if (*lecteur->ptr_courant == '\n') {
        lecteur->fini = 1;
        return 0;
    }

    // On recopie le champ dans le tampon jusqu'au séparateur, fin de ligne ou \n
    int i = 0;
    while (*lecteur->ptr_courant != lecteur->separateur &&
           *lecteur->ptr_courant != '\0' &&
           *lecteur->ptr_courant != '\n' &&
           i < CSV_TAMPON_TAILLE - 1) {
        lecteur->tampon[i++] = *lecteur->ptr_courant++;
    }
    lecteur->tampon[i] = '\0';

    // On utilise trim() pour enlever les espaces en début et fin
    trim(lecteur->tampon);

    // Et on saute le séparateur s'il est présent
    if (*lecteur->ptr_courant == lecteur->separateur) {
        lecteur->ptr_courant++;
    }

    return 1; // Champ lu avec succès
}

/**
 * Fonction de récupèration du champ courant comme chaîne de caractères.
 * Le champ est déjà nettoyé (espaces en début/fin enlevés) par csv_champ_suivant().
 *
 * ATTENTION : Le pointeur retourné pointe vers le tampon interne du lecteur.
 * S'il est libéré, il sera écrasé au prochain appel de csv_champ_suivant().
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @param max_size : Taille maximale attendue pour ce champ (incluant le '\0').
 * @return : Pointeur vers le champ dans le tampon interne, ou NULL si le champ dépasse max_size.
 */
char *csv_lire_texte(Un_lecteur_csv *lecteur, size_t max_size) {
    size_t longueur = strlen(lecteur->tampon);
    // On vérifie que le champ peut tenir dans un buffer de taille max_size (avec '\0').
    if (longueur >= max_size) {
        return NULL;  // Champ trop grand
    }
    return lecteur->tampon;
}

/**
 * Fonction de récupèration du champ courant en tant que nombre flottant.
 * Utilise atof() pour la conversion.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : La valeur convertie en float. Retourne 0.0 si le champ ne peut pas être converti.
 */
float csv_lire_flottant(Un_lecteur_csv *lecteur) {
    return atof(lecteur->tampon);
}

/**
 * Fonction de récupèration du champ courant en tant qu'entier.
 * Utilise atoi() pour la conversion.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : La valeur convertie en int. Retourne 0 si le champ ne peut pas être converti.
 */
int csv_lire_entier(Un_lecteur_csv *lecteur) {
    return atoi(lecteur->tampon);
}

/**
 * Fonction qui vérifie si le lecteur a atteint la fin de la ligne.
 * Permet de détecter si tous les champs ont bien été lus.
 *
 * @param lecteur : Pointeur vers le lecteur.
 * @return : 1 si fin de ligne atteinte, 0 sinon.
 */
int csv_est_fini(Un_lecteur_csv *lecteur) {
    return lecteur->fini;
}
